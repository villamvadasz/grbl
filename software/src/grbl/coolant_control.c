/*
  coolant_control.c - coolant control methods
  Part of Grbl

  Copyright (c) 2012-2016 Sungeun K. Jeon for Gnea Research LLC

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

void coolant_init(void ) {
    #ifdef COOLANT_FLOOD_DDR
        COOLANT_FLOOD_DDR = 0;
    #endif
	#ifdef COOLANT_MIST_DDR
		COOLANT_MIST_DDR = 0;
	#endif
    coolant_stop();
}

// Returns current coolant output state. Overrides may alter it from programmed state.
uint8 coolant_get_state(void) {
	uint8 cl_state = COOLANT_STATE_DISABLE;
    #ifdef COOLANT_FLOOD_PORT
		#ifdef INVERT_COOLANT_FLOOD_PIN
			if (COOLANT_FLOOD_PORT == 0) {
				cl_state |= COOLANT_STATE_FLOOD;
			}
		#else
			if (COOLANT_FLOOD_PORT != 0) {
				cl_state |= COOLANT_STATE_FLOOD;
			}
		#endif
	#endif
	#ifdef COOLANT_MIST_PORT
		#ifdef INVERT_COOLANT_MIST_PIN
			if (COOLANT_MIST_PORT == 0) {
				cl_state |= COOLANT_STATE_MIST;
			}
		#else
			if (COOLANT_MIST_PORT != 0) {
				cl_state |= COOLANT_STATE_MIST;
			}
		#endif
	#endif
	return(cl_state);
}

void coolant_stop(void) {
    #ifdef COOLANT_FLOOD_PORT
		#ifdef INVERT_COOLANT_FLOOD_PIN
			COOLANT_FLOOD_PORT = 1;
		#else
			COOLANT_FLOOD_PORT = 0;
		#endif
	#endif
	#ifdef COOLANT_MIST_PORT
		#ifdef INVERT_COOLANT_MIST_PIN
			COOLANT_MIST_PORT = 1;
		#else
			COOLANT_MIST_PORT = 0;
		#endif
	#endif
}

// Main program only. Immediately sets flood coolant running state and also mist coolant, 
// if enabled. Also sets a flag to report an update to a coolant state.
// Called by coolant toggle override, parking restore, parking retract, sleep mode, g-code
// parser program end, and g-code parser coolant_sync().
void coolant_set_state(uint8 mode) {
	if (mode == COOLANT_DISABLE) {
		coolant_stop(); 
	} else {
		if (mode & COOLANT_FLOOD_ENABLE) {
			#ifdef COOLANT_FLOOD_PORT
				#ifdef INVERT_COOLANT_FLOOD_PIN
					COOLANT_FLOOD_PORT = 0;
				#else
					COOLANT_FLOOD_PORT = 1;
				#endif
			#endif
		}
		#ifdef COOLANT_MIST_PORT
			if (mode & COOLANT_MIST_ENABLE) {
				#ifdef INVERT_COOLANT_MIST_PIN
					COOLANT_MIST_PORT = 0;
				#else
					COOLANT_MIST_PORT = 1;
				#endif
			}
		#endif
	}
	sys.report_ovr_counter = 0; // Set to report change immediately
}

// G-code parser entry-point for setting coolant state. Forces a planner buffer sync and bails 
// if an abort or check-mode is active.
void coolant_sync(uint8 mode) {
	coolant_set_state(mode);
}
