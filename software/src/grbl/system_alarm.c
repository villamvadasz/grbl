#include "grbl.h"

#include "k_stdtype.h"
#include "mal.h"

// Global realtime executor bitflag variable for setting various alarms.
volatile sys_rt_exec_alarm_t sys_rt_exec_alarm = 0;

void init_system_alarm(void) {
	sys_rt_exec_alarm = 0;
}

void system_set_exec_alarm(sys_rt_exec_alarm_t code) {
	lock_isr();
	sys_rt_exec_alarm = code;
	unlock_isr();
}

sys_rt_exec_alarm_t system_get_exec_alarm(void) {
	sys_rt_exec_alarm_t result = 0;
	lock_isr();
	result = sys_rt_exec_alarm;
	unlock_isr();
	return result;
}

void system_clear_exec_alarm(void) {
	lock_isr();
	sys_rt_exec_alarm = 0;
	unlock_isr();
}
