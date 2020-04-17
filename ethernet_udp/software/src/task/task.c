#include <stdio.h>
#include "task.h"
#include "c_task.h"

#include "k_stdtype.h"
#include "tmr.h"
#include "mal.h"

TaskItem taskLists[TASKLISTCNT];
uint32 tasksTotalTime = 0;
uint8 doTaskList1ms = 0;
uint8 doTaskListGuard = 0;
uint32 taskListsPtr = 0;

void clearTaskItem(TaskId taskNumber);
void taskHelperActivator(TaskId taskNumber);
TaskId findFreeTaskList(void);

void init_task(void) {
	uint32 i = 0;
	for (i = 0; i < TASKLISTCNT; i++) {
		clearTaskItem(i);
	}
}

void deinit_task(void) {
	uint32 i = 0;
	for (i = 0; i < TASKLISTCNT; i++) {
		clearTaskItem(i);
	}
}

void do_task(void) {
	if (doTaskList1ms) {
		{
			#ifdef TASK_LOOP_UNTIL_FIRST_HIT
				uint8 taskRunLoop = 1;
			#endif
			#ifdef TASK_LOOP_UNTIL_FIRST_HIT
				while (taskRunLoop) {
			#endif
				if (taskListsPtr < TASKLISTCNT) {
					if (taskLists[taskListsPtr].ptr != NULL) {
						if (	(taskLists[taskListsPtr].activationCycle != 0) && 
								((taskLists[taskListsPtr].active == TaskActiveType_Cyclic) || (taskLists[taskListsPtr].active == TaskActiveType_SingleShootTimed))
							) {
							taskLists[taskListsPtr].activationCycleTemp++;
							if (taskLists[taskListsPtr].activationCycleTemp >= taskLists[taskListsPtr].activationCycle) {
								taskLists[taskListsPtr].activationCycleTemp = 0;
								taskHelperActivator(taskListsPtr);
								#ifdef TASK_LOOP_UNTIL_FIRST_HIT
									#ifndef TASK_LOOP_UNTIL_ALL
										taskRunLoop = 0;
									#endif
								#endif
								if (taskLists[taskListsPtr].active == TaskActiveType_SingleShootTimed) {
									clearTaskItem(taskListsPtr); //Remove is only allowed for user, and can not delete a task that is running.
								}
							}
						} else if (taskLists[taskListsPtr].active == TaskActiveType_Trigger) {
							taskHelperActivator(taskListsPtr);
							#ifdef TASK_LOOP_UNTIL_FIRST_HIT
								#ifndef TASK_LOOP_UNTIL_ALL
									taskRunLoop = 0;
								#endif
							#endif
							clearTaskItem(taskListsPtr); //Remove is only allowed for user, and can not delete a task that is running.
						}
					}
					taskListsPtr++;
				} else {
					#ifdef TASK_LOOP_UNTIL_FIRST_HIT
						taskRunLoop = 0;
					#endif
					if (doTaskListGuard) {
						taskShortageCallout();
					}
					taskListsPtr = 0;
					doTaskList1ms = 0;
					doTaskListGuard = 0;
				}
			#ifdef TASK_LOOP_UNTIL_FIRST_HIT
				}
			#endif
		}
	}
}

void isr_task_1ms(void) {
	doTaskList1ms = 1;
	doTaskListGuard = 1;
}

void clearTaskItem(TaskId taskNumber) {
	taskLists[taskNumber].ptr = NULL;
	taskLists[taskNumber].activationCycle = 0;
	taskLists[taskNumber].activationCycleTemp = 0;
	#ifndef TASK_COMPACT_MODE
		taskLists[taskNumber].startTime = 0;
		taskLists[taskNumber].stopTime = 0;
		taskLists[taskNumber].runTime = 0;
		taskLists[taskNumber].totalTime = 0;
	#endif
	taskLists[taskNumber].active = TaskActiveType_NotActive;
	#ifndef TASK_COMPACT_MODE
		taskLists[taskNumber].str = NULL;
	#endif
}

TaskId addTask(Taskptr ptr, uint32 activationCycle, uint8 active, const sint8 *str) {
	TaskId result = findFreeTaskList();
	if (result != -1) {
		taskLists[result].ptr = ptr;
		taskLists[result].activationCycle = activationCycle;
		setTaskName(result, str);
		if (active) {
			resumeTask(result);
		} else {
			suspendTask(result);
		}
	}
	return result;
}

TaskId addTriggerTask(Taskptr ptr, const sint8 *str) {
	return addTask(ptr, 0, 0, str);
}

TaskId addSingleShootTimedTask(Taskptr ptr, uint32 activationCycle, const sint8 *str) {
	TaskId result = addTask(ptr, activationCycle, 1, str);
	taskLists[result].active = TaskActiveType_SingleShootTimed;
	return result;
}

TaskId addSingleShootTask(Taskptr ptr, const sint8 *str) {
	TaskId result = addTriggerTask(ptr, str);
	if (result != -1) {
		triggerTask(result);
	}
	return result;
}

TaskId findFreeTaskList(void) {
	TaskId result = -1;
	uint32 i = 0;
	for (i = 0; i < TASKLISTCNT; i++) {
		if (taskLists[i].ptr == NULL) {
			result = i;
			break;
		}
	}
	return result;
}

void removeTask(TaskId taskNumber) {
	if ((taskNumber >= 0) && (taskNumber < TASKLISTCNT)) {
		if (taskNumber != taskListsPtr) {
			clearTaskItem(taskNumber);
		} else {
			taskLists[taskNumber].active = TaskActiveType_Delete;
		}
	}
}

void suspendTask(TaskId taskNumber) {
	taskLists[taskNumber].active = TaskActiveType_NotActive;
}

void resumeTask(TaskId taskNumber) {
	taskLists[taskNumber].active = TaskActiveType_Cyclic;
}

void scheduleTask(TaskId taskNumber) {
	taskLists[taskNumber].activationCycleTemp = taskLists[taskNumber].activationCycle;
}

void changeActivationCycleTask(TaskId taskNumber, uint32 activationCycle) {
	taskLists[taskNumber].activationCycle = activationCycle;
}

void triggerTask(TaskId taskNumber) {
	lock_isr();
	taskLists[taskNumber].active = TaskActiveType_Trigger;
	doTaskList1ms = 1;
	unlock_isr();
}

void taskHelperActivator(TaskId taskNumber) {
	taskSwitchStartCall(&taskLists[taskListsPtr]);
	#ifndef TASK_COMPACT_MODE
		taskLists[taskListsPtr].startTime = getGlobalTimeUs();
	#endif
	taskLists[taskListsPtr].ptr();
	#ifndef TASK_COMPACT_MODE
		taskLists[taskListsPtr].stopTime = getGlobalTimeUs();
	#endif
	taskSwitchStopCall(&taskLists[taskListsPtr]);
	#ifndef TASK_COMPACT_MODE
		taskLists[taskListsPtr].runTime = taskLists[taskListsPtr].stopTime - taskLists[taskListsPtr].startTime;
		taskLists[taskListsPtr].totalTime = taskLists[taskListsPtr].runTime;
		tasksTotalTime += taskLists[taskListsPtr].runTime;
	#endif
	if (taskLists[taskNumber].active == TaskActiveType_Delete) {
		clearTaskItem(taskNumber); //Remove is only allowed for user, and can not delete a task that is running.	
	}
}

void setTaskName(TaskId taskNumber, const sint8 *str) {
	#ifndef TASK_COMPACT_MODE
		taskLists[taskNumber].str = str;
	#endif
}
