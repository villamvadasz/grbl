/*
  grbl_report.h - reporting and messaging methods
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
#ifndef report_h
#define report_h

	// Define Grbl status codes. Valid values (0-255)
	#define STATUS_ASYNCH_OK -1
	#define STATUS_OK 0
	#define STATUS_EXPECTED_COMMAND_LETTER 1
	#define STATUS_BAD_NUMBER_FORMAT 2
	#define STATUS_INVALID_STATEMENT 3
	#define STATUS_NEGATIVE_VALUE 4
	#define STATUS_SETTING_DISABLED 5
	#define STATUS_SETTING_STEP_PULSE_MIN 6
	#define STATUS_SETTING_READ_FAIL 7
	#define STATUS_IDLE_ERROR 8
	#define STATUS_SYSTEM_GC_LOCK 9
	#define STATUS_SOFT_LIMIT_ERROR 10
	#define STATUS_OVERFLOW 11
	#define STATUS_MAX_STEP_RATE_EXCEEDED 12
	#define STATUS_CHECK_DOOR 13
	#define STATUS_LINE_LENGTH_EXCEEDED 14
	#define STATUS_TRAVEL_EXCEEDED 15
	#define STATUS_INVALID_JOG_COMMAND 16

	#define STATUS_GCODE_UNSUPPORTED_COMMAND 20
	#define STATUS_GCODE_MODAL_GROUP_VIOLATION 21
	#define STATUS_GCODE_UNDEFINED_FEED_RATE 22
	#define STATUS_GCODE_COMMAND_VALUE_NOT_INTEGER 23
	#define STATUS_GCODE_AXIS_COMMAND_CONFLICT 24
	#define STATUS_GCODE_WORD_REPEATED 25
	#define STATUS_GCODE_NO_AXIS_WORDS 26
	#define STATUS_GCODE_INVALID_LINE_NUMBER 27
	#define STATUS_GCODE_VALUE_WORD_MISSING 28
	#define STATUS_GCODE_UNSUPPORTED_COORD_SYS 29
	#define STATUS_GCODE_G53_INVALID_MOTION_MODE 30
	#define STATUS_GCODE_AXIS_WORDS_EXIST 31
	#define STATUS_GCODE_NO_AXIS_WORDS_IN_PLANE 32
	#define STATUS_GCODE_INVALID_TARGET 33
	#define STATUS_GCODE_ARC_RADIUS_ERROR 34
	#define STATUS_GCODE_NO_OFFSETS_IN_PLANE 35
	#define STATUS_GCODE_UNUSED_WORDS 36
	#define STATUS_GCODE_G43_DYNAMIC_AXIS_ERROR 37
	#define STATUS_GCODE_MAX_VALUE_EXCEEDED 38

	// Define Grbl feedback message codes. Valid values (0-255).
	#define MESSAGE_CRITICAL_EVENT 1
	#define MESSAGE_ALARM_LOCK 2
	#define MESSAGE_ALARM_UNLOCK 3
	#define MESSAGE_ENABLED 4
	#define MESSAGE_DISABLED 5
	#define MESSAGE_SAFETY_DOOR_AJAR 6
	#define MESSAGE_CHECK_LIMITS 7
	#define MESSAGE_PROGRAM_END 8
	#define MESSAGE_RESTORE_DEFAULTS 9
	#define MESSAGE_SPINDLE_RESTORE 10
	#define MESSAGE_SLEEP_MODE 11
	#define MESSAGE_EEP_MASTER_ERROR 12
	#define MESSAGE_EEP_BACKUP_ERROR 13
	#define MESSAGE_ABORT_CYCLE 14
	#define MESSAGE_ALARM_UNLOCK_FAILED_EMERGENCYSTOP 15
	#define MESSAGE_ALARM_UNLOCK_FAILED_POWER 16
	#define MESSAGE_ALARM_EXTERNAL_POWER_OFF 17


	extern void init_grbl_report(void);
	extern void do_grbl_report(void);
	extern void isr_grbl_report_1ms(void);


	extern void grbl_report_dump_eeprom(void);

	// Prints system status messages.
	extern void report_status_message(uint8 status_code, int32_t line_number);

	// Prints system alarm messages.
	extern void report_alarm_message(uint8 alarm_code);

	// Prints miscellaneous feedback messages.
	extern void report_feedback_message(uint8 message_code);

	// Prints welcome message
	extern void report_init_message(void);

	// Prints Grbl help and current global settings
	extern void report_grbl_help(void);
	extern void report_grbl_debug_0(void);
	extern void report_grbl_debug_1(void);
	extern void report_grbl_crypto_0(void);//public
	extern void report_grbl_crypto_1(void);//private
	extern void report_grbl_crypto_2(void);//shared key

	// Prints Grbl global settings
	extern void report_grbl_settings(void);

	// Prints an echo of the pre-parsed line received right before execution.
	extern void report_echo_line_received(char *line);

	// Prints realtime status report
	extern void report_realtime_status(void);

	// Prints recorded probe position
	extern void report_probe_parameters(void);

	// Prints Grbl NGC parameters (coordinate offsets, probe)
	extern void report_ngc_parameters(void);

	// Prints current g-code parser mode state
	extern void report_gcode_modes(void);

	// Prints build info and user info
	extern void report_build_info(void);

	extern void report_stack_usage(void);

#endif
