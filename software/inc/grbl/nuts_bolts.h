/*
  nuts_bolts.h - Header file for shared definitions, variables, and functions
  Part of Grbl

  Copyright (c) 2011-2016 Sungeun K. Jeon for Gnea Research LLC
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

#ifndef nuts_bolts_h
#define nuts_bolts_h

#include "k_stdtype.h"
#include "cpu_map.h"
#include "c_main.h"

#define false 0
#define true 1

#define SOME_LARGE_VALUE 1.0E+38

//This is const now, since the port bits are handled separated.
//Still needed since info is stored in variables and this shows which bit is which axis
#define X_STEP_BIT      0  // Uno Digital Pin 2
#define Y_STEP_BIT      1  // Uno Digital Pin 3
#define Z_STEP_BIT      2  // Uno Digital Pin 4
#define STEP_MASK       ((1<<X_STEP_BIT)|(1<<Y_STEP_BIT)|(1<<Z_STEP_BIT)) // All step bits
#define X_DIRECTION_BIT   0  // Uno Digital Pin 5
#define Y_DIRECTION_BIT   1  // Uno Digital Pin 6
#define Z_DIRECTION_BIT   2  // Uno Digital Pin 7
#define DIRECTION_MASK    ((1<<X_DIRECTION_BIT)|(1<<Y_DIRECTION_BIT)|(1<<Z_DIRECTION_BIT)) // All direction bits
#define STEPPERS_DISABLE_BIT    0  // Uno Digital Pin 8
#define STEPPERS_DISABLE_MASK   (1<<STEPPERS_DISABLE_BIT)
#define X_LIMIT_BIT      0  // Uno Digital Pin 9
#define Y_LIMIT_BIT      1  // Uno Digital Pin 10
#define Z_LIMIT_BIT    2  // Uno Digital Pin 11
#define LIMIT_MASK       ((1<<X_LIMIT_BIT)|(1<<Y_LIMIT_BIT)|(1<<Z_LIMIT_BIT)) // All limit bits
#define PROBE_BIT       0  // Uno Analog Pin 5
#define PROBE_MASK      (1<<PROBE_BIT)

#define INVERT_CONTROL_PIN_FEED_HOLD
#define INVERT_CONTROL_PIN_CYCLE_START
#define INVERT_CONTROL_PIN_SAFETY_DOOR

// Axis array index values. Must start with 0 and be continuous.
#define N_AXIS 3 // Number of axes
#define X_AXIS 0 // Axis indexing value.
#define Y_AXIS 1
#define Z_AXIS 2
// #define A_AXIS 3

// Conversions
#define MM_PER_INCH (25.40)
#define INCH_PER_MM (0.0393701)
#define TIMER_PRESACLER 4
#ifdef CPU_MAP_PIC32_MZ
    #define TICKS_PER_MICROSECOND (GetPeripheral3Clock()/1000000/TIMER_PRESACLER)
#endif
#ifdef CPU_MAP_PIC32_MX
    #define TICKS_PER_MICROSECOND (GetPeripheralClock()/1000000/TIMER_PRESACLER)
#endif

#define DELAY_MODE_DWELL       0
#define DELAY_MODE_SYS_SUSPEND 1

#ifndef max
	#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
	#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

// Bit field and masking macros
#define bit(n) (1 << n)
#define bit_istrue(x,mask) ((x & mask) != 0)
#define bit_isfalse(x,mask) ((x & mask) == 0)

extern uint8 grbl_not_existing_tris;
extern uint8 grbl_not_existing_port;
extern uint8 grbl_not_existing_lat;

extern void init_nuts_bolts(void);
extern void do_nuts_bolts(void);
extern void isr_nuts_bolts_1ms(void);

// Read a floating point value from a string. Line points to the input buffer, char_counter
// is the indexer pointing to the current character of the line, while float_ptr is
// a pointer to the result variable. Returns true when it succeeds
extern uint8 read_float(char *line, uint8 *char_counter, float *float_ptr);

// Computes hypotenuse, avoiding avr-gcc's bloated version and the extra error checking.
extern float hypot_f(float x, float y);

extern float convert_delta_vector_to_unit_vector(float *vector);
extern float limit_value_by_axis_maximum(float *max_value, float *unit_vec);

extern float trunc_my (float x);
extern float round_my (float x);
extern int lround_my (float x);

#if (N_AXIS == 3)
	extern unsigned char isequal_position_vector(float x1, float y1, float z1, float x2, float y2, float z2);
#else
	#error TODO implement
#endif

extern float nuts_bolts_tooth_load_rpm_to_feed(float mm_rev, float RPM, float number_tooth);

#endif
