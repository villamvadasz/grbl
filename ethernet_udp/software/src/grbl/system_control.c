#include "k_stdtype.h"
#include "system_control.h"

#include "mal.h"
#include "system.h"
#include "system_exec.h"

uint8 system_reset_debug = 0; 
volatile uint8 do_system_control_1ms = 0;
volatile uint8 do_system_poll_control_pins = 0;
uint8 system_control_get_state_current = 0;
uint8 system_control_get_state_last = 0;
volatile uint8 system_control_get_state_debug = 0;

static void system_control_pins(void);

void init_system_control(void) {
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

// Returns control pin state as a uint8 bitfield. Each bit indicates the input pin state, where
// triggered is 1 and not triggered is 0. Invert mask is applied. Bitfield organization is
// defined by the CONTROL_PIN_INDEX in the header file.
uint8 system_control_get_state(void) {
	volatile uint8 control_state = 0;
	volatile uint8 pin = 0;
	#ifdef CONTROL_ABORT_PIN
		if (CONTROL_ABORT_PIN) {
			pin |= (1 << CONTROL_ABORT_BIT);
		}
	#endif
	#ifdef CONTROL_FEED_HOLD_PIN
		if (CONTROL_FEED_HOLD_PIN) {
			pin |= (1 << CONTROL_FEED_HOLD_BIT);
		}
	#endif
	#ifdef CONTROL_CYCLE_START_PIN
		if (CONTROL_CYCLE_START_PIN) {
			pin |= (1 << CONTROL_CYCLE_START_BIT);
		}
	#endif
	#ifdef CONTROL_SAFETY_DOOR_PIN
		if (CONTROL_SAFETY_DOOR_PIN) {
			pin |= (1 << CONTROL_SAFETY_DOOR_BIT);
		}
	#endif
	#ifdef INVERT_CONTROL_PIN_MASK
		pin ^= INVERT_CONTROL_PIN_MASK;
	#endif
	if (pin) {
		#ifdef CONTROL_ABORT_PIN
			if (bit_istrue(pin,(1 << CONTROL_ABORT_BIT))) {
				control_state |= CONTROL_PIN_INDEX_ABORT;
			}
		#endif
		#ifdef CONTROL_SAFETY_DOOR_PIN
			if (bit_istrue(pin,(1 << CONTROL_SAFETY_DOOR_BIT))) {
				control_state |= CONTROL_PIN_INDEX_SAFETY_DOOR; 
			}
		#endif
		#ifdef CONTROL_FEED_HOLD_PIN
			if (bit_istrue(pin,(1 << CONTROL_FEED_HOLD_BIT))) {
				control_state |= CONTROL_PIN_INDEX_FEED_HOLD;
			}
		#endif
		#ifdef CONTROL_CYCLE_START_PIN
			if (bit_istrue(pin,(1 << CONTROL_CYCLE_START_BIT))) {
				control_state |= CONTROL_PIN_INDEX_CYCLE_START;
			}
		#endif
	}
	return(control_state);
}

static void system_control_pins(void) {
	lock_isr();
	if (do_system_control_1ms) {
		do_system_control_1ms = 0;
		do_system_poll_control_pins = 1;
	}
	unlock_isr();
	if (do_system_poll_control_pins) {
		do_system_poll_control_pins = 0;
		
		system_control_get_state_current = system_control_get_state();
		system_control_get_state_debug = system_control_get_state_current;
		
		if (system_control_get_state_current != system_control_get_state_last) {
			if ( (bit_istrue(system_control_get_state_current, CONTROL_PIN_INDEX_CYCLE_START)) && (bit_isfalse(system_control_get_state_last, CONTROL_PIN_INDEX_CYCLE_START)) ) {
				system_set_exec_state_flag(EXEC_CYCLE_START);
			}
			if ( (bit_istrue(system_control_get_state_current, CONTROL_PIN_INDEX_FEED_HOLD)) && (bit_isfalse(system_control_get_state_last, CONTROL_PIN_INDEX_FEED_HOLD)) ) {
				system_set_exec_state_flag(EXEC_FEED_HOLD);
			} 
			if ( (bit_istrue(system_control_get_state_current, CONTROL_PIN_INDEX_ABORT)) && (bit_isfalse(system_control_get_state_last, CONTROL_PIN_INDEX_ABORT)) ) {
				system_set_exec_state_flag(EXEC_ABORT);
			} 
			if ( (bit_istrue(system_control_get_state_current, CONTROL_PIN_INDEX_SAFETY_DOOR)) && (bit_isfalse(system_control_get_state_last, CONTROL_PIN_INDEX_SAFETY_DOOR)) ) {
				system_set_exec_state_flag(EXEC_SAFETY_DOOR);
			}
		}
		system_control_get_state_last = system_control_get_state_current;
	}
}