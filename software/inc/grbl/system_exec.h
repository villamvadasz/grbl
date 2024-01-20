#ifndef _SYSTEM_EXEC_H_
#define _SYSTEM_EXEC_H_

	#include "k_stdtype.h"
	#include "nuts_bolts.h"

	// Define system executor bit map. Used internally by realtime protocol as realtime command flags,
	// which notifies the main program to execute the specified realtime command asynchronously.
	// NOTE: The system executor uses an unsigned 8-bit volatile variable (8 flag limit.) The default
	// flags are always false, so the realtime protocol only needs to check for a non-zero value to
	// know when there is a realtime command to execute.
	#define EXEC_STATUS_REPORT  bit(0) // bitmask 000000001
	#define EXEC_CYCLE_START    bit(1) // bitmask 000000010
	#define EXEC_CYCLE_STOP     bit(2) // bitmask 000000100
	#define EXEC_FEED_HOLD      bit(3) // bitmask 000001000
	#define EXEC_ABORT          bit(4) // bitmask 000010000
	#define EXEC_SAFETY_DOOR    bit(5) // bitmask 000100000
	#define EXEC_MOTION_CANCEL  bit(6) // bitmask 001000000
	#define EXEC_CLEAR_ALARM    bit(7) // bitmask 010000000
	#define EXEC_PROBE_START	bit(8) // bitmask 100000000
	#define EXEC_ALL         0xFFFF    

	typedef uint16 sys_rt_exec_state_t;

	extern void system_set_exec_state_flag(sys_rt_exec_state_t mask);
	extern void system_clear_exec_state_flag(sys_rt_exec_state_t mask);
	extern sys_rt_exec_state_t system_get_exec_state_flag(void);
	
	extern void init_system_exec(void);

#endif
