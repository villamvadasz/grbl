#ifndef _C_TASK_H_
#define _C_TASK_H_

	#error You have included an example of c_task.h

	//#define TASK_COMPACT_MODE
	//#define TASK_LOOP_UNTIL_FIRST_HIT
	//#define TASK_LOOP_UNTIL_ALL

	#define TASKLISTCNT 32
	#define taskShortageCallout()	taskShortageCalloutUser()
	#define taskSwitchStartCall(a)	;
	#define taskSwitchStopCall(a)	;

#endif
