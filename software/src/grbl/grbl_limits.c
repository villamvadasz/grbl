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

#include <string.h>
#include <stdio.h>
#include <math.h>
#include "grbl.h"

#include "k_stdtype.h"

#define GRBL_LIMITS_FILTER_ELEMENTS 4

typedef enum _limit_go_home_states {
	LIMIT_GO_HOME_INIT = 0,
	LIMIT_GO_HOME_1,
	LIMIT_GO_HOME_2,
	LIMIT_GO_HOME_DELAY,
	LIMIT_GO_HOME_FINISHED,
} limit_go_home_states;

volatile uint8 do_limits_go_home = 0;
uint8 cycle_mask_asynch = 0;
limit_go_home_states limit_go_home_state = LIMIT_GO_HOME_INIT;
uint8 n_cycle = 0;
uint8 step_pin[N_AXIS];
float target[N_AXIS];
float max_travel = 0.0f;
uint8 limit_state = 0;
uint8 axislock = 0;
uint8 n_active_axis = 0;
bool approach = 0;
float homing_rate = 0.0f;
plan_line_data_t plan_data;
plan_line_data_t *pl_data = &plan_data;
volatile uint32 do_limits_1ms = 0;
volatile uint32 do_limits_polling = 0;
uint8 limits_disabled = 1;
uint8 limits_get_state_last = 0;
uint8 limits_get_state_current = 0;
uint8 limits_get_state_debug = 0;
uint16 limit_delay_home = 0;
uint8 limit_flag_for_homeing_EXEC_CYCLE_STOP = 0;
unsigned char logger_limits_get_state_prev = 0;
uint8 limit_filter_state[GRBL_LIMITS_FILTER_ELEMENTS];
uint8 limits_set_state_filtered_currentState;

uint8 limits_get_state_non_filtered(void);
void limits_set_state_filtered(uint8 currentState);

// Homing axis search distance multiplier. Computed by this value times the cycle travel.
#ifndef HOMING_AXIS_SEARCH_SCALAR
	#define HOMING_AXIS_SEARCH_SCALAR  1.5 // Must be > 1 to ensure limit switch will be engaged.
#endif
#ifndef HOMING_AXIS_LOCATE_SCALAR
	#define HOMING_AXIS_LOCATE_SCALAR  5.0 // Must be > 1 to ensure limit switch is cleared.
#endif

