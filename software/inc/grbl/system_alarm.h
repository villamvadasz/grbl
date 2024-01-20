#ifndef _SYSTEM_ALARM_H_
#define _SYSTEM_ALARM_H_

	#include "k_stdtype.h"
	#include "nuts_bolts.h"
	
	// Alarm executor codes. Valid values (1-255). Zero is reserved.
	#define EXEC_ALARM_HARD_LIMIT           1
	#define EXEC_ALARM_SOFT_LIMIT           2
	#define EXEC_ALARM_ABORT_CYCLE          3
	#define EXEC_ALARM_PROBE_FAIL_INITIAL   4
	#define EXEC_ALARM_PROBE_FAIL_CONTACT   5

	#define EXEC_ALARM_HOMING_FAIL_DOOR     7
	#define EXEC_ALARM_HOMING_FAIL_PULLOFF  8
	#define EXEC_ALARM_HOMING_FAIL_APPROACH 9
	#define EXEC_ALARM_EEP_MASTER_READ_FAIL 10
	#define EXEC_ALARM_EEP_BACKUP_READ_FAIL 11

	#define EXEC_ALARM_EXTERNAL_POWER_MISSING 12

	typedef uint16 sys_rt_exec_alarm_t;

	extern void system_set_exec_alarm(sys_rt_exec_alarm_t code);
	extern sys_rt_exec_alarm_t system_get_exec_alarm(void);
	extern void system_clear_exec_alarm(void);
	
	extern void init_system_alarm(void);
	
#endif
