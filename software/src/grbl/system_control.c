#include "grbl.h"

#include "k_stdtype.h"
#include "mal.h"

uint8 system_reset_debug = 0; 
volatile uint8 do_system_control_1ms = 0;
volatile uint8 do_system_poll_control_pins = 0;
uint8 system_control_get_state_current_ABORT = 0;
uint8 system_control_get_state_current_FEED_HOLD = 0;
uint8 system_control_get_state_current_CYCLE_START = 0;
uint8 system_control_get_state_current_SAFETY_DOOR = 0;
uint8 system_control_get_state_last_ABORT = 0;
uint8 system_control_get_state_last_FEED_HOLD = 0;
uint8 system_control_get_state_last_CYCLE_START = 0;
uint8 system_control_get_state_last_SAFETY_DOOR = 0;

volatile uint8 system_control_get_state_debug = 0;

void system_control_pins(void);

void init_system_control(void) {
	system_reset_debug = 0; 
	do_system_control_1ms = 0;
	do_system_poll_control_pins = 0;
	system_control_get_state_current_ABORT = 0;
	system_control_get_state_current_FEED_HOLD = 0;
	system_control_get_state_current_CYCLE_START = 0;
	system_control_get_state_current_SAFETY_DOOR = 0;
	system_control_get_state_last_ABORT = 0;
	system_control_get_state_last_FEED_HOLD = 0;
	system_control_get_state_last_CYCLE_START = 0;
	system_control_get_state_last_SAFETY_DOOR = 0;

	system_control_get_state_debug = 0;

	#ifdef CONTROL_ABORT_DDR
		CONTROL_ABORT_DDR = 1; // Configure as input pins
		MAL_SYNC();
	#endif
	#ifdef CONTROL_FEED_HOLD_DDR
		CONTROL_FEED_HOLD_DDR = 1; // Configure as input pins
		MAL_SYNC();
	#endif
	#ifdef CONTROL_CYCLE_START_DDR
		CONTROL_CYCLE_START_DDR = 1; // Configure as input pins
		MAL_SYNC();
	#endif
	#ifdef CONTROL_SAFETY_DOOR_DDR
		CONTROL_SAFETY_DOOR_DDR = 1; // Configure as input pins
		MAL_SYNC();
	#endif

	#ifdef CONTROL_ABORT_PIN_PUE
		CONTROL_ABORT_PIN_PUE = 1;
		MAL_SYNC();
	#endif
	#ifdef CONTROL_FEED_HOLD_PIN_PUE
		CONTROL_FEED_HOLD_PIN_PUE = 1;
		MAL_SYNC();
	#endif
	#ifdef CONTROL_CYCLE_START_PIN_PUE
		CONTROL_CYCLE_START_PIN_PUE = 1;
		MAL_SYNC();
	#endif
	#ifdef CONTROL_SAFETY_DOOR_PIN_PUE
		CONTROL_SAFETY_DOOR_PIN_PUE = 1;
		MAL_SYNC();
	#endif

	#ifdef CONTROL_FEED_HOLD_PIN_CN
		CONTROL_FEED_HOLD_PIN_CN = 1;
		MAL_SYNC();
	#endif
	#ifdef CONTROL_CYCLE_START_PIN_CN
		CONTROL_CYCLE_START_PIN_CN = 1;
		MAL_SYNC();
	#endif
	#ifdef CONTROL_SAFETY_DOOR_PIN_CN
		CONTROL_SAFETY_DOOR_PIN_CN = 1;
		MAL_SYNC();
	#endif
}

void do_system_control(void) {
	system_control_pins();
	#ifdef CONTROL_ABORT_PIN
		if (CONTROL_ABORT_PIN) {
			system_reset_debug = 1;
		} else {
			system_reset_debug = 0;
		}
	#endif
}

void isr_system_control_1ms(void) {
	do_system_control_1ms = 1;
}

// Pin change interrupt for pin-out commands, i.e. cycle start, feed hold, and reset. Sets
// only the realtime command execute variable to have the main program execute these when
// its ready. This works exactly like the character-based realtime commands when picked off
// directly from the incoming serial data stream.
void CONTROL_INT_vect(void) {
	do_system_poll_control_pins = 1;
}