void limits_init(void) {
	do_limits_go_home = 0;
	cycle_mask_asynch = 0;
	limit_go_home_state = LIMIT_GO_HOME_INIT;
	n_cycle = 0;
	memset(step_pin, 0, sizeof(step_pin));
	memset(target, 0, sizeof(target));
	max_travel = 0.0f;
	limit_state = 0;
	axislock = 0;
	n_active_axis = 0;
	approach = 0;
	homing_rate = 0.0f;
	memset(&plan_data, 0, sizeof(plan_data));
	pl_data = &plan_data;
	do_limits_1ms = 0;
	do_limits_polling = 0;
	limits_disabled = 1;
	limits_get_state_debug = 0;
	limit_delay_home = 0;
	limit_flag_for_homeing_EXEC_CYCLE_STOP = 0;

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
					if (!isLimits_go_homeRunning()) {
						system_set_exec_alarm(EXEC_ALARM_HARD_LIMIT); // Indicate hard limit critical event
					}
				}
			}
		}
		
	}
	
	if (do_limits_go_home) {
		switch (limit_go_home_state) {
			case LIMIT_GO_HOME_INIT : {
				uint8 idx = 0;
				// Initialize plan data struct for homing motion. Spindle and coolant are disabled.
				//plan_data;
				system_log_home(); 
		
				pl_data = &plan_data;
				memset(pl_data, 0, sizeof(plan_line_data_t));
				pl_data->condition = (PL_COND_FLAG_SYSTEM_MOTION | PL_COND_FLAG_NO_FEED_OVERRIDE);
				#ifdef USE_LINE_NUMBERS
					pl_data->line_number = HOMING_CYCLE_LINE_NUMBER;
				#endif
				
				// Initialize variables used for homing computations.
				n_cycle = (2 * N_HOMING_LOCATE_CYCLE + 1);
				max_travel = 0.0;
				for (idx = 0; idx < N_AXIS; idx++) {
					// Initialize step pin masks
					step_pin[idx] = get_step_pin_mask(idx);
					if (bit_istrue(cycle_mask_asynch,bit(idx))) {
						// Set target based on max_travel setting. Ensure homing switches engaged with search scalar.
						// NOTE: settings.max_travel[] is stored as a negative value.
						max_travel = max(max_travel, (-HOMING_AXIS_SEARCH_SCALAR) * settings.max_travel[idx]);
					}
				}
				
				// Set search mode with approach at seek rate to quickly engage the specified cycle_mask_asynch limit switches.
				approach = true;
				homing_rate = settings.homing_seek_rate;
				limit_state = 0;
				axislock = 0;
				n_active_axis = 0;
				
				#ifdef HOMING_IS_JUST_ZEROING_MACHINE_POS
					limit_go_home_state = LIMIT_GO_HOME_FINISHED;
				#else
					limit_go_home_state = LIMIT_GO_HOME_1;
				#endif
				break;
			}
			case LIMIT_GO_HOME_1 : {
				uint8 idx = 0;
				system_convert_array_steps_to_mpos(target, sys_position);
				
				// Initialize and declare variables needed for homing routine.
				axislock = 0;
				n_active_axis = 0;
				for (idx = 0; idx < N_AXIS; idx++) {
					// Set target location for active axes and setup computation for homing rate.
					if (bit_istrue(cycle_mask_asynch,bit(idx))) {
						n_active_axis++;
						sys_position[idx] = 0;
						grbl_eeprom_storePositionToNoInit();
						// Set target direction based on cycle mask and homing cycle approach state.
						// NOTE: This happens to compile smaller than any other implementation tried.
						if (bit_istrue(settings.homing_dir_mask, bit(idx))) {
							if (approach) { 
								target[idx] = -max_travel; 
							} else {
								target[idx] = max_travel;
							}
						} else {
							if (approach) {
								target[idx] = max_travel; 
							} else { 
								target[idx] = -max_travel;
							}
						}
						// Apply axislock to the step port pins active in this cycle.
						axislock |= step_pin[idx];
					}
				}
				homing_rate *= sqrt(n_active_axis); // [sqrt(N_AXIS)] Adjust so individual axes all move at homing rate.
				sys.homing_axis_lock = axislock;
				
				// Perform homing cycle. Planner buffer should be empty, as required to initiate the homing cycle.
				pl_data->feed_rate = homing_rate; // Set current homing rate.
				limit_flag_for_homeing_EXEC_CYCLE_STOP = 0;
				plan_buffer_line(target, pl_data); // Bypass mc_line(). Directly plan homing motion.
				
				sys.step_control = STEP_CONTROL_EXECUTE_SYS_MOTION; // Set to execute homing motion and clear existing flags.
				st_prep_buffer(); // Prep and fill segment buffer from newly planned block.
				system_log_st_wake_up(4);
				st_wake_up(); // Initiate motion
				limit_go_home_state = LIMIT_GO_HOME_2;
				break;
			}
			case LIMIT_GO_HOME_2 : {
				if (approach) {
					uint8 idx;
					// Check limit state. Lock out cycle axes when they change.
					limit_state = limits_get_state_filtered();
					for (idx = 0; idx < N_AXIS; idx++) {
						if (axislock & step_pin[idx]) {
							if (limit_state & (1 << idx)) {
								axislock &= ~(step_pin[idx]);
							}
						}
					}
					sys.homing_axis_lock = axislock;
				}
				
				st_prep_buffer(); // Check and prep segment buffer. NOTE: Should take no longer than 200us.


				// Homing failure condition: Limit switch not found during approach.
				if ( (approach == 1) && (limit_flag_for_homeing_EXEC_CYCLE_STOP) && ((STEP_MASK & axislock) != 0)) {
					limit_flag_for_homeing_EXEC_CYCLE_STOP = 0;
					system_set_exec_alarm(EXEC_ALARM_HOMING_FAIL_APPROACH);
					limit_go_home_state = LIMIT_GO_HOME_INIT;
					do_limits_go_home = 0;
				}

				// Pull-off motion complete. Disable CYCLE_STOP from executing.
				if ( (approach == 0) && (limit_flag_for_homeing_EXEC_CYCLE_STOP)) {
					limit_flag_for_homeing_EXEC_CYCLE_STOP = 0;

					// Homing failure condition: Limit switch still engaged after pull-off motion
					if (!approach && (limits_get_state_filtered() & cycle_mask_asynch)) {
						system_set_exec_alarm(EXEC_ALARM_HOMING_FAIL_PULLOFF);
						limit_go_home_state = LIMIT_GO_HOME_INIT;
						do_limits_go_home = 0;
					} else {
						mc_stop(); // Initiate system kill.

						limit_delay_home = settings.homing_debounce_delay; // Delay to allow transient dynamics to dissipate.
						limit_go_home_state = LIMIT_GO_HOME_DELAY;
					}
				}
				
				if ((STEP_MASK & axislock) == 0) {
					mc_stop(); // Initiate system kill.

					limit_delay_home = settings.homing_debounce_delay; // Delay to allow transient dynamics to dissipate.
					limit_go_home_state = LIMIT_GO_HOME_DELAY;
				}
				break;
			}
			case LIMIT_GO_HOME_DELAY : {
				if (limit_delay_home != 0) {
					limit_delay_home--;
				} else {
					// Reverse direction and reset homing rate for locate cycle(s).
					approach = !approach;
				
					// After first cycle, homing enters locating phase. Shorten search to pull-off distance.
					if (approach) {
						max_travel = settings.homing_pulloff * HOMING_AXIS_LOCATE_SCALAR;
						homing_rate = settings.homing_feed_rate;
					} else {
						max_travel = settings.homing_pulloff;
						homing_rate = settings.homing_seek_rate;
					}
					if (n_cycle == 0) {
						limit_go_home_state = LIMIT_GO_HOME_FINISHED;
					} else {
						limit_go_home_state = LIMIT_GO_HOME_1;
					}
					n_cycle--;
				}
				break;
			}
			case LIMIT_GO_HOME_FINISHED : {
				uint8 idx = 0;
				// The active cycle axes should now be homed and machine limits have been located. By
				// default, Grbl defines machine space as all negative, as do most CNCs. Since limit switches
				// can be on either side of an axes, check and set axes machine zero appropriately. Also,
				// set up pull-off maneuver from axes limit switches that have been homed. This provides
				// some initial clearance off the switches and should also help prevent them from falsely
				// triggering when hard limits are enabled or when more than one axes shares a limit pin.
				int32_t set_axis_position = 0;
				// Set machine positions for homed limit switches. Don't update non-homed axes.
				for (idx = 0; idx < N_AXIS; idx++) {
					// NOTE: settings.max_travel[] is stored as a negative value.
					if (cycle_mask_asynch & bit(idx)) {
						uint8 skipCalculation = 0;
						#ifdef HOMING_FORCE_SET_ORIGIN_X
							if (cycle_mask_asynch & (1 << X_AXIS)) {
								set_axis_position = 0;
								skipCalculation = 1;
							}
						#endif
						#ifdef HOMING_FORCE_SET_ORIGIN_Y
							if (cycle_mask_asynch & (1 << Y_AXIS)) {
								set_axis_position = 0;
								skipCalculation = 1;
							}
						#endif
						#ifdef HOMING_FORCE_SET_ORIGIN_Z
							if (cycle_mask_asynch & (1 << Z_AXIS)) {
								set_axis_position = 0;
								skipCalculation = 1;
							}
						#endif
						if (skipCalculation == 0) {
							set_axis_position = grbl_limit_calculate_set_axis_position(idx);
						}
						sys_position[idx] = set_axis_position;
						grbl_eeprom_storePositionToNoInit();
					}
				}
				sys.step_control = STEP_CONTROL_NORMAL_OP; // Return step control to normal operation.
				limit_go_home_state = LIMIT_GO_HOME_INIT;
				system_log_home_finished();
				do_limits_go_home = 0;
				break;
			}
			default : {
				limit_go_home_state = LIMIT_GO_HOME_INIT;
				do_limits_go_home = 0;
				break;
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

uint8 isLimits_go_homeRunning(void) {
	uint8 result = 0;
	if (do_limits_go_home != 0) {
		result = 1;
	}
	return result;
}



// Homes the specified cycle axes, sets the machine position, and performs a pull-off motion after
// completing. Homing is a special motion case, which involves rapid uncontrolled stops to locate
// the trigger point of the limit switches. The rapid stops are handled by a system level axis lock
// mask, which prevents the stepper algorithm from executing step pulses. Homing motions typically
// circumvent the processes for executing motions in normal operation.
// NOTE: Only the abort realtime command can interrupt this process.
// TODO: Move limit pin-specific calls to a general function for portability.
void limits_go_home(uint8 cycle_mask) {
	do_limits_go_home = 1;
	cycle_mask_asynch = cycle_mask;
}

void limits_EXEC_CYCLE_STOP_event(void) {
	if (isLimits_go_homeRunning()) {
		limit_flag_for_homeing_EXEC_CYCLE_STOP = 1;
	}
}

void limits_EXEC_SAFETY_DOOR_event(void) {
	if (isLimits_go_homeRunning()) {
		// Homing failure condition: Safety door was opened.
		system_set_exec_alarm(EXEC_ALARM_HOMING_FAIL_DOOR);
		limit_go_home_state = LIMIT_GO_HOME_INIT;
		do_limits_go_home = 0;
	}
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

int32_t grbl_limit_calculate_set_axis_position(uint8 idx) {
	int32_t result = 0;
	if (idx < N_AXIS) {
		if ( bit_istrue(settings.homing_dir_mask, bit(idx)) ) {
			result = lround_my((settings.max_travel[idx] + settings.homing_pulloff) * settings.steps_per_mm[idx]);
		} else {
			result = lround_my((-settings.max_travel[idx] - settings.homing_pulloff) * settings.steps_per_mm[idx]);
		}
	}
	return result;
}
