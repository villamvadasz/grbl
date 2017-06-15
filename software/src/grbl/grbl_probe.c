/*
  probe.c - code pertaining to probing methods
  Part of Grbl

  Copyright (c) 2014-2016 Sungeun K. Jeon for Gnea Research LLC

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
#include "grbl.h"

#include "k_stdtype.h"

#ifndef PROBE_PIN
	/* If no Probe PIN is configured, then the probeing will generate a result after 10000ms*/
	#define GRBL_PROBE_SIMUALTING
#endif

// Inverts the probe pin state depending on user settings and probing cycle mode.
uint8 probe_invert_mask = 0;
volatile uint8 probe_pin_state = 0;
volatile uint8 do_probe_1ms = 0;
volatile uint8 sys_probe_state = 0;				// Probing state value. Used to coordinate the probing cycle with stepper ISR.
volatile uint8 do_mc_probe_cycle = 0;
uint8 probe_is_no_error = 0;
#ifdef GRBL_PROBE_SIMUALTING
	unsigned int grbl_probe_simulation = 0;
	unsigned int grbl_probe_simulation_trigger = 0;
#endif

uint8 mc_probe_cycle_async(void);

// Probe pin initialization routine.
void probe_init(void) {
	// Inverts the probe pin state depending on user settings and probing cycle mode.
	probe_invert_mask = 0;
	probe_pin_state = 0;
	do_probe_1ms = 0;
	do_mc_probe_cycle = 0;
	probe_is_no_error = 0;
	#ifdef GRBL_PROBE_SIMUALTING
		grbl_probe_simulation = 0;
		grbl_probe_simulation_trigger = 0;
	#endif


	#ifdef PROBE_TRIS
		PROBE_TRIS = 1; // Configure as input pins
	#endif
	#ifdef PROBE_PIN_PUE
		PROBE_PIN_PUE = 1;
	#endif
	#ifdef PROBE_PIN_CN
		#warning TODO Feature is not implemented yet for probeing
		PROBE_PIN_CN = 1;
	#endif

	#ifdef PROBE_NEGATE_TRIS
		PROBE_NEGATE_TRIS = 1; // Configure as input pins
	#endif
	#ifdef PROBE_NEGATE_PIN_PUE
		PROBE_NEGATE_PIN_PUE = 1;
	#endif
	#ifdef PROBE_NEGATE_PIN_CN
		#warning TODO Feature is not implemented yet for probeing
		PROBE_NEGATE_PIN_CN = 1;
	#endif

	probe_configure_invert_mask(false); // Initialize invert mask.
	sys_probe_state = 0;
}

void do_probe(void) {
	if (do_probe_1ms) {
		do_probe_1ms = 0;
		probe_pin_state = probe_get_state();
		
		#ifdef GRBL_PROBE_SIMUALTING
			if (do_mc_probe_cycle) {
				if (grbl_probe_simulation != 0) {
					grbl_probe_simulation--;
				} else {
					grbl_probe_simulation_trigger = 1;
				}				
			}
		#endif
		
	}
	if (do_mc_probe_cycle) {
		
		uint8 gc_update_pos_result = mc_probe_cycle_async();
		
		switch (gc_update_pos_result) {
			case GC_UPDATE_POS_PENDING : {
				//do calling further
				break;
			}
			case GC_UPDATE_POS_TARGET : {
				gcode_probe_update_pos();
				do_mc_probe_cycle = 0;
				break;
			}
			case GC_UPDATE_POS_SYSTEM : {
				gc_sync_position(); // gc_state.position[] = sys_position
				do_mc_probe_cycle = 0;
				break;
			}
			case GC_UPDATE_POS_NONE : {
				//some error
				do_mc_probe_cycle = 0;
			}
			default : {
				do_mc_probe_cycle = 0;
				break;
			}
		}
	}
}

void isr_probe_1ms(void) {
	do_probe_1ms = 1;
}

// Called by probe_init() and the mc_probe() routines. Sets up the probe pin invert mask to
// appropriately set the pin logic according to setting for normal-high/normal-low operation
// and the probing cycle modes for toward-workpiece/away-from-workpiece.
void probe_configure_invert_mask(uint8 is_probe_away) {
	//TODO remove this function. Does not makes much sense, can be done easier
	probe_invert_mask = 0; // Initialize as zero.
	if (bit_isfalse(settings.flags, BITFLAG_INVERT_PROBE_PIN)) {
		probe_invert_mask ^= PROBE_MASK;
	}
	if (is_probe_away) {
		probe_invert_mask ^= PROBE_MASK;
	}
}

