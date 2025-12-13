/*
  limits.c - code pertaining to limit-switches and performing the homing cycle
  Part of Grbl

  Copyright (c) 2012-2016 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "grbl.h"

#include "k_stdtype.h"

#define GRBL_LIMITS_FILTER_ELEMENTS 4

#define LIMIT_SIMULATION_ENGAGED	1
#define LIMIT_SIMULATION_DISENGAGED 0

volatile uint32 do_limits_1ms = 0;
volatile uint32 do_limits_polling = 0;
uint8 limits_disabled = 1;
uint8 limits_get_state_last = 0;
uint8 limits_get_state_current = 0;
uint8 limits_get_state_debug = 0;
unsigned char logger_limits_get_state_prev = 0;
uint8 limit_filter_state[GRBL_LIMITS_FILTER_ELEMENTS];
uint8 limits_set_state_filtered_currentState;

#ifdef GRBL_SIMULATE_ENDSWITCH
	uint32 limits_delay_activate[N_AXIS] = {0};
	uint32 limits_delay_deactivate[N_AXIS] = {0};
#endif

uint8 limits_get_state_non_filtered(void);
void limits_set_state_filtered(uint8 currentState);

void limits_init(void) {
	do_limits_1ms = 0;
	do_limits_polling = 0;
	limits_disabled = 1;
	limits_get_state_debug = 0;

	#ifdef LIMIT_X_DDR
		LIMIT_X_DDR = 1;
		MAL_SYNC();
	#endif
	#ifdef LIMIT_Y_DDR
		LIMIT_Y_DDR = 1;
		MAL_SYNC();
	#endif
	#ifdef LIMIT_Z_DDR
		LIMIT_Z_DDR = 1;
		MAL_SYNC();
	#endif

	#ifdef LIMIT_X_PIN_PUE
		LIMIT_X_PIN_PUE = 1;
		MAL_SYNC();
	#endif
	#ifdef LIMIT_Y_PIN_PUE
		LIMIT_Y_PIN_PUE = 1;
		MAL_SYNC();
	#endif
	#ifdef LIMIT_Z_PIN_PUE
		LIMIT_Z_PIN_PUE = 1;
		MAL_SYNC();
	#endif

	limits_enable();

	limits_get_state_current = limits_get_state_non_filtered();
	limits_set_state_filtered_currentState = limits_get_state_current;
	limits_get_state_last = limits_get_state_current;
	logger_limits_get_state_prev = limits_get_state_non_filtered();
	
	memset(limit_filter_state, 0x00, sizeof(limit_filter_state));
	#ifdef GRBL_SIMULATE_ENDSWITCH
	{
		LIMIT_X_PIN = LIMIT_SIMULATION_DISENGAGED;
		LIMIT_Y_PIN = LIMIT_SIMULATION_DISENGAGED;
		LIMIT_Z_PIN = LIMIT_SIMULATION_DISENGAGED;
	}
	#endif

}

void do_limits(void) {
	{
		volatile uint8 isr_tempVal = 0;
		lock_isr();
		if (do_limits_1ms) {
			do_limits_1ms = 0;
			isr_tempVal = 1;
		}
		unlock_isr();
		if (isr_tempVal) {
			isr_tempVal = 0;
			{// Filter inputs every 1ms
				uint8 currentState = limits_get_state_non_filtered();
				
				uint32 x = 0;
				for (x = 1; x < GRBL_LIMITS_FILTER_ELEMENTS; x++) {
					limit_filter_state[x - 1] = limit_filter_state[x];
				}
				limit_filter_state[GRBL_LIMITS_FILTER_ELEMENTS - 1] = currentState;
				
				uint8 newState = 0xFF;
				for (x = 0; x < GRBL_LIMITS_FILTER_ELEMENTS; x++) {
					newState &= limit_filter_state[x];
				}
				
				limits_set_state_filtered(newState);
			}
			limits_get_state_debug = limits_get_state_filtered();
			if (limits_disabled == 0) {
				do_limits_polling = 1;
			}
			
			#ifdef GRBL_SIMULATE_ENDSWITCH
			{
				uint8 idx = 0;
				for (idx = 0; idx < N_AXIS; idx++) {
					if (limits_delay_deactivate[idx] != 0) {
						limits_delay_deactivate[idx]--;
						if (limits_delay_deactivate[idx] == 1) {
							if (idx == X_AXIS) {
								LIMIT_X_PIN = LIMIT_SIMULATION_DISENGAGED;
							}
							if (idx == Y_AXIS) {
								LIMIT_Y_PIN = LIMIT_SIMULATION_DISENGAGED;
							}
							if (idx == Z_AXIS) {
								LIMIT_Z_PIN = LIMIT_SIMULATION_DISENGAGED;
							}
						}
					}
					if (limits_delay_activate[idx] != 0) {
						limits_delay_activate[idx]--;
						if (limits_delay_activate[idx] == 1) {
							if (idx == X_AXIS) {
								LIMIT_X_PIN = LIMIT_SIMULATION_ENGAGED;
							}
							if (idx == Y_AXIS) {
								LIMIT_Y_PIN = LIMIT_SIMULATION_ENGAGED;
							}
							if (idx == Z_AXIS) {
								LIMIT_Z_PIN = LIMIT_SIMULATION_ENGAGED;
							}
						}
					}
				}
			}
			#endif
			
		}
	}
	
	if (do_limits_polling) {
		do_limits_polling = 0;
		
		// Ignore limit switches if already in an alarm state or in-process of executing an alarm.
		// When in the alarm state, Grbl should have been reset or will force a reset, so any pending
		// moves in the planner and serial buffers are all cleared and newly sent blocks will be
		// locked out until a homing cycle or a kill lock command. Allows the user to disable the hard
		// limit setting if their limits are constantly triggering after a reset and move their axes.
		if (sys.state != STATE_ALARM) {
			if (!(system_get_exec_alarm())) {
				uint8 isLimitActivated = 0;
				limits_get_state_current = limits_get_state_filtered();
				if (limits_get_state_current != limits_get_state_last) {
					if (limits_get_state_current > limits_get_state_last) {
						isLimitActivated = 1;
					}
				}
				limits_get_state_last = limits_get_state_current;
				if (isLimitActivated) {
					mc_stop();
					if (!grbl_homing_mc_homing_cycle_running()) {
						system_set_exec_alarm(EXEC_ALARM_HARD_LIMIT); // Indicate hard limit critical event
					}
				}
			}
		}
		
	}
}

void isr_limits_1ms(void) {
	do_limits_1ms = 1;
}

// This is the Limit Pin Change Interrupt, which handles the hard limit feature. A bouncing
// limit switch can cause a lot of problems, like false readings and multiple interrupt calls.
// If a switch is triggered at all, something bad has happened and treat it as such, regardless
// if a limit switch is being disengaged. It's impossible to reliably tell the state of a
// bouncing pin because the Arduino microcontroller does not retain any state information when
// detecting a pin change. If we poll the pins in the ISR, you can miss the correct reading if the 
// switch is bouncing.
// NOTE: Do not attach an e-stop to the limit pins, because this interrupt is disabled during
// homing cycles and will not respond correctly. Upon user request or need, there may be a
// special pinout for an e-stop, but it is generally recommended to just directly connect
// your e-stop switch to the Arduino reset pin, since it is the most correct way to do this.
void LIMIT_INT_vect(void) {
	if (limits_disabled == 0) {
		do_limits_polling = 1;
	}
}

// Disables hard limits.
void limits_disable(void) {
	limits_disabled = 1;
	#ifdef LIMIT_X_PIN_CN
		LIMIT_X_PIN_CN = 0;
		MAL_SYNC();
	#endif
	#ifdef LIMIT_Y_PIN_CN
		LIMIT_Y_PIN_CN = 0;
		MAL_SYNC();
	#endif
	#ifdef LIMIT_Z_PIN_CN
		LIMIT_Z_PIN_CN = 0;
		MAL_SYNC();
	#endif
}

void limits_enable(void) {
	if (bit_istrue(settings.flags, BITFLAG_HARD_LIMIT_ENABLE)) {
		#ifdef LIMIT_X_PIN_CN
			LIMIT_X_PIN_CN = 1;
			MAL_SYNC();
		#endif
		#ifdef LIMIT_Y_PIN_CN
			LIMIT_Y_PIN_CN = 1;
			MAL_SYNC();
		#endif
		#ifdef LIMIT_Z_PIN_CN
			LIMIT_Z_PIN_CN = 1;
			MAL_SYNC();
		#endif
		limits_disabled = 0;
	} else {
		limits_disable();
	}
}

// Returns limit state as a bit-wise uint8 variable. Each bit indicates an axis limit, where
// triggered is 1 and not triggered is 0. Invert mask is applied. Axes are defined by their
// number in bit position, i.e. Z_AXIS is (1<<2) or bit 2, and Y_AXIS is (1<<1) or bit 1.
uint8 limits_get_state_non_filtered(void) {
	uint8 limit_state = 0;
	uint8 pin = 0;
	
	#ifdef LIMITS_X_IS_ON_Y
		#ifdef LIMIT_Y_PIN
			if (LIMIT_Y_PIN) {
				pin |= bit(X_AXIS);
				pin |= bit(Y_AXIS);
			}
		#endif
		#ifdef LIMIT_Z_PIN
			if (LIMIT_Z_PIN) {
				pin |= bit(Z_AXIS);
			}
		#endif
	#else
		#ifdef LIMIT_X_PIN
			if (LIMIT_X_PIN) {
				pin |= bit(X_AXIS);
			}
		#endif
		#ifdef LIMIT_Y_PIN
			if (LIMIT_Y_PIN) {
				pin |= bit(Y_AXIS);
			}
		#endif
		#ifdef LIMIT_Z_PIN
			if (LIMIT_Z_PIN) {
				pin |= bit(Z_AXIS);
			}
		#endif
	#endif

	#ifdef INVERT_LIMIT_PIN_MASK
		pin ^= INVERT_LIMIT_PIN_MASK;
	#endif
	if (bit_isfalse(settings.flags, BITFLAG_INVERT_LIMIT_PINS)) {
		pin ^= LIMIT_MASK;
	}

	limit_state = pin;
	
	{
		if ((logger_limits_get_state_prev & bit(X_AXIS)) != (limit_state & bit(X_AXIS))) {
			system_log_limit_x();
		}
		if ((logger_limits_get_state_prev & bit(Y_AXIS)) != (limit_state & bit(Y_AXIS))) {
			system_log_limit_y();
		}
		if ((logger_limits_get_state_prev & bit(Z_AXIS)) != (limit_state & bit(Z_AXIS))) {
			system_log_limit_z();
		}
		logger_limits_get_state_prev = limit_state;
	}

	return(limit_state);
}

uint8 limits_get_state_filtered(void) {
	uint8 result = 0;
	result = limits_set_state_filtered_currentState;
	return result;	
}

void limits_set_state_filtered(uint8 currentState) {
	limits_set_state_filtered_currentState = currentState;
}

// Performs a soft limit check. Called from mc_line() only. Assumes the machine has been homed,
// the workspace volume is in all negative space, and the system is in normal operation.
// NOTE: Used by jogging to limit travel within soft-limit volume.
void limits_soft_check(float *target) {
	if (system_check_travel_limits(target)) {
		sys.soft_limit = true;
		// Force feed hold if cycle is active. All buffered blocks are guaranteed to be within
		// workspace volume so just come to a controlled stop so position is not lost. When complete
		// enter alarm mode.
		if (sys.state == STATE_CYCLE) {
			system_set_exec_state_flag(EXEC_FEED_HOLD);
		}
		mc_stop(); // Issue system reset and ensure spindle and coolant are shutdown.
		system_set_exec_alarm(EXEC_ALARM_SOFT_LIMIT); // Indicate soft limit critical event
	}
}