//uC PIN is 1 ==> Emergency abort button is pressed in, so machine must stop everything
//uC PIN is 0 ==> Emergency abort button is released, it is allowed to move
//uC PIN is 1 ==> Function returns 1 => Machine must stop
//uC PIN is 0 ==> Function returns 0
uint8 system_control_get_state_ABORT(void) {
	volatile uint8 pin = 0;
	#ifdef CONTROL_ABORT_PIN
		if (CONTROL_ABORT_PIN) {
			pin = 1;
		} else {
			pin = 0;
		}
	#else
		#if defined(DEFAULTS_PIC32_MX_CNC_1_0_0_DEBUG) || defined (DEFAULTS_PIC32_MX_CNC_1_0_0_SIMULATOR) || defined (DEFAULTS_PIC32_MX_CNC_1_0_2_SIMULATOR)
			pin = 0;
		#else
			pin = 1;
		#endif
	#endif
	return(pin);
}

uint8 system_control_get_state_FEED_HOLD(void) {
	volatile uint8 pin = 0;
	#ifdef CONTROL_FEED_HOLD_PIN
		if (CONTROL_FEED_HOLD_PIN) {
			#ifdef INVERT_CONTROL_PIN_FEED_HOLD
				pin = 0;
			#else
				pin = 1;
			#endif
		} else {
			#ifdef INVERT_CONTROL_PIN_FEED_HOLD
				pin = 1;
			#else
				pin = 0;
			#endif
		}
	#endif
	return(pin);
}

uint8 system_control_get_state_CYCLE_START(void) {
	volatile uint8 pin = 0;
	#ifdef CONTROL_CYCLE_START_PIN
		if (CONTROL_CYCLE_START_PIN) {
			#ifdef INVERT_CONTROL_PIN_CYCLE_START
				pin = 0;
			#else
				pin = 1;
			#endif
		} else {
			#ifdef INVERT_CONTROL_PIN_CYCLE_START
				pin = 1;
			#else
				pin = 0;
			#endif
		}
	#endif
	return(pin);
}

uint8 system_control_get_state_SAFETY_DOOR(void) {
	volatile uint8 pin = 0;
	#ifdef CONTROL_SAFETY_DOOR_PIN
		if (CONTROL_SAFETY_DOOR_PIN) {
			#ifdef INVERT_CONTROL_PIN_SAFETY_DOOR
				pin = 0;
			#else
				pin = 1;
			#endif
		} else {
			#ifdef INVERT_CONTROL_PIN_SAFETY_DOOR
				pin = 1;
			#else
				pin = 0;
			#endif
		}
	#endif
	return(pin);
}

void system_control_pins(void) {
	lock_isr();
	if (do_system_control_1ms) {
		do_system_control_1ms = 0;
		do_system_poll_control_pins = 1;
	}
	unlock_isr();
	if (do_system_poll_control_pins) {
		do_system_poll_control_pins = 0;
		
		system_control_get_state_current_ABORT = system_control_get_state_ABORT();
		system_control_get_state_current_FEED_HOLD = system_control_get_state_FEED_HOLD();
		system_control_get_state_current_CYCLE_START = system_control_get_state_CYCLE_START();
		system_control_get_state_current_SAFETY_DOOR = system_control_get_state_SAFETY_DOOR();

		system_control_get_state_debug = system_control_get_state_current_ABORT | system_control_get_state_current_FEED_HOLD | system_control_get_state_current_CYCLE_START | system_control_get_state_current_SAFETY_DOOR;
		
		//For this pin it is a safer strategy to issue an abort for booth direction of change. User have to ack this any way.
		if ( (system_control_get_state_current_ABORT != 0) && (system_control_get_state_last_ABORT == 0) ) {
			system_set_exec_state_flag(EXEC_ABORT);
			system_log_sm_abort_input();
		} 
		if ( (system_control_get_state_current_ABORT == 0) && (system_control_get_state_last_ABORT != 0) ) {
			system_set_exec_state_flag(EXEC_ABORT);
			system_log_sm_abort_input();
		}
		
		if ( (system_control_get_state_current_FEED_HOLD != 0) && (system_control_get_state_last_FEED_HOLD == 0) ) {
			system_set_exec_state_flag(EXEC_FEED_HOLD);
		} 
		if ( (system_control_get_state_current_CYCLE_START != 0) && (system_control_get_state_last_CYCLE_START == 0) ) {
			system_set_exec_state_flag(EXEC_CYCLE_START);
		}
		if ( (system_control_get_state_current_SAFETY_DOOR != 0) && (system_control_get_state_last_SAFETY_DOOR == 0) ) {
			system_set_exec_state_flag(EXEC_SAFETY_DOOR);
		}
		system_control_get_state_last_ABORT = system_control_get_state_current_ABORT;
		system_control_get_state_last_FEED_HOLD = system_control_get_state_current_FEED_HOLD;
		system_control_get_state_last_CYCLE_START = system_control_get_state_current_CYCLE_START;
		system_control_get_state_last_SAFETY_DOOR = system_control_get_state_current_SAFETY_DOOR;
	}
}