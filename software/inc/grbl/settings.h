/*
  settings.h - eeprom configuration handling
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

#ifndef settings_h
#define settings_h

#include "nuts_bolts.h"
#include "grbl_protocol.h"

// Version of the EEPROM data. Will be used to migrate existing data from older versions of Grbl
// when firmware is upgraded. Always stored in byte 0 of eeprom
#define SETTINGS_VERSION 11  // NOTE: Check settings_reset() when moving to next version.

// Define bit flag masks for the boolean settings in settings.flag.
#define BITFLAG_REPORT_INCHES      bit(0)
#define BITFLAG_LASER_MODE         bit(1)
#define BITFLAG_EMPTY			   bit(2)
#define BITFLAG_HARD_LIMIT_ENABLE  bit(3)
#define BITFLAG_HOMING_ENABLE      bit(4)
#define BITFLAG_SOFT_LIMIT_ENABLE  bit(5)
#define BITFLAG_INVERT_LIMIT_PINS  bit(6)
#define BITFLAG_INVERT_PROBE_PIN   bit(7)

// Define status reporting boolean enable bit flags in settings.status_report_mask
#define BITFLAG_RT_STATUS_POSITION_TYPE     bit(0)

// Define settings restore bitflags.
#define SETTINGS_RESTORE_DEFAULTS bit(0)
#define SETTINGS_RESTORE_PARAMETERS bit(1)
#ifndef SETTINGS_RESTORE_ALL
  #define SETTINGS_RESTORE_ALL 0xFF // All bitflags
#endif

// Define EEPROM address indexing for coordinate parameters
#define N_COORDINATE_SYSTEM 6  // Number of supported work coordinate systems (from index 1)
#define SETTING_INDEX_NCOORD N_COORDINATE_SYSTEM+1 // Total number of system stored (from index 0)
// NOTE: Work coordinate indices are (0=G54, 1=G55, ... , 6=G59)
#define SETTING_INDEX_G28    N_COORDINATE_SYSTEM    // Home position 1
#define SETTING_INDEX_G30    N_COORDINATE_SYSTEM+1  // Home position 2
// #define SETTING_INDEX_G92    N_COORDINATE_SYSTEM+2  // Coordinate offset (G92.2,G92.3 not supported)
#define N_COORDINATE_SYSTEM_VAR    N_COORDINATE_SYSTEM+2+2 //+2 is reserve

// Define Grbl axis settings numbering scheme. Starts at START_VAL, every INCREMENT, over N_SETTINGS.
#define AXIS_N_SETTINGS          4
#define AXIS_SETTINGS_START_VAL  100 // NOTE: Reserving settings values >= 100 for axis settings. Up to 255.
#define AXIS_SETTINGS_INCREMENT  10  // Must be greater than the number of axis settings

// Global persistent settings (Stored from byte EEPROM_ADDR_GLOBAL onwards)
typedef struct _settings_t {
  // Axis settings
  float steps_per_mm[N_AXIS];
  float max_rate[N_AXIS];
  float acceleration[N_AXIS];
  float max_travel[N_AXIS];

  // Remaining Grbl settings
  uint8 pulse_microseconds;
  uint8 step_invert_mask;
  uint8 dir_invert_mask;
  uint8 stepper_idle_lock_time; // If max value 255, steppers do not disable.
  uint8 status_report_mask; // Mask to indicate desired report data.
  float junction_deviation;
  float arc_tolerance;

  float rpm_max;
  float rpm_min;

  uint8 flags;  // Contains default boolean settings

  uint8 homing_dir_mask;
  float homing_feed_rate;
  float homing_seek_rate;
  uint16 homing_debounce_delay;
  float homing_pulloff;
  uint8 grbl_parameter_automatic_tool_change;

  uint8 spindle_algorithm;
  float spindle_a;
  float spindle_b;
  float spindle_c;
  float spindle_d;

} settings_t;
extern settings_t settings;

extern unsigned char eep_settings_version;
extern settings_t eep_settings;
extern settings_t eep_settings_backup;
extern float eep_coord_data[N_COORDINATE_SYSTEM_VAR][N_AXIS];
extern uint8 eep_coord_teached[N_COORDINATE_SYSTEM_VAR];

// Initialize the configuration subsystem (load settings from EEPROM)
void settings_init();

// Helper function to clear and restore EEPROM defaults
void settings_restore(uint8 restore_flag);

// A helper method to set new settings from command line
uint8 settings_store_global_setting(uint8 parameter, float value);

// Writes selected coordinate data to EEPROM
void settings_write_coord_data(uint8 coord_select, float *coord_data);

// Reads selected coordinate data from EEPROM
uint8 settings_read_coord_data(uint8 coord_select, float *coord_data);

// Returns the step pin mask according to Grbl's internal axis numbering
uint8 get_step_pin_mask(uint8 i);

// Returns the direction pin mask according to Grbl's internal axis numbering
uint8 get_direction_pin_mask(uint8 i);


#endif
