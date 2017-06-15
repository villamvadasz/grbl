#include "grbl.h"

#include "k_stdtype.h"
#include "mal.h"

volatile sys_rt_exec_state_t sys_rt_exec_state = 0;				// Global realtime executor bitflag variable for state management. See EXEC bitmasks.

void init_system_exec(void) {
	sys_rt_exec_state = 0;
}

// Special handlers for setting and clearing Grbl's real-time execution flags.
void system_set_exec_state_flag(sys_rt_exec_state_t mask) {
	lock_isr();
	sys_rt_exec_state |= (mask);
	unlock_isr();
}

void system_clear_exec_state_flag(sys_rt_exec_state_t mask) {
	lock_isr();
	sys_rt_exec_state &= ~(mask);
	unlock_isr();
}

sys_rt_exec_state_t system_get_exec_state_flag(void) {
	sys_rt_exec_state_t result = 0;
	lock_isr();
	result = sys_rt_exec_state;
	unlock_isr();
	return result;
}
