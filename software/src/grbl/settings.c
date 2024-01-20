/*
  settings.c - eeprom configuration handling
  Part of Grbl

  Copyright (c) 2019-2020 Andras Huszti
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
#include <string.h>
#include "grbl.h"

#include "k_stdtype.h"


settings_t settings;

unsigned char eep_settings_version = 0;
settings_t eep_settings;
settings_t eep_settings_backup;
float eep_coord_data[N_COORDINATE_SYSTEM_VAR][N_AXIS];
uint8 eep_coord_teached[N_COORDINATE_SYSTEM_VAR];

uint8 read_global_settings(void);

// Initialize the config subsystem
void settings_init(void) {
	memset(&settings, 0, sizeof(settings));

	//eep_settings_version = 0; //read by eeprom
	//memset(&eep_settings, 0, sizeof(eep_settings)); //read by eeprom
	//memset(&eep_settings_backup, 0, sizeof(eep_settings_backup)); //read by eeprom
	//memset(eep_coord_data, 0, sizeof(eep_coord_data)); //read by eeprom
	//memset(eep_coord_teached, 0, sizeof(eep_coord_teached)); //read by eeprom

	if(!read_global_settings()) {
		report_status_message(STATUS_SETTING_READ_FAIL, 0);
		settings_restore(SETTINGS_RESTORE_ALL); // Force restore all EEPROM data.
		report_grbl_settings();
	}
}

// Method to store coord data parameters into EEPROM
void settings_write_coord_data(uint8 coord_select, float *coord_data) {
	unsigned int x = 0;
	if (coord_select < N_COORDINATE_SYSTEM_VAR) {
		for (x = 0; x < N_AXIS; x++) {
			eep_coord_data[coord_select][x] = coord_data[x];
		}
		eep_coord_teached[coord_select] = coord_select + N_AXIS;
	}
	grbl_eeprom_trigger();
}


// Method to store Grbl global settings struct and version number into EEPROM
// NOTE: This function can only be called in IDLE state.
void write_global_settings(void) {
	eep_settings_version = SETTINGS_VERSION;
	eep_settings = settings;
	grbl_eeprom_trigger_settings();

	//eeprom_put_char(0, SETTINGS_VERSION);
	//memcpy_to_eeprom_with_checksum(EEPROM_ADDR_GLOBAL, (char*)&settings, sizeof(settings_t));
}


// Method to restore EEPROM-saved Grbl global settings back to defaults.
void settings_restore(uint8 restore_flag) {
	if (restore_flag & SETTINGS_RESTORE_DEFAULTS) {
		settings.pulse_microseconds = DEFAULT_STEP_PULSE_MICROSECONDS;
		settings.stepper_idle_lock_time = DEFAULT_STEPPER_IDLE_LOCK_TIME;
		settings.step_invert_mask = DEFAULT_STEPPING_INVERT_MASK;
		settings.dir_invert_mask = DEFAULT_DIRECTION_INVERT_MASK;
		settings.status_report_mask = DEFAULT_STATUS_REPORT_MASK;
		settings.junction_deviation = DEFAULT_JUNCTION_DEVIATION;
		settings.arc_tolerance = DEFAULT_ARC_TOLERANCE;

		settings.rpm_max = DEFAULT_SPINDLE_RPM_MAX;
		settings.rpm_min = DEFAULT_SPINDLE_RPM_MIN;

		settings.spindle_algorithm = DEFAULT_SPINDLE_ALGORITHM;
		settings.spindle_a = DEFAULT_SPINDLE_A;
		settings.spindle_b = DEFAULT_SPINDLE_B;
		settings.spindle_c = DEFAULT_SPINDLE_C;
		settings.spindle_d = DEFAULT_SPINDLE_D;

		settings.homing_dir_mask = DEFAULT_HOMING_DIR_MASK;
		settings.homing_feed_rate = DEFAULT_HOMING_FEED_RATE;
		settings.homing_seek_rate = DEFAULT_HOMING_SEEK_RATE;
		settings.homing_debounce_delay = DEFAULT_HOMING_DEBOUNCE_DELAY;
		settings.homing_pulloff = DEFAULT_HOMING_PULLOFF;

		settings.flags = 0;
		if (DEFAULT_REPORT_INCHES) { settings.flags |= BITFLAG_REPORT_INCHES; }
		if (DEFAULT_LASER_MODE) { settings.flags |= BITFLAG_LASER_MODE; }
		if (DEFAULT_EMPTY) { settings.flags |= BITFLAG_EMPTY; }
		if (DEFAULT_HARD_LIMIT_ENABLE) { settings.flags |= BITFLAG_HARD_LIMIT_ENABLE; }
		if (DEFAULT_HOMING_ENABLE) { settings.flags |= BITFLAG_HOMING_ENABLE; }
		if (DEFAULT_SOFT_LIMIT_ENABLE) { settings.flags |= BITFLAG_SOFT_LIMIT_ENABLE; }
		if (DEFAULT_INVERT_LIMIT_PINS) { settings.flags |= BITFLAG_INVERT_LIMIT_PINS; }
		if (DEFAULT_INVERT_PROBE_PIN) { settings.flags |= BITFLAG_INVERT_PROBE_PIN; }

		settings.steps_per_mm[X_AXIS] = DEFAULT_X_STEPS_PER_MM;
		settings.steps_per_mm[Y_AXIS] = DEFAULT_Y_STEPS_PER_MM;
		settings.steps_per_mm[Z_AXIS] = DEFAULT_Z_STEPS_PER_MM;
		settings.max_rate[X_AXIS] = DEFAULT_X_MAX_RATE;
		settings.max_rate[Y_AXIS] = DEFAULT_Y_MAX_RATE;
		settings.max_rate[Z_AXIS] = DEFAULT_Z_MAX_RATE;
		settings.acceleration[X_AXIS] = DEFAULT_X_ACCELERATION;
		settings.acceleration[Y_AXIS] = DEFAULT_Y_ACCELERATION;
		settings.acceleration[Z_AXIS] = DEFAULT_Z_ACCELERATION;
		settings.max_travel[X_AXIS] = (-DEFAULT_X_MAX_TRAVEL);
		settings.max_travel[Y_AXIS] = (-DEFAULT_Y_MAX_TRAVEL);
		settings.max_travel[Z_AXIS] = (-DEFAULT_Z_MAX_TRAVEL);

		settings.grbl_parameter_automatic_tool_change = DEFAULT_AUTOMATIC_TOOL_CHANGE;
		
		write_global_settings();
	}

	if (restore_flag & SETTINGS_RESTORE_PARAMETERS) {
		uint8 idx;
		float coord_data[N_AXIS];
		memset(&coord_data, 0, sizeof(coord_data));
		for (idx=0; idx <= SETTING_INDEX_NCOORD; idx++) {
			settings_write_coord_data(idx, coord_data);
		}
	}

	grbl_eeprom_trigger();
}


// Read selected coordinate data from EEPROM. Updates pointed coord_data value.
uint8 settings_read_coord_data(uint8 coord_select, float *coord_data) {
	uint8 result = true;
	unsigned int x = 0;
	if (coord_select < N_COORDINATE_SYSTEM_VAR) {
		for (x = 0; x < N_AXIS; x++) {
			coord_data[x] = eep_coord_data[coord_select][x];
		}
		if (eep_coord_teached[coord_select] == (coord_select + N_AXIS) ) {
			result = true;
		}
	} else {
		for (x = 0; x < N_AXIS; x++) {
			coord_data[x] = 0.0f;
		}
	}
	return result;
}


// Reads Grbl global settings struct from EEPROM.
uint8 read_global_settings(void) {
	uint8 result = false;
	// Check version-byte of eeprom
	if (eep_settings_version == SETTINGS_VERSION) {
		// Read settings-record and check checksum
		settings = eep_settings;
		result = true;
	}
	return result;
}


// A helper method to set settings from command line
uint8 settings_store_global_setting(uint8 parameter, float value) {
	if (
		(value < 0.0) && 
		((parameter != 35) ||(parameter != 36) ||(parameter != 37) ||(parameter != 38))
	) { 
		return(STATUS_NEGATIVE_VALUE); 
	}
	if (parameter >= AXIS_SETTINGS_START_VAL) {
		// Store axis configuration. Axis numbering sequence set by AXIS_SETTING defines.
		// NOTE: Ensure the setting index corresponds to the report.c settings printout.
		parameter -= AXIS_SETTINGS_START_VAL;
		uint8 set_idx = 0;
		while (set_idx < AXIS_N_SETTINGS) {
			if (parameter < N_AXIS) {
				// Valid axis setting found.
				switch (set_idx) {
					case 0:
						#ifdef MAX_STEP_RATE_HZ
							if (value*settings.max_rate[parameter] > (MAX_STEP_RATE_HZ*60.0)) {
								return(STATUS_MAX_STEP_RATE_EXCEEDED);
							}
						#endif
						settings.steps_per_mm[parameter] = value;
						break;
					case 1:
						#ifdef MAX_STEP_RATE_HZ
							if (value*settings.steps_per_mm[parameter] > (MAX_STEP_RATE_HZ*60.0)) {
								return(STATUS_MAX_STEP_RATE_EXCEEDED); 
							}
						#endif
						settings.max_rate[parameter] = value;
						break;
					case 2: 
						settings.acceleration[parameter] = value*60*60; 
						break; // Convert to mm/min^2 for grbl internal use.
					case 3: 
						settings.max_travel[parameter] = -value; 
						break;  // Store as negative for grbl internal use.
				}
				break; // Exit while-loop after setting has been configured and proceed to the EEPROM write call.
			} else {
				set_idx++;
				// If axis index greater than N_AXIS or setting index greater than number of axis settings, error out.
				if ((parameter < AXIS_SETTINGS_INCREMENT) || (set_idx == AXIS_N_SETTINGS)) {
					return(STATUS_INVALID_STATEMENT); 
				}
				parameter -= AXIS_SETTINGS_INCREMENT;
			}
		}
	} else {
		// Store non-axis Grbl settings
		uint8 int_value = trunc_my(value);
		switch(parameter) {
			case 0:
				if (int_value < 3) {
					return(STATUS_SETTING_STEP_PULSE_MIN); 
				}
				settings.pulse_microseconds = int_value; 
				break;
			case 1: 
				settings.stepper_idle_lock_time = int_value; 
				break;
			case 2:
				settings.step_invert_mask = int_value;
				st_generate_step_dir_invert_masks(); // Regenerate step and direction port invert masks.
				break;
			case 3:
				settings.dir_invert_mask = int_value;
				st_generate_step_dir_invert_masks(); // Regenerate step and direction port invert masks.
				break;
			case 4: // Reset to ensure change. Immediate re-init may cause problems.
				if (int_value) { 
					settings.flags |= BITFLAG_EMPTY; 
				} else { 
					settings.flags &= ~BITFLAG_EMPTY; 
				}
				break;
			case 5: // Reset to ensure change. Immediate re-init may cause problems.
				if (int_value) {
					settings.flags |= BITFLAG_INVERT_LIMIT_PINS;
				} else { 
					settings.flags &= ~BITFLAG_INVERT_LIMIT_PINS;
				}
				break;
			case 6: // Reset to ensure change. Immediate re-init may cause problems.
				if (int_value) { 
					settings.flags |= BITFLAG_INVERT_PROBE_PIN; 
				} else { 
					settings.flags &= ~BITFLAG_INVERT_PROBE_PIN; 
				}
				probe_configure_invert_mask(false);
				break;
			case 10: 
				settings.status_report_mask = int_value; 
				break;
			case 11: 
				settings.junction_deviation = value; 
				break;
			case 12: 
				settings.arc_tolerance = value; 
				break;
			case 13:
				if (int_value) { 
					settings.flags |= BITFLAG_REPORT_INCHES; 
				} else { 
					settings.flags &= ~BITFLAG_REPORT_INCHES; 
				}
				system_flag_wco_change(); // Make sure WCO is immediately updated.
				break;
			case 20:
				if (int_value) {
					if (bit_isfalse(settings.flags, BITFLAG_HOMING_ENABLE)) {
						return(STATUS_SOFT_LIMIT_ERROR); 
					}
					settings.flags |= BITFLAG_SOFT_LIMIT_ENABLE;
				} else { 
					settings.flags &= ~BITFLAG_SOFT_LIMIT_ENABLE; 
				}
				break;
			case 21:
				if (int_value) { 
					settings.flags |= BITFLAG_HARD_LIMIT_ENABLE; 
				} else { 
					settings.flags &= ~BITFLAG_HARD_LIMIT_ENABLE; 
				}
				limits_enable(); // Re-init to immediately change. NOTE: Nice to have but could be problematic later.
				break;
			case 22:
				if (int_value) { 
					settings.flags |= BITFLAG_HOMING_ENABLE;
				} else {
					settings.flags &= ~BITFLAG_HOMING_ENABLE;
					settings.flags &= ~BITFLAG_SOFT_LIMIT_ENABLE; // Force disable soft-limits.
				}
				break;
			case 23: 
				settings.homing_dir_mask = int_value; 
				break;
			case 24: 
				settings.homing_feed_rate = value; 
				break;
			case 25: 
				settings.homing_seek_rate = value;
				break;
			case 26: 
				settings.homing_debounce_delay = int_value;
				break;
			case 27: 
				settings.homing_pulloff = value; 
				break;
			case 30: 
				settings.rpm_max = value; spindle_init(); 
				break; // Re-initialize spindle rpm calibration
			case 31: 
				settings.rpm_min = value; spindle_init(); 
				break; // Re-initialize spindle rpm calibration
			case 32:
				if (int_value) {
					settings.flags |= BITFLAG_LASER_MODE; 
				} else { 
					settings.flags &= ~BITFLAG_LASER_MODE;
				}
				break;
			case 33: 
				settings.grbl_parameter_automatic_tool_change = value; 
				break;
			case 34: 
				settings.spindle_algorithm = value; spindle_init(); 
				break; // Re-initialize spindle rpm calibration
			case 35: 
				settings.spindle_a = value; spindle_init(); 
				break; // Re-initialize spindle rpm calibration
			case 36: 
				settings.spindle_b = value; spindle_init(); 
				break; // Re-initialize spindle rpm calibration
			case 37: 
				settings.spindle_c = value; spindle_init(); 
				break; // Re-initialize spindle rpm calibration
			case 38: 
				settings.spindle_d = value; spindle_init(); 
				break; // Re-initialize spindle rpm calibration
			default:
				return(STATUS_INVALID_STATEMENT);
		}
	}
	write_global_settings();
	return(STATUS_OK);
}

// Returns step pin mask according to Grbl internal axis indexing.
uint8 get_step_pin_mask(uint8 axis_idx) {
	if ( axis_idx == X_AXIS ) { 
		return((1<<X_STEP_BIT)); 
	}
	if ( axis_idx == Y_AXIS ) { 
		return((1<<Y_STEP_BIT));
	}
	return((1<<Z_STEP_BIT));
}


// Returns direction pin mask according to Grbl internal axis indexing.
uint8 get_direction_pin_mask(uint8 axis_idx) {
	if ( axis_idx == X_AXIS ) { 
		return((1<<X_DIRECTION_BIT)); 
	}
	if ( axis_idx == Y_AXIS ) { 
		return((1<<Y_DIRECTION_BIT)); 
	}
	return((1<<Z_DIRECTION_BIT));
}
