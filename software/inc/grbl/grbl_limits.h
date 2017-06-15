/*
  limits.h - code pertaining to limit-switches and performing the homing cycle
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

#ifndef limits_grbl_h
#define limits_grbl_h 

#include "k_stdtype.h"

// Initialize the limits module
extern void limits_init(void);

extern void do_limits(void);

extern void isr_limits_1ms(void);

extern uint8 isLimits_go_homeRunning(void);

// Disables hard limits.
extern void limits_disable(void);

extern void limits_enable(void);

// Returns limit state as a bit-wise uint8 variable.
extern uint8 limits_get_state_filtered(void);

// Perform one portion of the homing cycle based on the input settings.
extern void limits_go_home(uint8 cycle_mask);

// Check for soft limit violations
extern void limits_soft_check(float *target);

extern void limits_EXEC_CYCLE_STOP_event(void);
extern void limits_EXEC_SAFETY_DOOR_event(void);

extern void LIMIT_INT_vect(void);

extern int32_t grbl_limit_calculate_set_axis_position(uint8 idx);

#endif