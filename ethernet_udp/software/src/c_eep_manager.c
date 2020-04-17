#include "eep_manager.h"
#include "c_eep_manager.h"

#include "grbl_eep_export.h"
#include "TCPIP_ENC28_PIC32_OLIMEX.h"
#include "ethernet.h"

const unsigned char default_eep_settings_version = SETTINGS_VERSION;
const settings_t default_eep_settings = {
  // Axis settings
    {(float)DEFAULT_X_STEPS_PER_MM, (float)DEFAULT_Y_STEPS_PER_MM, (float)DEFAULT_Z_STEPS_PER_MM},
    {(float)DEFAULT_X_MAX_RATE, (float)DEFAULT_Y_MAX_RATE, (float)DEFAULT_Z_MAX_RATE},
    {(float)DEFAULT_X_ACCELERATION, (float)DEFAULT_Y_ACCELERATION, (float)DEFAULT_Z_ACCELERATION},
    {(float)(-DEFAULT_X_MAX_TRAVEL), (float)(-DEFAULT_Y_MAX_TRAVEL), (float)(-DEFAULT_Z_MAX_TRAVEL)},

  // Remaining Grbl settings
  (uint8)DEFAULT_STEP_PULSE_MICROSECONDS,
  (uint8)DEFAULT_STEPPING_INVERT_MASK,
  (uint8)DEFAULT_DIRECTION_INVERT_MASK,
  (uint8)DEFAULT_STEPPER_IDLE_LOCK_TIME,
  (uint8)DEFAULT_STATUS_REPORT_MASK,
  (float)DEFAULT_JUNCTION_DEVIATION,
  (float)DEFAULT_ARC_TOLERANCE,

  (float)DEFAULT_SPINDLE_RPM_MAX,
  (float)DEFAULT_SPINDLE_RPM_MIN,

  (uint8)( 
    (DEFAULT_REPORT_INCHES		<< 0) |
    (DEFAULT_LASER_MODE			<< 1) |
//    (DEFAULT_EMPTY			<< 2) |
    (DEFAULT_HARD_LIMIT_ENABLE	<< 3) |
    (DEFAULT_HOMING_ENABLE		<< 4) |
    (DEFAULT_SOFT_LIMIT_ENABLE	<< 5) |
    (DEFAULT_INVERT_LIMIT_PINS	<< 6) |
    (DEFAULT_INVERT_PROBE_PIN	<< 7)
  ),

  (uint8)DEFAULT_HOMING_DIR_MASK,
  (float)DEFAULT_HOMING_FEED_RATE,
  (float)DEFAULT_HOMING_SEEK_RATE,
  (uint16)DEFAULT_HOMING_DEBOUNCE_DELAY,
  (float)DEFAULT_HOMING_PULLOFF,
  (uint8)DEFAULT_AUTOMATIC_TOOL_CHANGE
};
const unsigned char default_eep_build_info[LINE_BUFFER_SIZE] = {0};
const float default_eep_coord_data[N_COORDINATE_SYSTEM_VAR][N_AXIS] = { {0.0f, 0.0f, 0.0f}, //G54
																		{0.0f, 0.0f, 0.0f}, //G55
																		{0.0f, 0.0f, 0.0f}, //G56 
																		{0.0f, 0.0f, 0.0f}, //G57
																		{0.0f, 0.0f, 0.0f}, //G58
																		{0.0f, 0.0f, 0.0f}, //G59
																		{-100.0, -150.0, 50.0}, //G28
																		{0.0f, 0.0f, 0.0f}, //G30
																		{0.0f, 0.0f, 0.0f}, //Reserve 
																		{0.0f, 0.0f, 0.0f}, //Reserve
																	};
const int32_t default_sys_position[N_AXIS] = {0.0f, 0.0f, 0.0f};
const float default_coord_system_eep[N_AXIS] = {0.0f, 0.0f, 0.0f};
const float default_coord_offset_eep[N_AXIS] = {0.0f, 0.0f, 0.0f};
const uint32 default_grbl_running_running_minutes = 0;
const uint32 default_grbl_running_running_hours = 0;
const uint32 default_grbl_running_startup_cnt = 0;
const uint32 default_grbl_running_spindle_running_minutes = 0;
const unsigned char default_SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};

EepManager_ItemTable eepManager_ItemTable[EepManager_Items_LastItem] = {
	{EepManager_Items_GRBL_0, 				sizeof(eep_settings_version),					&eep_settings_version,					(void *)&default_eep_settings_version,					EEP_USE_CRC,	EEP_HAVE_BACKUP},
	{EepManager_Items_GRBL_Settings,		sizeof(eep_settings),							&eep_settings, 							(void *)&default_eep_settings,							EEP_USE_CRC,	EEP_NO_BACKUP},
	{EepManager_Items_GRBL_Settings_Backup,	sizeof(eep_settings),							&eep_settings_backup,					(void *)&default_eep_settings,							EEP_USE_CRC,	EEP_NO_BACKUP},
	{EepManager_Items_GRBL_3, 				sizeof(eep_build_info),							eep_build_info, 						(void *)default_eep_build_info,							EEP_USE_SUM,	EEP_NO_BACKUP},
	{EepManager_Items_GRBL_4, 				sizeof(eep_coord_data),							eep_coord_data, 						(void *)default_eep_coord_data,							EEP_USE_CRC,	EEP_HAVE_BACKUP},
	{EepManager_Items_GRBL_5, 				sizeof(sys_position),							&sys_position, 							(void *)&sys_position,									EEP_USE_CRC,	EEP_HAVE_BACKUP},
	{EepManager_Items_GRBL_6, 				sizeof(coord_system_eep),						coord_system_eep, 						(void *)default_coord_system_eep,						EEP_USE_CRC,	EEP_HAVE_BACKUP},
	{EepManager_Items_GRBL_7, 				sizeof(coord_offset_eep),						coord_offset_eep, 						(void *)default_coord_offset_eep,						EEP_USE_CRC,	EEP_HAVE_BACKUP},
	{EepManager_Items_RunningMinutes,		sizeof(grbl_running_running_minutes),			&grbl_running_running_minutes, 			(void *)&default_grbl_running_running_minutes,			EEP_USE_CRC,	EEP_HAVE_BACKUP},
	{EepManager_Items_RunningHours,			sizeof(grbl_running_running_hours),				&grbl_running_running_hours, 			(void *)&default_grbl_running_running_hours,			EEP_USE_CRC,	EEP_HAVE_BACKUP},
	{EepManager_Items_StartUpCnt,			sizeof(grbl_running_startup_cnt),				&grbl_running_startup_cnt, 				(void *)&default_grbl_running_startup_cnt,				EEP_USE_CRC,	EEP_HAVE_BACKUP},
	{EepManager_Items_SpindleRunningMinutes,sizeof(grbl_running_spindle_running_minutes),	&grbl_running_spindle_running_minutes,	(void *)&default_grbl_running_spindle_running_minutes,	EEP_USE_CRC,	EEP_HAVE_BACKUP},
	{EepManager_Items_MAC_Address,			sizeof(SerializedMACAddress),					&SerializedMACAddress,					(void *)&default_SerializedMACAddress,					EEP_USE_CRC,	EEP_HAVE_BACKUP},
};
