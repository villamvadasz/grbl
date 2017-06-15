#ifndef _TASK_H_
#define _TASK_H_

	#include "k_stdtype.h"
	#include "..\c_task.h"

	typedef void (*Taskptr)(void);

	typedef sint32 TaskId;

	typedef enum _TaskActiveType {
		TaskActiveType_NotActive = 0,
		TaskActiveType_Cyclic = 1,
		TaskActiveType_Trigger = 2,
		TaskActiveType_SingleShootTimed = 2,
		TaskActiveType_Delete = 9,
	} TaskActiveType;

	typedef struct _TaskItem {
		Taskptr ptr;
		#ifndef TASK_COMPACT_MODE
			uint32 activationCycle;
			uint32 activationCycleTemp;
		#else
			uint16 activationCycle;
			uint16 activationCycleTemp;
		#endif
		#ifndef TASK_COMPACT_MODE
			uint32 startTime;
			uint32 stopTime;
			uint32 runTime;
			uint32 totalTime;
		#endif
		TaskActiveType active;
		#ifndef TASK_COMPACT_MODE
			const sint8 *str;
		#endif
	} TaskItem;

	extern uint32 tasksTotalTime;

	extern void init_task(void);
	extern void deinit_task(void);
	extern void do_task(void);
	extern void isr_task_1ms(void);

	extern TaskId addTask(Taskptr ptr, uint32 activationCycle, uint8 active, const sint8 *str);
	extern void setTaskName(TaskId taskNumber, const sint8 *str);
	extern TaskId addSingleShootTask(Taskptr ptr, const sint8 *str);
	extern TaskId addSingleShootTimedTask(Taskptr ptr, uint32 activationCycle, const sint8 *str);
	extern TaskId addTriggerTask(Taskptr ptr, const sint8 *str);
	extern void removeTask(TaskId taskNumber);
	extern void suspendTask(TaskId taskNumber);
	extern void resumeTask(TaskId taskNumber);
	extern void scheduleTask(TaskId taskNumber);
	extern void changeActivationCycleTask(TaskId taskNumber, uint32 activationCycle);
	extern void triggerTask(TaskId taskNumber);

#endif
