/*
	system.c - Handles system level commands and real-time processes
	Part of Grbl

	Copyright (c) 2014-2016 Sungeun K. Jeon for Gnea Research LLC

	Grbl is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Grbl is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Grbl.	If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "grbl.h"

#include "k_stdtype.h"
#include "tmr.h"
#include "grbl_gcode_external.h"

typedef enum _grbl_system_sm_states {
	sm_init = 0,
	sm_idle,
	sm_suspend,
	sm_suspend_delay
} grbl_system_sm_states;

grbl_system_sm_states grbl_system_sm = sm_init;

grbl_system_logger_st grbl_system_logger[SYSTEM_LOG_ITEMS];

system_t sys;
int32_t sys_position[N_AXIS];						// Real-time machine (aka home) position vector in steps.
int32_t sys_probe_position[N_AXIS];					// Last probe position in machine coordinates and steps.
volatile uint8 sys_rt_exec_motion_override = 0; 		// Global realtime executor bitflag variable for motion-based overrides.
volatile uint8 sys_rt_exec_accessory_override = 0;	// Global realtime executor bitflag variable for spindle/coolant overrides.
Timer system_suspendTime;

float restore_spindle_speed = 0.0f;
uint8 restore_condition = 0;
uint8 system_lastState = 0;

uint32 grbl_serial_number = 0xFFFFFFF0;

void system_exec_rt_system_alarm(void);
void system_exec_rt_system_state(void);
void system_exec_rt_system_override(void);
void protocol_exec_rt_suspend_preapare(void);
void protocol_exec_rt_suspend_async(void);

__attribute__(( weak )) void system_init_exteral_power(void) {}
__attribute__(( weak )) void do_system_exteral_power(void) {}
__attribute__(( weak )) void isr_system_exteral_power_1ms(void) {}
__attribute__(( weak )) unsigned int system_get_external_power_disengage_alarm(void) {return 1;}


void system_init(unsigned int alarm_supress) {
	
	system_init_exteral_power();
	//memset(sys_position,0,sizeof(sys_position)); // Clear probe position. read from NVM
	restore_spindle_speed = 0.0f;
	restore_condition = 0;
	system_lastState = 0;

	memset(grbl_system_logger, 0x00, sizeof(grbl_system_logger));
	
	init_timer(&system_suspendTime);
	system_log_sm_init_log(0);
	grbl_system_sm = sm_init;

	system_clear_exec_state_flag(EXEC_ALL);
	system_clear_exec_alarm();
	sys_rt_exec_motion_override = 0;
	sys_rt_exec_accessory_override = 0;

	// Initialize system state.
	// Force Grbl into an ALARM state upon a power-cycle or hard reset.
	if (alarm_supress == 0) {
		#ifndef _ISSUE_TESTING_
			system_set_sys_state(STATE_ALARM);
		#endif
	}

	// Check for power-up and set system alarm if homing is enabled to force homing cycle
	// by setting Grbl's alarm state. Alarm locks out all g-code commands, including the
	// startup scripts, but allows access to settings and internal commands. Only a homing
	// cycle '$H' or kill alarm locks '$X' will disable the alarm.
	// NOTE: The startup script will run after successful completion of the homing cycle, but
	// not after disabling the alarm locks. Prevents motion startup blocks from crashing into
	// things uncontrollably. Very bad.
	if (bit_istrue(settings.flags, BITFLAG_HOMING_ENABLE)) {
		if (alarm_supress == 0) {
			#ifndef _ISSUE_TESTING_
				system_set_sys_state(STATE_ALARM);
			#endif
		}
	}

	// Grbl initialization loop upon power-up or a system abort. For the latter, all processes
	// will return to this loop to be cleanly re-initialized.

	// Reset system variables.
	uint8 prior_state = sys.state;
	memset(&sys, 0, sizeof(system_t)); // Clear system struct variable.
	system_set_sys_state(prior_state);
	sys.f_override = DEFAULT_FEED_OVERRIDE;  // Set to 100%
	sys.r_override = DEFAULT_RAPID_OVERRIDE; // Set to 100%
	sys.spindle_speed_ovr = DEFAULT_SPINDLE_SPEED_OVERRIDE; // Set to 100%
	memset(sys_probe_position,0,sizeof(sys_probe_position)); // Clear probe position.
}

void do_system(void) {
	do_system_exteral_power();
	system_exec_rt_system_alarm();
	system_exec_rt_system_state();
	system_exec_rt_system_override();
	
	// Reload step segment buffer
	if (sys.state & (STATE_CYCLE | STATE_HOLD | STATE_SAFETY_DOOR | STATE_HOMING | STATE_JOG)) {
		st_prep_buffer();
	}
	
	switch (grbl_system_sm) {
		case sm_init : {
			// Perform some machine checks to make sure everything is good to go.
			if (bit_istrue(settings.flags, BITFLAG_HARD_LIMIT_ENABLE)) {
				if (limits_get_state_filtered()) {
					system_set_sys_state(STATE_ALARM); // Ensure alarm state is active.
					report_feedback_message(MESSAGE_CHECK_LIMITS);
				}
			}
			// Check for and report alarm state after a reset, error, or an initial power up.
			// NOTE: Sleep mode disables the stepper drivers and position can't be guaranteed.
			// Re-initialize the sleep state as an ALARM mode to ensure user homes or acknowledges.
			if (sys.state & (STATE_ALARM)) {
				report_feedback_message(MESSAGE_ALARM_LOCK);
				system_set_sys_state(STATE_ALARM); // Ensure alarm state is set.
			} else {
				// Check if the safety door is open.
				system_set_sys_state(STATE_IDLE);
				if (system_check_safety_door_ajar()) {
					system_set_exec_state_flag(EXEC_SAFETY_DOOR);
				}
			}
			system_log_sm_idle_log(0);
			grbl_system_sm = sm_idle;
			break;
		}
		case sm_idle : {
			// If there are no more characters in the serial read buffer to be processed and executed,
			// this indicates that g-code streaming has either filled the planner buffer or has
			// completed. In either case, auto-cycle start, if enabled, any queued moves.
			if (system_lastState == 0) {
				if (plan_get_current_block() != NULL) { // Check if there are any blocks in the buffer.
					system_lastState = 1;
					system_set_exec_state_flag(EXEC_CYCLE_START); // If so, execute them!
				}
			} else {
				if (plan_get_current_block() == NULL) { // Check if there are any blocks in the buffer.
					system_lastState = 0;
				}
			}

			if (sys.suspend) {
				protocol_exec_rt_suspend_preapare();
				system_log_sm_suspend_log(0);
				grbl_system_sm = sm_suspend;
			}
			break;
		}
		case sm_suspend : {
			protocol_exec_rt_suspend_async();

			if (sys.suspend == SUSPEND_DISABLE) {
				system_log_sm_idle_log(1);
				grbl_system_sm = sm_idle;
			} else if (system_get_requestSuspendTime()) {
				system_log_sm_suspend_delay_log(0);
				grbl_system_sm = sm_suspend_delay;
			}
			break;
		}
		case sm_suspend_delay : {
			if (read_timer(&system_suspendTime) == 0) {
				system_log_sm_suspend_log(1);
				grbl_system_sm = sm_suspend;
			}
			break;
		}
		default : {
			system_log_sm_init_log(2);
			grbl_system_sm = sm_init;
			break;
		}
	}
}

void isr_system_1ms(void) {
	isr_system_exteral_power_1ms();
}

// Returns if safety door is ajar(T) or closed(F), based on pin state.
uint8 system_check_safety_door_ajar(void) {
	uint8 ctrl_pin_state_SAFETY_DOOR = system_control_get_state_SAFETY_DOOR();
	return(ctrl_pin_state_SAFETY_DOOR);
}

void system_flag_wco_change(void) {
	sys.report_wco_counter = 0;
}

// Returns machine position of axis 'idx'. Must be sent a 'step' array.
// NOTE: If motor steps and machine position are not in the same coordinate frame, this function
//	serves as a central place to compute the transformation.
float system_convert_axis_steps_to_mpos(int32_t *steps, uint8 idx) {
	float pos = 0.0f;
	pos = steps[idx] / settings.steps_per_mm[idx];
	return(pos);
}

void system_convert_array_steps_to_mpos(float *position, int32_t *steps) {
	uint8 idx;
	for (idx=0; idx<N_AXIS; idx++) {
		position[idx] = system_convert_axis_steps_to_mpos(steps, idx);
	}
}

// Checks and reports if target array exceeds machine travel limits.
uint8 system_check_travel_limits(float *target) {
	uint8 idx = 0;
	for (idx = 0; idx < N_AXIS; idx++) {
		// NOTE: max_travel is stored as negative
		if (target[idx] > 0 || target[idx] < settings.max_travel[idx]) {
			return(true);
		}
	}
	return(false);
}

void system_set_exec_motion_override_flag(uint8 mask) {
	lock_isr();
	sys_rt_exec_motion_override |= (mask);
	unlock_isr();
}

void system_set_exec_accessory_override_flag(uint8 mask) {
	lock_isr();
	sys_rt_exec_accessory_override |= (mask);
	unlock_isr();
}

void system_clear_exec_motion_overrides(void) {
	lock_isr();
	sys_rt_exec_motion_override = 0;
	unlock_isr();
}

void system_clear_exec_accessory_overrides(void) {
	lock_isr();
	sys_rt_exec_accessory_override = 0;
	unlock_isr();
}

void system_set_requestSuspendTime(uint16 value) {
	write_timer(&system_suspendTime, value) ;
}

uint8 system_get_requestSuspendTime(void) {
	uint8 result = 0;
	if (read_timer(&system_suspendTime) != 0) {
		result = 1;
	}
	return result;
}

// This function is the general interface to Grbl's real-time command execution system. It is called
// from various check points in the main program, primarily where there may be a while loop waiting
// for a buffer to clear space or any point where the execution time from the last check point may
// be more than a fraction of a second. This is a way to execute realtime commands asynchronously
// (aka multitasking) with grbl's g-code parsing and planning functions. This function also serves
// as an interface for the interrupts to set the system realtime flags, where only the main program
// handles them, removing the need to define more computationally-expensive volatile variables. This
// also provides a controlled way to execute certain tasks without having two or more instances of
// the same task, such as the planner recalculating the buffer upon a feedhold or overrides.
// NOTE: The sys_rt_exec_state variable flags are set by any process, step or serial interrupts, pinouts,
// limit switches, or the main program.
// sys.suspend moved to outside state machine

// Executes run-time commands, when required. This function primarily operates as Grbl's state
// machine and controls the various real-time features Grbl has to offer.
// NOTE: Do not alter this unless you know exactly what you are doing!
void system_exec_rt_system_alarm(void) {
	uint8 rt_exec; // Temp variable to avoid calling volatile multiple times.
	rt_exec = system_get_exec_alarm();
	if (rt_exec) { // Enter only if any bit flag is true
		// System alarm. Everything has shutdown by something that has gone severely wrong. Report
		// the source of the error to the user. If critical, Grbl disables by entering an infinite
		// loop until system reset/abort.
		system_set_sys_state(STATE_ALARM); // Set system alarm state
		report_alarm_message(rt_exec);
		// Halt everything upon a critical event flag. Currently hard and soft limits flag this.
		if ((rt_exec == EXEC_ALARM_HARD_LIMIT) || (rt_exec == EXEC_ALARM_SOFT_LIMIT)) {
			report_feedback_message(MESSAGE_CRITICAL_EVENT);
		}
		if (rt_exec == EXEC_ALARM_EEP_MASTER_READ_FAIL) {
			report_feedback_message(MESSAGE_EEP_MASTER_ERROR);
		}
		if (rt_exec == EXEC_ALARM_EEP_BACKUP_READ_FAIL) {
			report_feedback_message(MESSAGE_EEP_BACKUP_ERROR);
		}
		if (rt_exec == EXEC_ALARM_ABORT_CYCLE) {
			report_feedback_message(MESSAGE_ABORT_CYCLE);
		}
		if (rt_exec == EXEC_ALARM_EXTERNAL_POWER_MISSING) {
			report_feedback_message(MESSAGE_ALARM_EXTERNAL_POWER_OFF);
		}
		system_clear_exec_alarm(); // Clear alarm
	}
}

void system_exec_rt_system_state(void) {
	sys_rt_exec_state_t rt_exec; // Temp variable to avoid calling volatile multiple times.
	rt_exec = system_get_exec_state_flag();
	if (rt_exec) {

		// Execute and serial print status
		if (rt_exec & EXEC_STATUS_REPORT) {
			report_realtime_status();
			system_clear_exec_state_flag(EXEC_STATUS_REPORT);
		}

		// NOTE: Once hold is initiated, the system immediately enters a suspend state to block all
		// main program processes until either reset or resumed. This ensures a hold completes safely.
		if (rt_exec & (EXEC_MOTION_CANCEL | EXEC_FEED_HOLD | EXEC_SAFETY_DOOR)) {

			// State check for allowable states for hold methods.
			if (!(sys.state & (STATE_ALARM))) {

				// If in CYCLE or JOG states, immediately initiate a motion HOLD.
				if (sys.state & (STATE_CYCLE | STATE_JOG)) {
					if (!(sys.suspend & (SUSPEND_MOTION_CANCEL | SUSPEND_JOG_CANCEL))) { // Block, if already holding.
						system_log_st_update_plan_block_parameters(0);
						st_update_plan_block_parameters(); // Notify stepper module to recompute for hold deceleration.
						sys.step_control = STEP_CONTROL_EXECUTE_HOLD; // Initiate suspend state with active flag.
						if (sys.state == STATE_JOG) { // Jog cancelled upon any hold event, except for sleeping.
							sys.suspend |= SUSPEND_JOG_CANCEL;
						}
					}
				}
				// If IDLE, Grbl is not in motion. Simply indicate suspend state and hold is complete.
				if (sys.state == STATE_IDLE) {
					system_set_sys_suspend(SUSPEND_HOLD_COMPLETE);
				}

				// Execute and flag a motion cancel with deceleration and return to idle. Used primarily by probing cycle
				// to halt and cancel the remainder of the motion.
				if (rt_exec & EXEC_MOTION_CANCEL) {
					system_log_EXEC_MOTION_CANCEL(0);

					// MOTION_CANCEL only occurs during a CYCLE, but a HOLD and SAFETY_DOOR may been initiated beforehand
					// to hold the CYCLE. Motion cancel is valid for a single planner block motion only, while jog cancel
					// will handle and clear multiple planner block motions.
					if (!(sys.state & STATE_JOG)) { // NOTE: State is STATE_CYCLE.
						sys.suspend |= SUSPEND_MOTION_CANCEL;
					}
				}

				// Execute a feed hold with deceleration, if required. Then, suspend system.
				if (rt_exec & EXEC_FEED_HOLD) {
					// Block SAFETY_DOOR, JOG, and SLEEP states from changing to HOLD state.
					if (!(sys.state & (STATE_SAFETY_DOOR | STATE_JOG ))) {
						system_set_sys_state(STATE_HOLD);
					}
				}

				// Execute a safety door stop with a feed hold and disable spindle/coolant.
				// NOTE: Safety door differs from feed holds by stopping everything no matter state, disables powered
				// devices (spindle/coolant), and blocks resuming until switch is re-engaged.
				if (rt_exec & EXEC_SAFETY_DOOR) {
					grbl_homing_EXEC_SAFETY_DOOR_event();
					report_feedback_message(MESSAGE_SAFETY_DOOR_AJAR);
					// If jogging, block safety door methods until jog cancel is complete. Just flag that it happened.
					if (!(sys.suspend & SUSPEND_JOG_CANCEL)) {
						// Check if the safety re-opened during a restore parking motion only. Ignore if
						// already retracting, parked or in sleep state.
						if (sys.state == STATE_SAFETY_DOOR) {
							if (sys.suspend & SUSPEND_INITIATE_RESTORE) { // Actively restoring
								sys.suspend &= ~(SUSPEND_RETRACT_COMPLETE | SUSPEND_INITIATE_RESTORE | SUSPEND_RESTORE_COMPLETE);
								sys.suspend |= SUSPEND_RESTART_RETRACT;
							}
						}
						system_set_sys_state(STATE_SAFETY_DOOR);
					}
					// NOTE: This flag doesn't change when the door closes, unlike sys.state. Ensures any parking motions
					// are executed if the door switch closes and the state returns to HOLD.
					sys.suspend |= SUSPEND_SAFETY_DOOR_AJAR;
				}
				
			}

			system_clear_exec_state_flag((EXEC_MOTION_CANCEL | EXEC_FEED_HOLD | EXEC_SAFETY_DOOR));
		} else {
			// Execute a cycle start by starting the stepper interrupt to begin executing the blocks in queue.
			if (rt_exec & EXEC_CYCLE_START) {
				// Block if called at same time as the hold commands: feed hold, motion cancel, and safety door.
				// Ensures auto-cycle-start doesn't resume a hold without an explicit user-input.
				// Resume door state when parking motion has retracted and door has been closed.
				if ((sys.state == STATE_SAFETY_DOOR) && !(sys.suspend & SUSPEND_SAFETY_DOOR_AJAR)) {
					if (sys.suspend & SUSPEND_RESTORE_COMPLETE) {
						system_set_sys_state(STATE_IDLE); // Set to IDLE to immediately resume the cycle.
					} else if (sys.suspend & SUSPEND_RETRACT_COMPLETE) {
						// Flag to re-energize powered components and restore original position, if disabled by SAFETY_DOOR.
						// NOTE: For a safety door to resume, the switch must be closed, as indicated by HOLD state, and
						// the retraction execution is complete, which implies the initial feed hold is not active. To
						// restore normal operation, the restore procedures must be initiated by the following flag. Once,
						// they are complete, it will call CYCLE_START automatically to resume and exit the suspend.
						sys.suspend |= SUSPEND_INITIATE_RESTORE;
					}
				}
				// Cycle start only when IDLE or when a hold is complete and ready to resume.
				if ((sys.state == STATE_IDLE) || ((sys.state & STATE_HOLD) && (sys.suspend & SUSPEND_HOLD_COMPLETE))) {
					if (sys.state == STATE_HOLD && sys.spindle_stop_ovr) {
						sys.spindle_stop_ovr |= SPINDLE_STOP_OVR_RESTORE_CYCLE; // Set to restore in suspend routine and cycle start after.
					} else {
						// Start cycle only if queued motions exist in planner buffer and the motion is not canceled.
						sys.step_control = STEP_CONTROL_NORMAL_OP; // Restore step control to normal operation
						if (plan_get_current_block() && bit_isfalse(sys.suspend, SUSPEND_MOTION_CANCEL)) {
							system_set_sys_suspend(SUSPEND_DISABLE); // Break suspend state.
							system_set_sys_state(STATE_CYCLE);
							st_prep_buffer(); // Initialize step segment buffer before beginning cycle.
							system_log_st_wake_up(0);
							st_wake_up();
						} else { // Otherwise, do nothing. Set and resume IDLE state.
							system_set_sys_suspend(SUSPEND_DISABLE); // Break suspend state.
							system_set_sys_state(STATE_IDLE);
						}
					}
				}
				system_clear_exec_state_flag(EXEC_CYCLE_START);
			}
			
			// Execute a probe start by starting the stepper interrupt to begin executing the blocks in queue.
			if (rt_exec & EXEC_PROBE_START) {
				// Block if called at same time as the hold commands: feed hold, motion cancel, and safety door.
				// Ensures auto-cycle-start doesn't resume a hold without an explicit user-input.
				// Cycle start only when IDLE.
				if (sys.state == STATE_IDLE) {
					// Start cycle only if queued motions exist in planner buffer and the motion is not canceled.
					sys.step_control = STEP_CONTROL_NORMAL_OP; // Restore step control to normal operation
					if (plan_get_current_block() && bit_isfalse(sys.suspend, SUSPEND_MOTION_CANCEL)) {
						system_set_sys_suspend(SUSPEND_DISABLE); // Break suspend state.
						system_set_sys_state(STATE_CYCLE);
						st_prep_buffer(); // Initialize step segment buffer before beginning cycle.
						system_log_st_wake_up(1);
						st_wake_up();
					} else { // Otherwise, do nothing. Set and resume IDLE state.
						system_set_sys_suspend(SUSPEND_DISABLE); // Break suspend state.
						system_set_sys_state(STATE_IDLE);
					}
				}
				system_clear_exec_state_flag(EXEC_PROBE_START);
			}
		}
	
		if (rt_exec & EXEC_CYCLE_STOP) {
			grbl_homing_EXEC_CYCLE_STOP_event();
			// Reinitializes the cycle plan and stepper system after a feed hold for a resume. Called by
			// realtime command execution in the main program, ensuring that the planner re-plans safely.
			// NOTE: Bresenham algorithm variables are still maintained through both the planner and stepper
			// cycle reinitializations. The stepper path should continue exactly as if nothing has happened.
			// NOTE: EXEC_CYCLE_STOP is set by the stepper subsystem when a cycle or feed hold completes.
			if ((sys.state & (STATE_HOLD | STATE_SAFETY_DOOR)) && !(sys.soft_limit) && !(sys.suspend & SUSPEND_JOG_CANCEL)) {
				// Hold complete. Set to indicate ready to resume. Remain in HOLD or DOOR states until user
				// has issued a resume command or reset.
				plan_cycle_reinitialize();
				if (sys.step_control & STEP_CONTROL_EXECUTE_HOLD) {
					sys.suspend |= SUSPEND_HOLD_COMPLETE;
				}
				sys.step_control &= ~(STEP_CONTROL_EXECUTE_HOLD | STEP_CONTROL_EXECUTE_SYS_MOTION);
			} else {
				// Motion complete. Includes CYCLE/JOG/HOMING states and jog cancel/motion cancel/soft limit events.
				// NOTE: Motion and jog cancel both immediately return to idle after the hold completes.
				if (sys.suspend & SUSPEND_JOG_CANCEL) {	// For jog cancel, flush buffers and sync positions.
					sys.step_control = STEP_CONTROL_NORMAL_OP;
					system_log_sm_plan_reset(0);
					plan_reset();
					system_log_st_reset(0);
					st_reset();
					gc_sync_position();
					plan_sync_position();
				}
				if (sys.suspend & SUSPEND_SAFETY_DOOR_AJAR) { // Only occurs when safety door opens during jog.
					sys.suspend &= ~(SUSPEND_JOG_CANCEL);
					sys.suspend |= SUSPEND_HOLD_COMPLETE;
					system_set_sys_state(STATE_SAFETY_DOOR);
				} else {
					system_set_sys_suspend(SUSPEND_DISABLE);
					if (sys.state != STATE_ALARM) {
						system_set_sys_state(STATE_IDLE);
					}
				}
			}
			system_clear_exec_state_flag(EXEC_CYCLE_STOP);
		}

		if (rt_exec & EXEC_ABORT) {
			stepper_enable_emergency_stop();
			system_log_spindle_off(0);
			spindle_set_state(SPINDLE_DISABLE, 0.0f);
			system_set_exec_alarm(EXEC_ALARM_ABORT_CYCLE);		
			//Empty out other movements
			system_log_sm_plan_reset(1);
			plan_reset();
			system_log_st_reset(1);
			st_reset();
			gc_sync_position();
			plan_sync_position(); 

			system_clear_exec_state_flag(EXEC_ABORT);
		}
		if (rt_exec & EXEC_CLEAR_ALARM) {
			if (stepper_enable_emergency_stop_cleared(3) == 0) {
				report_feedback_message(MESSAGE_ALARM_UNLOCK_FAILED_EMERGENCYSTOP);
			} else if (system_get_external_power_disengage_alarm() == 0) {
				report_feedback_message(MESSAGE_ALARM_UNLOCK_FAILED_POWER);
			} else {
				report_feedback_message(MESSAGE_ALARM_UNLOCK);
				system_log_unlock();
				system_set_sys_state(STATE_IDLE);
				// Don't run startup script. Prevents stored moves in startup from causing accidents.
			}
			system_clear_exec_state_flag(EXEC_CLEAR_ALARM);
		}
	}
}

void system_exec_rt_system_override(void) {
	uint8 rt_exec; // Temp variable to avoid calling volatile multiple times.
	// Execute overrides.
	rt_exec = sys_rt_exec_motion_override; // Copy volatile sys_rt_exec_motion_override
	if (rt_exec) {
		system_clear_exec_motion_overrides(); // Clear all motion override flags.

		uint8 new_f_override = sys.f_override;
		if (rt_exec & EXEC_FEED_OVR_RESET) { new_f_override = DEFAULT_FEED_OVERRIDE; }
		if (rt_exec & EXEC_FEED_OVR_COARSE_PLUS) { new_f_override += FEED_OVERRIDE_COARSE_INCREMENT; }
		if (rt_exec & EXEC_FEED_OVR_COARSE_MINUS) { new_f_override -= FEED_OVERRIDE_COARSE_INCREMENT; }
		if (rt_exec & EXEC_FEED_OVR_FINE_PLUS) { new_f_override += FEED_OVERRIDE_FINE_INCREMENT; }
		if (rt_exec & EXEC_FEED_OVR_FINE_MINUS) { new_f_override -= FEED_OVERRIDE_FINE_INCREMENT; }
		new_f_override = min(new_f_override,MAX_FEED_RATE_OVERRIDE);
		new_f_override = max(new_f_override,MIN_FEED_RATE_OVERRIDE);

		uint8 new_r_override = sys.r_override;
		if (rt_exec & EXEC_RAPID_OVR_RESET) { new_r_override = DEFAULT_RAPID_OVERRIDE; }
		if (rt_exec & EXEC_RAPID_OVR_MEDIUM) { new_r_override = RAPID_OVERRIDE_MEDIUM; }
		if (rt_exec & EXEC_RAPID_OVR_LOW) { new_r_override = RAPID_OVERRIDE_LOW; }

		if ((new_f_override != sys.f_override) || (new_r_override != sys.r_override)) {
			sys.f_override = new_f_override;
			sys.r_override = new_r_override;
			sys.report_ovr_counter = 0; // Set to report change immediately
			plan_update_velocity_profile_parameters();
			plan_cycle_reinitialize();
		}
	}

	rt_exec = sys_rt_exec_accessory_override;
	if (rt_exec) {
		system_clear_exec_accessory_overrides(); // Clear all accessory override flags.

		// NOTE: Unlike motion overrides, spindle overrides do not require a planner reinitialization.
		uint8 last_s_override = sys.spindle_speed_ovr;
		if (rt_exec & EXEC_SPINDLE_OVR_RESET) { last_s_override = DEFAULT_SPINDLE_SPEED_OVERRIDE; }
		if (rt_exec & EXEC_SPINDLE_OVR_COARSE_PLUS) { last_s_override += SPINDLE_OVERRIDE_COARSE_INCREMENT; }
		if (rt_exec & EXEC_SPINDLE_OVR_COARSE_MINUS) { last_s_override -= SPINDLE_OVERRIDE_COARSE_INCREMENT; }
		if (rt_exec & EXEC_SPINDLE_OVR_FINE_PLUS) { last_s_override += SPINDLE_OVERRIDE_FINE_INCREMENT; }
		if (rt_exec & EXEC_SPINDLE_OVR_FINE_MINUS) { last_s_override -= SPINDLE_OVERRIDE_FINE_INCREMENT; }
		last_s_override = min(last_s_override,MAX_SPINDLE_SPEED_OVERRIDE);
		last_s_override = max(last_s_override,MIN_SPINDLE_SPEED_OVERRIDE);

		if (last_s_override != sys.spindle_speed_ovr) {
			sys.step_control |= STEP_CONTROL_UPDATE_SPINDLE_PWM;
			sys.spindle_speed_ovr = last_s_override;
			sys.report_ovr_counter = 0; // Set to report change immediately
		}

		if (rt_exec & EXEC_SPINDLE_OVR_STOP) {
			// Spindle stop override allowed only while in HOLD state.
			// NOTE: Report counters are set in spindle_set_state() when spindle stop is executed.
			if (sys.state == STATE_HOLD) {
				if (!(sys.spindle_stop_ovr)) { sys.spindle_stop_ovr = SPINDLE_STOP_OVR_INITIATE; }
				else if (sys.spindle_stop_ovr & SPINDLE_STOP_OVR_ENABLED) { sys.spindle_stop_ovr |= SPINDLE_STOP_OVR_RESTORE; }
			}
		}

		// NOTE: Since coolant state always performs a planner sync whenever it changes, the current
		// run state can be determined by checking the parser state.
		if (rt_exec & (EXEC_COOLANT_FLOOD_OVR_TOGGLE | EXEC_COOLANT_MIST_OVR_TOGGLE)) {
			if ((sys.state == STATE_IDLE) || (sys.state & (STATE_CYCLE | STATE_HOLD))) {
				uint8 coolant_state = gc_state.modal.coolant;
				#ifdef ENABLE_M7
					if (rt_exec & EXEC_COOLANT_MIST_OVR_TOGGLE) {
						if (coolant_state & COOLANT_MIST_ENABLE) {
							coolant_state &= ~(COOLANT_MIST_ENABLE); 
						}
						else { coolant_state |= COOLANT_MIST_ENABLE; }
					}
					if (rt_exec & EXEC_COOLANT_FLOOD_OVR_TOGGLE) {
						if (coolant_state & COOLANT_FLOOD_ENABLE) {
							coolant_state &= ~ (COOLANT_FLOOD_ENABLE); 
						}
						else { 
							coolant_state |= COOLANT_FLOOD_ENABLE; 
						}
					}
				#else
					if (coolant_state & COOLANT_FLOOD_ENABLE) { 
						coolant_state &= ~(COOLANT_FLOOD_ENABLE); 
					} else { 
						coolant_state |= COOLANT_FLOOD_ENABLE; 
					}
				#endif
				coolant_set_state(coolant_state); // Report counter set in coolant_set_state().
				gc_state.modal.coolant = coolant_state;
			}
		}
	}
}

void protocol_exec_rt_suspend_preapare(void) {
	plan_block_t *block = plan_get_current_block();
	restore_spindle_speed = 0.0f;
	if (block == NULL) {
		restore_condition = (gc_state.modal.spindle | gc_state.modal.coolant);
		restore_spindle_speed = gc_state.spindle_speed;
	} else {
		restore_condition = block->condition;
		restore_spindle_speed = block->spindle_speed;
	}
	#ifdef DISABLE_LASER_DURING_HOLD
		if (bit_istrue(settings.flags,BITFLAG_LASER_MODE)) { 
			system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_STOP);
		}
	#endif
}

// Handles Grbl system suspend procedures, such as feed hold, safety door, and parking motion.
// The system will enter this loop, create local variables for suspend tasks, and return to
// whatever function that invoked the suspend, such that Grbl resumes normal operation.
// This function is written in a way to promote custom parking motions. Simply use this as a
// template
void protocol_exec_rt_suspend_async(void) {
	// Block until initial hold is complete and the machine has stopped motion.
	if (sys.suspend & SUSPEND_HOLD_COMPLETE) {

		// Parking manager. Handles de/re-energizing, switch state checks, and parking motions for 
		// the safety door and sleep states.
		if (sys.state & (STATE_SAFETY_DOOR)) {
			// Handles retraction motions and de-energizing.
			if (bit_isfalse(sys.suspend, SUSPEND_RETRACT_COMPLETE)) {

				// Ensure any prior spindle stop override is disabled at start of safety door routine.
				sys.spindle_stop_ovr = SPINDLE_STOP_OVR_DISABLED;

				system_log_spindle_off(1);
				spindle_set_state(SPINDLE_DISABLE, 0.0);	// De-energize
				coolant_set_state(COOLANT_DISABLE);		// De-energize

				sys.suspend &= ~(SUSPEND_RESTART_RETRACT);
				sys.suspend |= SUSPEND_RETRACT_COMPLETE;

			} else {
				// Allows resuming from parking/safety door. Actively checks if safety door is closed and ready to resume.
				if (sys.state == STATE_SAFETY_DOOR) {
					if (!(system_check_safety_door_ajar())) {
						sys.suspend &= ~(SUSPEND_SAFETY_DOOR_AJAR); // Reset door ajar flag to denote ready to resume.
						system_set_exec_state_flag(EXEC_CYCLE_START);
						sys.suspend |= SUSPEND_INITIATE_RESTORE;
					}
				}

				// Handles parking restore and safety door resume.
				if (sys.suspend & SUSPEND_INITIATE_RESTORE) {

					// Delayed Tasks: Restart spindle and coolant, delay to power-up, then resume cycle.
					if (gc_state.modal.spindle != SPINDLE_DISABLE) {
						// Block if safety door re-opened during prior restore actions.
						if (bit_isfalse(sys.suspend,SUSPEND_RESTART_RETRACT)) {
							if (bit_istrue(settings.flags,BITFLAG_LASER_MODE)) {
								// When in laser mode, ignore spindle spin-up delay. Set to turn on laser when cycle starts.
								sys.step_control |= STEP_CONTROL_UPDATE_SPINDLE_PWM;
							} else {
								if (restore_spindle_speed == 0.0f) {
									system_log_spindle_off(2);
								} else {
									system_log_spindle_on(3);
								}
								spindle_set_state((restore_condition & (PL_COND_FLAG_SPINDLE_CW | PL_COND_FLAG_SPINDLE_CCW)), restore_spindle_speed);
								system_set_requestSuspendTime(SAFETY_DOOR_SPINDLE_DELAY);
								//delay_sec(SAFETY_DOOR_SPINDLE_DELAY, DELAY_MODE_SYS_SUSPEND);
							}
						}
					}
					if (gc_state.modal.coolant != COOLANT_DISABLE) {
						// Block if safety door re-opened during prior restore actions.
						if (bit_isfalse(sys.suspend,SUSPEND_RESTART_RETRACT)) {
							// NOTE: Laser mode will honor this delay. An exhaust system is often controlled by this pin.
							coolant_set_state((restore_condition & (PL_COND_FLAG_COOLANT_FLOOD | PL_COND_FLAG_COOLANT_FLOOD)));
							system_set_requestSuspendTime(SAFETY_DOOR_COOLANT_DELAY);
							//delay_sec(SAFETY_DOOR_COOLANT_DELAY, DELAY_MODE_SYS_SUSPEND);
						}
					}

					if (bit_isfalse(sys.suspend,SUSPEND_RESTART_RETRACT)) {
						sys.suspend |= SUSPEND_RESTORE_COMPLETE;
						system_set_exec_state_flag(EXEC_CYCLE_START); // Set to resume program.
					}
				}
			}
		} else {
			// Feed hold manager. Controls spindle stop override states.
			// NOTE: Hold ensured as completed by condition check at the beginning of suspend routine.
			if (sys.spindle_stop_ovr) {
				// Handles beginning of spindle stop
				if (sys.spindle_stop_ovr & SPINDLE_STOP_OVR_INITIATE) {
					if (gc_state.modal.spindle != SPINDLE_DISABLE) {
						system_log_spindle_off(4);
						spindle_set_state(SPINDLE_DISABLE,0.0); // De-energize
						sys.spindle_stop_ovr = SPINDLE_STOP_OVR_ENABLED; // Set stop override state to enabled, if de-energized.
					} else {
						sys.spindle_stop_ovr = SPINDLE_STOP_OVR_DISABLED; // Clear stop override state
					}
					// Handles restoring of spindle state
				} else if (sys.spindle_stop_ovr & (SPINDLE_STOP_OVR_RESTORE | SPINDLE_STOP_OVR_RESTORE_CYCLE)) {
					if (gc_state.modal.spindle != SPINDLE_DISABLE) {
						report_feedback_message(MESSAGE_SPINDLE_RESTORE);
						if (bit_istrue(settings.flags,BITFLAG_LASER_MODE)) {
							// When in laser mode, ignore spindle spin-up delay. Set to turn on laser when cycle starts.
							sys.step_control |= STEP_CONTROL_UPDATE_SPINDLE_PWM;
						} else {
							if (restore_spindle_speed == 0.0f) {
								system_log_spindle_off(5);
							} else {
								system_log_spindle_on(6);
							}
							spindle_set_state((restore_condition & (PL_COND_FLAG_SPINDLE_CW | PL_COND_FLAG_SPINDLE_CCW)), restore_spindle_speed);
						}
					}
					if (sys.spindle_stop_ovr & SPINDLE_STOP_OVR_RESTORE_CYCLE) {
						system_set_exec_state_flag(EXEC_CYCLE_START);	// Set to resume program.
					}
					sys.spindle_stop_ovr = SPINDLE_STOP_OVR_DISABLED; // Clear stop override state
				}
			} else {
				// Handles spindle state during hold. NOTE: Spindle speed overrides may be altered during hold state.
				// NOTE: STEP_CONTROL_UPDATE_SPINDLE_PWM is automatically reset upon resume in step generator.
				if (bit_istrue(sys.step_control, STEP_CONTROL_UPDATE_SPINDLE_PWM)) {
					if (restore_spindle_speed == 0.0f) {
						system_log_spindle_off(7);
					} else {
						system_log_spindle_on(8);
					}
					spindle_set_state((restore_condition & (PL_COND_FLAG_SPINDLE_CW | PL_COND_FLAG_SPINDLE_CCW)), restore_spindle_speed);
					sys.step_control &= ~(STEP_CONTROL_UPDATE_SPINDLE_PWM);
				}
			}
		}
	}
}


void grbl_set_serial_number(unsigned char *str) {
	if (str != NULL) {
		unsigned int value = hexStringToInt(&str[0]);
		if (
			(grbl_serial_number == 0xFFFFFFFF) ||
			((grbl_serial_number & 0xFF000000)== 0xFF000000) ||
			((grbl_serial_number & 0xFF000000)== 0x1F000000) ||
			((grbl_serial_number & 0xFF000000)== 0x3F000000)
		) {
			grbl_serial_number = value;
		}
	}
}

void system_log_add_event(grbl_system_log ev, unsigned int caller) {
	unsigned int x = 0;
	unsigned int cnt = (sizeof(system_log) / sizeof(*system_log));
	for (x = 0; x < (cnt - 1) ; x++) {//1 element less!
		grbl_system_logger[(cnt - 1) - x].system_log = 			grbl_system_logger[(cnt - 1) - x - 1].system_log;
		grbl_system_logger[(cnt - 1) - x].system_log_caller = 	grbl_system_logger[(cnt - 1) - x - 1].system_log_caller;
		grbl_system_logger[(cnt - 1) - x].system_log_time = 	grbl_system_logger[(cnt - 1) - x - 1].system_log_time;
	}
	grbl_system_logger[0].system_log = ev;
	grbl_system_logger[0].system_log_caller = caller;
	grbl_system_logger[0].system_log_time = getGlobalTime();
}

void system_set_sys_state(unsigned int new_sys_state) {
	system_log_sm_sys_state(new_sys_state);
	sys.state = new_sys_state;
}

void system_set_sys_suspend(unsigned int new_sys_suspend) {
	system_log_sm_sys_suspend(new_sys_suspend);
	sys.suspend = new_sys_suspend;
}

void system_log_unlock(void) {
	system_log_add_event(sm_unlock, 0);
}

void system_log_sm_abort_input(void) {
	system_log_add_event(sm_abort_input, 0);
}

void system_log_internal_stepper_enable(unsigned int caller) {
	system_log_add_event(sm_internal_stepper_enable, caller);
}

void system_log_internal_stepper_disable(unsigned int caller) {
	system_log_add_event(sm_internal_stepper_disable, caller);
}

void system_log_home(void) {
	system_log_add_event(sm_home, 0);
}

void system_log_limit_x(void) {
	system_log_add_event(sm_limit_x, 0);
}

void system_log_limit_y(void) {
	system_log_add_event(sm_limit_y, 0);
}

void system_log_limit_z(void) {
	system_log_add_event(sm_limit_z, 0);
}

void system_log_home_finished(void) {
	system_log_add_event(sm_home_finished, 0);
}

void system_log_spindle_on(unsigned int caller) {
	system_log_add_event(sm_home_spindle_on, caller);
}

void system_log_spindle_off(unsigned int caller) {
	system_log_add_event(sm_home_spindle_off, caller);
}

void system_log_probe_start(unsigned int caller) {
	system_log_add_event(sm_probe_start, caller);
}

void system_log_probe_touch(unsigned int caller) {
	system_log_add_event(sm_probe_touch, caller);
}

void system_log_st_reset(unsigned int caller) {
	system_log_add_event(sm_st_reset, caller);
}

void system_log_sm_plan_reset(unsigned int caller) {
	system_log_add_event(sm_plan_reset, caller);
}

void system_log_st_go_idle(unsigned int caller) {
	system_log_add_event(sm_st_go_idle, caller);
}

void system_log_st_wake_up(unsigned int caller) {
	system_log_add_event(sm_st_wake_up, caller);
}

void system_log_sm_g28(unsigned int caller) {
	system_log_add_event(sm_g28, caller);
}

void system_log_st_update_plan_block_parameters(unsigned int caller) {
	system_log_add_event(sm_st_update_plan_block_parameters, caller);
}

void system_log_EXEC_MOTION_CANCEL(unsigned int caller) {
	system_log_add_event(sm_EXEC_MOTION_CANCEL, caller);
}

void system_log_sm_init_log(unsigned int caller) {
	system_log_add_event(sm_init_log, caller);
}

void system_log_sm_idle_log(unsigned int caller) {
	system_log_add_event(sm_idle_log, caller);
}

void system_log_sm_suspend_log(unsigned int caller) {
	system_log_add_event(sm_suspend_log, caller);
}

void system_log_sm_suspend_delay_log(unsigned int caller) {
	system_log_add_event(sm_suspend_delay_log, caller);
}

void system_log_sm_sys_state(unsigned int caller) {
	system_log_add_event(sm_sys_state, caller);
}

void system_log_sm_sys_suspend(unsigned int caller) {
	system_log_add_event(sm_sys_suspend, caller);
}
