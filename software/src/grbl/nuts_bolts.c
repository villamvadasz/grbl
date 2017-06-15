/*
  nuts_bolts.c - Shared functions
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
#include <math.h>
#include "grbl.h"

#include "k_stdtype.h"
#include "tmr.h"

#define MAX_INT_DIGITS 8 // Maximum number of digits in int32 (and float)

uint8 grbl_not_existing_tris = 0;
uint8 grbl_not_existing_port = 0;
uint8 grbl_not_existing_lat = 0;

void init_nuts_bolts(void) {
}

void do_nuts_bolts(void) {
}

void isr_nuts_bolts_1ms(void) {
}


// Extracts a floating point value from a string. The following code is based loosely on
// the avr-libc strtod() function by Michael Stumpf and Dmitry Xmelkov and many freely
// available conversion method examples, but has been highly optimized for Grbl. For known
// CNC applications, the typical decimal value is expected to be in the range of E0 to E-4.
// Scientific notation is officially not supported by g-code, and the 'E' character may
// be a g-code word on some CNC systems. So, 'E' notation will not be recognized.
// NOTE: Thanks to Radu-Eosif Mihailescu for identifying the issues with using strtod().
uint8 read_float(char *line, uint8 *char_counter, float *float_ptr)
{
  char *ptr = line + *char_counter;
  unsigned char c;

  // Grab first character and increment pointer. No spaces assumed in line.
  c = *ptr++;

  // Capture initial positive/minus character
  bool isnegative = false;
  if (c == '-') {
    isnegative = true;
    c = *ptr++;
  } else if (c == '+') {
    c = *ptr++;
  }

  // Extract number into fast integer. Track decimal in terms of exponent value.
  uint32 intval = 0;
  int8_t exp = 0;
  uint8 ndigit = 0;
  bool isdecimal = false;
  while(1) {
    c -= '0';
    if (c <= 9) {
      ndigit++;
      if (ndigit <= MAX_INT_DIGITS) {
        if (isdecimal) { exp--; }
        intval = (((intval << 2) + intval) << 1) + c; // intval*10 + c
      } else {
        if (!(isdecimal)) { exp++; }  // Drop overflow digits
      }
    } else if (c == (('.'-'0') & 0xff)  &&  !(isdecimal)) {
      isdecimal = true;
    } else {
      break;
    }
    c = *ptr++;
  }

  // Return if no digits have been read.
  if (!ndigit) { return(false); };

  // Convert integer into floating point.
  float fval;
  fval = (float)intval;

  // Apply decimal. Should perform no more than two floating point multiplications for the
  // expected range of E0 to E-4.
  if (fval != 0) {
    while (exp <= -2) {
      fval *= 0.01;
      exp += 2;
    }
    if (exp < 0) {
      fval *= 0.1;
    } else if (exp > 0) {
      do {
        fval *= 10.0;
      } while (--exp > 0);
    }
  }

  // Assign floating point value with correct sign.
  if (isnegative) {
    *float_ptr = -fval;
  } else {
    *float_ptr = fval;
  }

  *char_counter = ptr - line - 1; // Set char_counter to next statement

  return(true);
}

// Simple hypotenuse computation function.
float hypot_f(float x, float y) {
    return(sqrt(x*x + y*y));
}


float convert_delta_vector_to_unit_vector(float *vector)
{
  uint8 idx;
  float magnitude = 0.0;
  for (idx=0; idx<N_AXIS; idx++) {
    if (vector[idx] != 0.0) {
      magnitude += vector[idx]*vector[idx];
    }
  }
  magnitude = sqrt(magnitude);
  float inv_magnitude = 1.0/magnitude;
  for (idx=0; idx<N_AXIS; idx++) { vector[idx] *= inv_magnitude; }
  return(magnitude);
}


float limit_value_by_axis_maximum(float *max_value, float *unit_vec)
{
  uint8 idx;
  float limit_value = SOME_LARGE_VALUE;
  for (idx=0; idx<N_AXIS; idx++) {
    if (unit_vec[idx] != 0) {  // Avoid divide by zero.
      limit_value = min(limit_value,fabs(max_value[idx]/unit_vec[idx]));
    }
  }
  return(limit_value);
}

float trunc_my (float x) {
	return (x>0) ? floor(x) : ceil(x);
}

float round_my (float x) {
	return (float)((int)(x + 0.5f));
}

int lround_my (float x) {
	int result = 0;
	if (x > 0.0f) {
		result = (int)(x + 0.5f);
	} else if (x < 0.0f) {
		result = (int)(x - 0.5f);
	} else {
		result = 0;
	}
	return result;
}

#if (N_AXIS == 3)
	unsigned char isequal_position_vector(float x1, float y1, float z1, float x2, float y2, float z2) {
		unsigned char result = 0;
		float diffX = x1 - x2;
		float diffY = y1 - y2;
		float diffZ = z1 - z2;

		if ((fabs(diffX) < 0.001) && (fabs(diffY) < 0.001) && (fabs(diffZ) < 0.001))
		{
			result = 1;
		}
		return result;
	}
#else
	unsigned char isequal_position_vector(float x1, float y1, float z1, float a1, float x2, float y2, float z2, float a2) {
		unsigned char result = 0;
		float diffX = x1 - x2;
		float diffY = y1 - y2;
		float diffZ = z1 - z2;
		float diffA = a1 - a2;

		if ((fabs(diffX) < 0.001) && (fabs(diffY) < 0.001) && (fabs(diffY) < 0.001) && (fabs(diffA) < 0.001))
		{
			result = 1;
		}
		return result;
	}
#endif

float nuts_bolts_tooth_load_rpm_to_feed(float mm_rev, float RPM, float number_tooth) {
	float result = 0.0f;
	if ((mm_rev >= 0.0f) && (RPM >= 0.0f) && (number_tooth >= 1.0f)) {
		RPM *= (0.010f * sys.spindle_speed_ovr);
		result = mm_rev;
		result *= RPM;
		result *= number_tooth;
	} else {
		result = 100.0f;//default answer for wrong data to keep SW running.
	}
	return result;//mm/min
}
