#include "grbl.h"
#include "grbl_homing.h"

// Homing axis search distance multiplier. Computed by this value times the cycle travel.
#ifndef HOMING_AXIS_SEARCH_SCALAR
	#define HOMING_AXIS_SEARCH_SCALAR  1.5 // Must be > 1 to ensure limit switch will be engaged.
#endif
#ifndef HOMING_AXIS_LOCATE_SCALAR
	#define HOMING_AXIS_LOCATE_SCALAR  5.0 // Must be > 1 to ensure limit switch is cleared.
#endif

typedef enum _mc_homing_sm {
	MC_HOMING_IDLE = 0,
	MC_HOMING_TRIGGERED,
	MC_HOMING_Z_WAIT,
	MC_HOMING_Y_WAIT,
	MC_HOMING_X_WAIT,
} mc_homing_sm;

typedef enum _grbl_homing_go_home_states {
	grbl_homing_GO_HOME_INIT = 0,
	grbl_homing_GO_HOME_1,
	grbl_homing_GO_HOME_2,
	grbl_homing_GO_HOME_DELAY,
	grbl_homing_GO_HOME_FINISHED,
} grbl_homing_go_home_states;

uint8 do_mc_homing_cycle = 0;
mc_homing_sm mc_homing_state_machine = MC_HOMING_IDLE;
uint8 cycle_mask_buffer = 0;
volatile uint8 do_grbl_homing_go_home = 0;
grbl_homing_go_home_states grbl_homing_go_home_state = grbl_homing_GO_HOME_INIT;
plan_line_data_t plan_data;
plan_line_data_t *pl_data = &plan_data;
uint8 n_cycle = 0;
float max_travel = 0.0f;
uint8 step_pin[N_AXIS];
uint8 cycle_mask_asynch = 0;
bool approach = 0;
float homing_rate = 0.0f;
uint8 grbl_homing_state_global = 0;
uint8 axislock = 0;
uint8 n_active_axis = 0;
float target_global[N_AXIS];
uint8 grbl_homing_flag_for_homing_EXEC_CYCLE_STOP = 0;
uint16 grbl_homing_delay_home = 0;

void grbl_homing_go_home(uint8 cycle_mask);
uint8 grbl_homing_go_home_running(void);
int32_t grbl_homing_calculate_set_axis_position(uint8 idx);
void grbl_homing_clear_go_come_instant(void);
void do_homing_single_axis_sm(void);
	
void init_homing(void) {
	do_mc_homing_cycle = 0;
	mc_homing_state_machine = MC_HOMING_IDLE;
	cycle_mask_buffer = 0;
	do_grbl_homing_go_home = 0;
	grbl_homing_go_home_state = grbl_homing_GO_HOME_INIT;
	memset(&plan_data, 0, sizeof(plan_data));
	pl_data = &plan_data;
	n_cycle = 0;
	max_travel = 0.0f;
	memset(step_pin, 0, sizeof(step_pin));
	cycle_mask_asynch = 0;
	approach = 0;
	homing_rate = 0.0f;
	grbl_homing_state_global = 0;
	axislock = 0;
	n_active_axis = 0;
	memset(target_global, 0, sizeof(target_global));
	grbl_homing_flag_for_homing_EXEC_CYCLE_STOP = 0;
	grbl_homing_delay_home = 0;
}

void do_homing(void) {
	switch (mc_homing_state_machine) {
		case MC_HOMING_IDLE : {
			if (do_mc_homing_cycle) {
				mc_homing_state_machine = MC_HOMING_TRIGGERED;
			}
			break;
		}
		case MC_HOMING_TRIGGERED : {
			limits_disable(); // Disable hard limits pin change register for cycle duration

			// -------------------------------------------------------------------------------------
			// Perform homing routine. NOTE: Special motion case. Only system reset works.

			// Search to engage all axes limit switches at faster homing seek rate.
			grbl_homing_go_home(HOMING_CYCLE_0);  // Homing cycle 0
			mc_homing_state_machine = MC_HOMING_Z_WAIT;
			break;
		}
		case MC_HOMING_Z_WAIT : {
			if (grbl_homing_go_home_running() == 0) {
				#ifdef HOMING_CYCLE_1
					grbl_homing_go_home(HOMING_CYCLE_1);  // Homing cycle 1
				#endif
				mc_homing_state_machine = MC_HOMING_Y_WAIT;
			}
			break;
		}
		case MC_HOMING_Y_WAIT : {
			if (grbl_homing_go_home_running() == 0) {
				#ifdef HOMING_CYCLE_2
					grbl_homing_go_home(HOMING_CYCLE_2);  // Homing cycle 2
				#endif
				mc_homing_state_machine = MC_HOMING_X_WAIT;
			}
			break;
		}
		case MC_HOMING_X_WAIT : {
			if (grbl_homing_go_home_running() == 0) {
				// Homing cycle complete! Setup system for normal operation.
				// -------------------------------------------------------------------------------------
				//TODO Here rewrite all coorinates at one, so that no rounding errors can happen

				// Sync gcode parser and planner positions to homed position.
				gc_sync_position();
				plan_sync_position();

				// If hard limits feature enabled, re-enable hard limits pin change register after homing cycle.
				limits_enable();
				do_mc_homing_cycle = 0;
				mc_homing_state_machine = MC_HOMING_IDLE;
			}
			break;
		}
		default : {
			do_mc_homing_cycle = 0;
			mc_homing_state_machine = MC_HOMING_IDLE;
			break;
		}
	}
	
	do_homing_single_axis_sm();
	
}

