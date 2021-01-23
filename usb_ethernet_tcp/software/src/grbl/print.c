/*
  print.c - Functions for formatting output strings
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
#include "k_stdtype.h"
#include "grbl.h"


void printString(const char *s) {
	while (*s) {
		putChar_grbl(*s++);
	}
}


// Print a string stored in PGM-memory
void printPgmString(const char *s) {
	while (*s) {
		putChar_grbl(*s++);
	}
}

// Prints an uint8 variable in base 10.
void print_uint8_base10(uint8 n)
{
  uint8 digit_a = 0;
  uint8 digit_b = 0;
  if (n >= 100) { // 100-255
    digit_a = '0' + n % 10;
    n /= 10;
  }
  if (n >= 10) { // 10-99
    digit_b = '0' + n % 10;
    n /= 10;
  }
  putChar_grbl('0' + n);
  if (digit_b) { putChar_grbl(digit_b); }
  if (digit_a) { putChar_grbl(digit_a); }
}


// Prints an uint8 variable in base 2 with desired number of desired digits.
void print_uint8_base2_ndigit(uint8 n, uint8 digits) {
  unsigned char buf[digits];
  uint8 i = 0;

  for (; i < digits; i++) {
      buf[i] = n % 2 ;
      n /= 2;
  }

  for (; i > 0; i--)
      putChar_grbl('0' + buf[i - 1]);
}


void print_uint32_base10(uint32 n)
{
  if (n == 0) {
    putChar_grbl('0');
    return;
  }

  unsigned char buf[10];
  uint8 i = 0;

  while (n > 0) {
    buf[i++] = n % 10;
    n /= 10;
  }

  for (; i > 0; i--)
    putChar_grbl('0' + buf[i-1]);
}

void print_uint32_base16(uint32 n) {
	uint8 x = 0;
	uint32 part = 0;
	for (x = 0; x < 8; x++) {
		part = (n >> (( 7 - x) * 4)) & 0xF;
		
		if ((part >= 0) && (part <= 9)) {
			putChar_grbl('0' + part);
		} else {
			putChar_grbl('A' + (part - 0xA));
		}
	}
}

void print_uint16_base16(uint16 n) {
	uint8 x = 0;
	uint32 part = 0;
	for (x = 0; x < 4; x++) {
		part = (n >> (( 3 - x) * 4)) & 0xF;
		
		if ((part >= 0) && (part <= 9)) {
			putChar_grbl('0' + part);
		} else {
			putChar_grbl('A' + part - 0xA);
		}
	}
}

void print_uint8_base16(uint8 n) {
	uint8 x = 0;
	uint32 part = 0;
	for (x = 0; x < 2; x++) {
		part = (n >> (( 1 - x) * 4)) & 0xF;
		
		if ((part >= 0) && (part <= 9)) {
			putChar_grbl('0' + part - 0x00);
		} else {
			putChar_grbl('A' + part - 0x0A);
		}
	}
}

void printInteger(long n)
{
  if (n < 0) {
    putChar_grbl('-');
    print_uint32_base10(-n);
  } else {
    print_uint32_base10(n);
  }
}


// Convert float to string by immediately converting to a long integer, which contains
// more digits than a float. Number of decimal places, which are tracked by a counter,
// may be set by the user. The integer is then efficiently converted to a string.
// NOTE: AVR '%' and '/' integer operations are very efficient. Bitshifting speed-up
// techniques are actually just slightly slower. Found this out the hard way.
void printFloat(float n, uint8 decimal_places)
{
  if (n < 0) {
    putChar_grbl('-');
    n = -n;
  }

  uint8 decimals = decimal_places;
  while (decimals >= 2) { // Quickly convert values expected to be E0 to E-4.
    n *= 100;
    decimals -= 2;
  }
  if (decimals) { n *= 10; }
  n += 0.5; // Add rounding factor. Ensures carryover through entire value.

  // Generate digits backwards and store in string.
  unsigned char buf[13];
  uint8 i = 0;
  uint32 a = (long)n;
  while(a > 0) {
    buf[i++] = (a % 10) + '0'; // Get digit
    a /= 10;
  }
  while (i < decimal_places) {
     buf[i++] = '0'; // Fill in zeros to decimal point for (n < 1)
  }
  if (i == decimal_places) { // Fill in leading zero, if needed.
    buf[i++] = '0';
  }

  // Print the generated string.
  for (; i > 0; i--) {
    if (i == decimal_places) { putChar_grbl('.'); } // Insert decimal point in right place.
    putChar_grbl(buf[i-1]);
  }
}


// Floating value printing handlers for special variables types used in Grbl and are defined
// in the config.h.
//  - CoordValue: Handles all position or coordinate values in inches or mm reporting.
//  - RateValue: Handles feed rate and current velocity in inches or mm reporting.
void printFloat_CoordValue(float n) {
  if (bit_istrue(settings.flags,BITFLAG_REPORT_INCHES)) {
    printFloat(n*INCH_PER_MM,N_DECIMAL_COORDVALUE_INCH);
  } else {
    printFloat(n,N_DECIMAL_COORDVALUE_MM);
  }
}

void printFloat_RateValue(float n) {
  if (bit_istrue(settings.flags,BITFLAG_REPORT_INCHES)) {
    printFloat(n*INCH_PER_MM,N_DECIMAL_RATEVALUE_INCH);
  } else {
    printFloat(n,N_DECIMAL_RATEVALUE_MM);
  }
}

// Debug tool to print free memory in bytes at the called point.
// NOTE: Keep commented unless using. Part of this function always gets compiled in.
// void printFreeMemory()
// {
//   extern int __heap_start, *__brkval;
//   uint16 free;  // Up to 64k values.
//   free = (int) &free - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
//   printInteger((int32_t)free);
//   printString(" ");
// }