// Returns the probe pin state. Triggered = true. Called by gcode parser and probe state monitor.
uint8 probe_get_state(void) {
	uint8 pin = 0;
	#ifdef PROBE_PIN
		if (PROBE_PIN != 0) {
			pin = 1;
		}
	#endif
	#ifdef PROBE_NEGATE_PIN
		if (PROBE_NEGATE_PIN == 0) {
			pin = 1;
		}
	#endif
	pin ^= probe_invert_mask;
	#ifdef GRBL_PROBE_SIMUALTING
		if (grbl_probe_simulation_trigger) {
			grbl_probe_simulation_trigger = 0;
			pin = 1;
		}
	#endif
	return(pin);
}

// Monitors probe pin state and records the system position when detected. Called by the
// stepper ISR per ISR tick.
// NOTE: This function must be extremely efficient as to not bog down the stepper ISR.
void probe_state_monitor(void) {
	if (probe_get_state()) {
		system_log_probe_touch(0);
		sys_probe_state = PROBE_OFF;
		memcpy(sys_probe_position, sys_position, sizeof(sys_position));
		system_set_exec_state_flag(EXEC_MOTION_CANCEL);
	}
}

uint8 isProbing_cycleRunning(void) {
	uint8 result = 0;
	if (do_mc_probe_cycle != 0) {
		result = 1;
	}
	return result;
}

// Perform tool length probe cycle. Requires probe switch.
// NOTE: Upon probe failure, the program will be stopped and placed into ALARM state.
void mc_probe_cycle(float *target, plan_line_data_t *pl_data, uint8 parser_flags) {
	system_log_probe_start(0);
	// TODO: Need to update this cycle so it obeys a non-auto cycle start.
	uint8 is_probe_away = bit_istrue(parser_flags, GC_PARSER_PROBE_IS_AWAY);
	probe_is_no_error = bit_istrue(parser_flags, GC_PARSER_PROBE_IS_NO_ERROR);
	
	// Initialize probing control variables
	sys.probe_succeeded = false; // Re-initialize probe history before beginning cycle.
	probe_configure_invert_mask(is_probe_away);

	// After syncing, check if probe is already triggered. If so, halt and issue alarm.
	// NOTE: This probe initialization error applies to all probing cycles.
	if ( probe_get_state() ) { // Check probe pin state.
		system_set_exec_alarm(EXEC_ALARM_PROBE_FAIL_INITIAL);
		probe_configure_invert_mask(false); // Re-initialize invert mask before returning.
	} else {
		// Setup and queue probing motion. Auto cycle-start should not start the cycle.
		mc_line(target, pl_data);
		// Activate the probing state monitor in the stepper module.
		sys_probe_state = PROBE_ACTIVE;
		// Perform probing cycle. Wait here until probe is triggered or motion completes.
		system_set_exec_state_flag(EXEC_PROBE_START);

		do_mc_probe_cycle = 1;
		#ifdef GRBL_PROBE_SIMUALTING
			grbl_probe_simulation_trigger = 0;
			grbl_probe_simulation = 10000;
		#endif
	}
}

uint8 mc_probe_cycle_async(void) {
	uint8 result = GC_UPDATE_POS_PENDING;

	sys_rt_exec_state_t rt_exec; // Temp variable to avoid calling volatile multiple times.
	rt_exec = system_get_exec_state_flag();
	
	if (rt_exec & EXEC_PROBE_START) {
		//Probeing issued a movement start but it was not yet processed
	} else {
		if (sys.state != STATE_IDLE) {
			//Probing running
		} else {
			system_log_probe_touch(1);
			// Probing cycle complete!
			// Set state variables and error out, if the probe failed and cycle with error is enabled.
			if (sys_probe_state == PROBE_ACTIVE) {
				if (probe_is_no_error) {
					memcpy(sys_probe_position, sys_position, sizeof(sys_position));
				} else {
					system_set_exec_alarm(EXEC_ALARM_PROBE_FAIL_CONTACT);
				}
			} else {
				sys.probe_succeeded = true; // Indicate to system the probing cycle completed successfully.
			}
			sys_probe_state = PROBE_OFF; // Ensure probe state monitor is disabled.
			probe_configure_invert_mask(false); // Re-initialize invert mask.

			//Original called protocol_execute_realtime, might be needed here too.
			
			// Reset the stepper and planner buffers to remove the remainder of the probe motion.
			system_log_st_reset(4);
			st_reset(); // Reset step segment buffer.
			system_log_sm_plan_reset(4);
			plan_reset(); // Reset planner buffer. Zero planner positions. Ensure probing motion is cleared.
			plan_sync_position(); // Sync planner position to current machine position.

			#ifdef MESSAGE_PROBE_COORDINATES
				// All done! Output the probe position as message.
				report_probe_parameters();
			#endif

			if (sys.probe_succeeded) {
				result = GC_UPDATE_POS_SYSTEM;
			} else {
				result = GC_UPDATE_POS_TARGET;
			} // Failed to trigger probe within travel. With or without error.
		}
	}
	return result;
}