uint8 grbl_homing_mc_homing_cycle_running(void) {
	uint8 result = 0;
	if (do_mc_homing_cycle != 0) {
		result = 1;
	}
	return result;
}

// Perform homing cycle to locate and set machine zero. Only '$H' executes this command.
// NOTE: There should be no motions in the buffer and Grbl must be in an idle state before
// executing the homing cycle. This prevents incorrect buffered plans after homing.
void mc_homing_cycle(uint8 cycle_mask) {
	cycle_mask_buffer = cycle_mask;
	do_mc_homing_cycle = 1;
}

void do_homing_single_axis_sm(void) {
	if (do_grbl_homing_go_home) {
		switch (grbl_homing_go_home_state) {
			case grbl_homing_GO_HOME_INIT : {
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
					if (bit_istrue(cycle_mask_asynch, bit(idx))) {
						// Set target based on max_travel setting. Ensure homing switches engaged with search scalar.
						// NOTE: settings.max_travel[] is stored as a negative value.
						max_travel = max(max_travel, (-HOMING_AXIS_SEARCH_SCALAR) * settings.max_travel[idx]);
					}
				}
				
				// Set search mode with approach at seek rate to quickly engage the specified cycle_mask_asynch limit switches.
				approach = true;
				homing_rate = settings.homing_seek_rate;
				grbl_homing_state_global = 0;
				axislock = 0;
				n_active_axis = 0;
				
				#ifdef HOMING_IS_JUST_ZEROING_MACHINE_POS
					grbl_homing_go_home_state = grbl_homing_GO_HOME_FINISHED;
				#else
					grbl_homing_go_home_state = grbl_homing_GO_HOME_1;
				#endif
				break;
			}
			case grbl_homing_GO_HOME_1 : {
				uint8 idx = 0;
				system_convert_array_steps_to_mpos(target_global, sys_position);
				
				// Initialize and declare variables needed for homing routine.
				axislock = 0;
				n_active_axis = 0;
				for (idx = 0; idx < N_AXIS; idx++) {
					// Set target location for active axes and setup computation for homing rate.
					if (bit_istrue(cycle_mask_asynch, bit(idx))) {
						n_active_axis++;
						sys_position[idx] = 0;
						grbl_eeprom_storePositionToNoInit();
						// Set target direction based on cycle mask and homing cycle approach state.
						// NOTE: This happens to compile smaller than any other implementation tried.
						if (bit_istrue(settings.homing_dir_mask, bit(idx))) {
							if (approach) { 
								target_global[idx] = -max_travel; 
							} else {
								target_global[idx] = max_travel;
							}
						} else {
							if (approach) {
								target_global[idx] = max_travel; 
							} else { 
								target_global[idx] = -max_travel;
							}
						}
						// Apply axislock to the step port pins active in this cycle.
						axislock |= step_pin[idx];
						#ifdef GRBL_SIMULATE_ENDSWITCH
							if (approach) {
								limits_delay_activate[idx] = 1500;
							} else {
								limits_delay_deactivate[idx] = 100;
							}
						#endif
					}
				}
				homing_rate *= sqrt(n_active_axis); // [sqrt(N_AXIS)] Adjust so individual axes all move at homing rate.
				sys.homing_axis_lock = axislock;
				
				// Perform homing cycle. Planner buffer should be empty, as required to initiate the homing cycle.
				pl_data->feed_rate = homing_rate; // Set current homing rate.
				grbl_homing_flag_for_homing_EXEC_CYCLE_STOP = 0;
				plan_buffer_line(target_global, pl_data); // Bypass mc_line(). Directly plan homing motion.
				
				sys.step_control = STEP_CONTROL_EXECUTE_SYS_MOTION; // Set to execute homing motion and clear existing flags.
				st_prep_buffer(); // Prep and fill segment buffer from newly planned block.
				system_log_st_wake_up(4);
				st_wake_up(); // Initiate motion
				grbl_homing_go_home_state = grbl_homing_GO_HOME_2;
				break;
			}
			case grbl_homing_GO_HOME_2 : {
				if (approach) {
					uint8 idx;
					// Check limit state. Lock out cycle axes when they change.
					grbl_homing_state_global = limits_get_state_filtered();
					for (idx = 0; idx < N_AXIS; idx++) {
						if (axislock & step_pin[idx]) {
							if (grbl_homing_state_global & (1 << idx)) {
								axislock &= ~(step_pin[idx]);
							}
						}
					}
					sys.homing_axis_lock = axislock;
				}
				
				st_prep_buffer(); // Check and prep segment buffer. NOTE: Should take no longer than 200us.


				// Homing failure condition: Limit switch not found during approach.
				if ( (approach == 1) && (grbl_homing_flag_for_homing_EXEC_CYCLE_STOP) && ((STEP_MASK & axislock) != 0)) {
					grbl_homing_flag_for_homing_EXEC_CYCLE_STOP = 0;
					system_set_exec_alarm(EXEC_ALARM_HOMING_FAIL_APPROACH);
					grbl_homing_go_home_state = grbl_homing_GO_HOME_INIT;
					do_grbl_homing_go_home = 0;
				}

				// Pull-off motion complete. Disable CYCLE_STOP from executing.
				if ( (approach == 0) && (grbl_homing_flag_for_homing_EXEC_CYCLE_STOP)) {
					grbl_homing_flag_for_homing_EXEC_CYCLE_STOP = 0;

					// Homing failure condition: Limit switch still engaged after pull-off motion
					if (!approach && (limits_get_state_filtered() & cycle_mask_asynch)) {
						system_set_exec_alarm(EXEC_ALARM_HOMING_FAIL_PULLOFF);
						grbl_homing_go_home_state = grbl_homing_GO_HOME_INIT;
						do_grbl_homing_go_home = 0;
					} else {
						mc_stop(); // Initiate system kill.

						grbl_homing_delay_home = settings.homing_debounce_delay; // Delay to allow transient dynamics to dissipate.
						grbl_homing_go_home_state = grbl_homing_GO_HOME_DELAY;
					}
				}
				
				if ((STEP_MASK & axislock) == 0) {
					mc_stop(); // Initiate system kill.

					grbl_homing_delay_home = settings.homing_debounce_delay; // Delay to allow transient dynamics to dissipate.
					grbl_homing_go_home_state = grbl_homing_GO_HOME_DELAY;
				}
				break;
			}
			case grbl_homing_GO_HOME_DELAY : {
				if (grbl_homing_delay_home != 0) {
					grbl_homing_delay_home--;
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
						grbl_homing_go_home_state = grbl_homing_GO_HOME_FINISHED;
					} else {
						grbl_homing_go_home_state = grbl_homing_GO_HOME_1;
					}
					n_cycle--;
				}
				break;
			}
			case grbl_homing_GO_HOME_FINISHED : {
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
							set_axis_position = grbl_homing_calculate_set_axis_position(idx);
						}
						sys_position[idx] = set_axis_position;
						grbl_eeprom_storePositionToNoInit();
					}
				}
				sys.step_control = STEP_CONTROL_NORMAL_OP; // Return step control to normal operation.
				grbl_homing_go_home_state = grbl_homing_GO_HOME_INIT;
				system_log_home_finished();
				do_grbl_homing_go_home = 0;
				break;
			}
			default : {
				grbl_homing_go_home_state = grbl_homing_GO_HOME_INIT;
				do_grbl_homing_go_home = 0;
				break;
			}
		}
	}
}

