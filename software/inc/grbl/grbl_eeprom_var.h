#ifndef _GRBL_EEPROM_VAR_
#define _GRBL_EEPROM_VAR_

	extern unsigned char eep_settings_version;
	extern settings_t eep_settings;
	extern settings_t eep_settings_backup;
	extern float eep_coord_data[N_COORDINATE_SYSTEM_VAR][N_AXIS];
							
	extern int32_t sys_position[N_AXIS];      // Real-time machine (aka home) position vector in steps.
	extern float coord_system_eep[N_AXIS];
	extern float coord_offset_eep[N_AXIS];
	extern uint32 grbl_running_running_minutes;
	extern uint32 grbl_running_running_hours;
	extern uint32 grbl_running_startup_cnt;
	extern uint32 grbl_running_spindle_running_minutes;

	extern float grbl_Tool_Length_1;
	extern float grbl_Tool_Length_2;

	extern uint8 eep_coord_teached[N_COORDINATE_SYSTEM_VAR];

#endif
				