uint8 grbl_homing_go_home_running(void) {
	uint8 result = 0;
	if (do_grbl_homing_go_home != 0) {
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
void grbl_homing_go_home(uint8 cycle_mask) {
	do_grbl_homing_go_home = 1;
	cycle_mask_asynch = cycle_mask;
}

int32_t grbl_homing_calculate_set_axis_position(uint8 idx) {
	int32_t result = 0;
	if (idx < N_AXIS) {
		if ( bit_istrue(settings.homing_dir_mask, bit(idx)) ) {
			result = lround_my((settings.max_travel[idx] + settings.homing_pulloff) * settings.steps_per_mm[idx]);
		} else {
			//result = lround_my((- settings.homing_pulloff) * settings.steps_per_mm[idx]);
			//In this case maybe it is better to let the zero position be the real zero
			result = lround_my(0);
		}
	}
	return result;
}

void grbl_homing_clear_go_come_instant(void) {
	grbl_homing_go_home_state = grbl_homing_GO_HOME_INIT;
	do_grbl_homing_go_home = 0;
}

void grbl_homing_EXEC_SAFETY_DOOR_event(void) {
	if (grbl_homing_go_home_running()) {
		// Homing failure condition: Safety door was opened.
		system_set_exec_alarm(EXEC_ALARM_HOMING_FAIL_DOOR);
		grbl_homing_clear_go_come_instant();
	}
}

void grbl_homing_EXEC_CYCLE_STOP_event(void) {
	if (grbl_homing_go_home_running()) {
		grbl_homing_flag_for_homing_EXEC_CYCLE_STOP = 1;
	}
}
