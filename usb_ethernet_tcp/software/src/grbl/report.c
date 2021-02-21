/*
  report.c - reporting and messaging methods
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

/*
  This file functions as the primary feedback interface for Grbl. Any outgoing data, such
  as the protocol status messages, feedback messages, and status reports, are stored here.
  For the most part, these functions primarily are called from protocol.c methods. If a
  different style feedback is desired (i.e. JSON), then a user can change these following
  methods to accomodate their needs.
*/
#include <string.h>
#include "grbl.h"

#include "k_stdtype.h"
#include "mal.h"
#include "dee.h"
#include "exceptions.h"
#include "ethernet.h"
#include "version.h"
#include "stackmeasure.h"


unsigned int grbl_report_sys_log_0 = 0;
unsigned int grbl_report_sys_log_1 = 0;
unsigned int grbl_report_eeprom_dump = 0;
volatile unsigned int do_grbl_report_50ms = 0;

void report_mac_address(void);

__attribute__(( weak )) unsigned int get_enc28j60_spi_corruption1_cnt(void) { return 0; } 
__attribute__(( weak )) unsigned int get_enc28j60_spi_corruption2_cnt(void) { return 0; } 

// Internal report utilities to reduce flash with repetitive tasks turned into functions.
void report_util_setting_prefix(uint8 n) { putChar_grbl('$'); print_uint8_base10(n); putChar_grbl('='); }
static void report_util_line_feed() { printPgmString(("\r\n")); }
static void report_util_feedback_line_feed() { putChar_grbl(']'); report_util_line_feed(); }
static void report_util_gcode_modes_G() { printPgmString((" G")); }
static void report_util_gcode_modes_M() { printPgmString((" M")); }
// static void report_util_comment_line_feed() { putChar_grbl(')'); report_util_line_feed(); }

static void report_util_axis_values(float *axis_value) {
	uint8 idx;
	for (idx = 0; idx < N_AXIS; idx++) {
		printFloat_CoordValue(axis_value[idx]);
		if (idx < (N_AXIS-1)) { 
			putChar_grbl(','); 
		}
	}
}

/*
static void report_util_setting_string(uint8 n) {
  putChar_grbl(' ');
  putChar_grbl('(');
  switch(n) {
    case 0: printPgmString(("stp pulse")); break;
    case 1: printPgmString(("idl delay")); break; 
    case 2: printPgmString(("stp inv")); break;
    case 3: printPgmString(("dir inv")); break;
    case 4: printPgmString(("stp en inv")); break;
    case 5: printPgmString(("lim inv")); break;
    case 6: printPgmString(("prb inv")); break;
    case 10: printPgmString(("rpt")); break;
    case 11: printPgmString(("jnc dev")); break;
    case 12: printPgmString(("arc tol")); break;
    case 13: printPgmString(("rpt inch")); break;
    case 20: printPgmString(("sft lim")); break;
    case 21: printPgmString(("hrd lim")); break;
    case 22: printPgmString(("hm cyc")); break;
    case 23: printPgmString(("hm dir inv")); break;
    case 24: printPgmString(("hm feed")); break;
    case 25: printPgmString(("hm seek")); break;
    case 26: printPgmString(("hm delay")); break;
    case 27: printPgmString(("hm pulloff")); break;
    case 30: printPgmString(("rpm max")); break;
    case 31: printPgmString(("rpm min")); break;
    case 32: printPgmString(("laser")); break;
    default:
      n -= AXIS_SETTINGS_START_VAL;
      uint8 idx = 0;
      while (n >= AXIS_SETTINGS_INCREMENT) {
        n -= AXIS_SETTINGS_INCREMENT;
        idx++;
      }
      putChar_grbl(n+'x');
      switch (idx) {
        case 0: printPgmString((":stp/mm")); break;
        case 1: printPgmString((":mm/min")); break;
        case 2: printPgmString((":mm/s^2")); break;
        case 3: printPgmString((":mm max")); break;
      }
      break;
  }
  report_util_comment_line_feed();
}
*/

static void report_util_uint8_setting(uint8 n, int val) { 
  report_util_setting_prefix(n); 
  print_uint8_base10(val); 
  report_util_line_feed(); // report_util_setting_string(n); 
}
static void report_util_float_setting(uint8 n, float val, uint8 n_decimal) { 
  report_util_setting_prefix(n); 
  printFloat(val,n_decimal);
  report_util_line_feed(); // report_util_setting_string(n);
}

void init_grbl_report(void) {
}

void do_grbl_report(void) {
	if (do_grbl_report_50ms) {
		do_grbl_report_50ms = 0;
		if (grbl_report_eeprom_dump) {
			static unsigned int grbl_report_eeprom_dump_state = 0;
			static unsigned int add_term = 0;
			switch (grbl_report_eeprom_dump_state) {
				case 0 : {
					add_term = 0;
					printPgmString("eeprom dump:\r\n");
					grbl_report_eeprom_dump_state++;
					break;
				}
				case 1 : {
					printPgmString("Address: ");
					print_uint32_base16((unsigned int) (read_dee_page_startaddress + add_term));
					printPgmString(" Data: ");
					print_uint32_base16((unsigned int) *(read_dee_page_startaddress + add_term));
					printPgmString("\r\n");
					
					add_term++;
					if (add_term >= (read_dee_page_size / 4)) {
						grbl_report_eeprom_dump_state++;
					}
					break;
				}
				case 2 : {
					printPgmString("eeprom dump finished!\r\n");
					grbl_report_eeprom_dump_state++;
					break;
				}
				default : {
					grbl_report_eeprom_dump_state = 0;
					grbl_report_eeprom_dump = 0;
					break;
				}
			}
		}
		if (grbl_report_sys_log_0) {
			static unsigned int grbl_report_sys_log_0_state = 0;
			static unsigned int xterm = 0;
			switch (grbl_report_sys_log_0_state) {
				case 0 : {
					printPgmString("softwareIdentification: ");
					print_uint16_base16(softwareIdentification.year);
					printPgmString("-");
					print_uint8_base16(softwareIdentification.month);
					printPgmString("-");
					print_uint8_base16(softwareIdentification.day);
					printPgmString(" ");
					print_uint16_base16(softwareIdentification.version);
					printPgmString(" ");
					print_uint16_base16(softwareIdentification.softwareType);
					printPgmString("\r\n");

					
					printPgmString("VERSION_DATE: ");
					printPgmString(VERSION_DATE);
					printPgmString("\r\n");

					printPgmString("VERSION_TIME: ");
					printPgmString(VERSION_TIME);
					printPgmString("\r\n");

					printPgmString("VERSION_ID_FIX: ");
					printPgmString(VERSION_ID_FIX);
					printPgmString("\r\n");
					
					printPgmString("Running Minutes: ");
					print_uint32_base10(grbl_running_running_minutes);
					printPgmString("\r\n");
					
					printPgmString("Running Hours: ");
					print_uint32_base10(grbl_running_running_hours);
					printPgmString("\r\n");
					
					printPgmString("Start Up Counter: ");
					print_uint32_base10(grbl_running_startup_cnt);
					printPgmString("\r\n");
					
					printPgmString("Spindle Running Minutes: ");
					print_uint32_base10(grbl_running_spindle_running_minutes);
					printPgmString("\r\n");
					grbl_report_sys_log_0_state++;
					break;
					
				}
				case 1 : {
					{
						printPgmString("Probe HW: ");
						#ifdef PROBE_TRIS
							if (PROBE_PIN) {
								printPgmString("1");
							} else {
								printPgmString("0");
							}
						#else
								printPgmString("N/A");
						#endif
						printPgmString("\r\n");
					}
					{
						printPgmString("Limit X: ");
						#ifdef LIMIT_X_PIN
							if (LIMIT_X_PIN) {
								printPgmString("1");
							} else {
								printPgmString("0");
							}
						#else
								printPgmString("N/A");
						#endif
						printPgmString("\r\n");
					}
					{
						printPgmString("Limit Y: ");
						#ifdef LIMIT_Y_PIN
							if (LIMIT_Y_PIN) {
								printPgmString("1");
							} else {
								printPgmString("0");
							}
						#else
								printPgmString("N/A");
						#endif
						printPgmString("\r\n");
					}
					{
						printPgmString("Limit Z: ");
						#ifdef LIMIT_Z_PIN
							if (LIMIT_Z_PIN) {
								printPgmString("1");
							} else {
								printPgmString("0");
							}
						#else
								printPgmString("N/A");
						#endif
						printPgmString("\r\n");
					}
					
					{
						printPgmString("Abort/Emergency: ");
						#ifdef CONTROL_ABORT_PIN
							if (CONTROL_ABORT_PIN) {
								printPgmString("1");
							} else {
								printPgmString("0");
							}
						#else
								printPgmString("N/A");
						#endif
						printPgmString("\r\n");
					}
					{
						uint16 tmpVal = sys.state;
						printPgmString("sys.state: ");
						print_uint16_base16(tmpVal);
						printPgmString("\r\n");
					}
					{
						uint16 tmpVal = (uint16)system_get_exec_state_flag();
						printPgmString("rt_exec: ");
						print_uint16_base16(tmpVal);
						printPgmString("\r\n");
					}
					{
						report_mac_address();
					}
					{
						if (get_stepper_enable_emergency_stop_active() == STEPPER_ENABLE_MACHINE_STOP_STATE) {
							printPgmString("get_stepper_enable_emergency_stop_active STEPPER_ENABLE_MACHINE_STOP_STATE");
							printPgmString("\r\n");
						} else if (get_stepper_enable_emergency_stop_active() == STEPPER_ENABLE_MACHINE_MOVE_STATE) {
							printPgmString("get_stepper_enable_emergency_stop_active STEPPER_ENABLE_MACHINE_MOVE_STATE");
							printPgmString("\r\n");
						} else {
							printPgmString("get_stepper_enable_emergency_stop_active OTHER");
							printPgmString("\r\n");
						}
					}
					grbl_report_sys_log_0_state++;
					break;
				}
				case 2 : {
					unsigned int rcon_value = 0;
					ResetReason reset_reason_value = 0;
					reset_reason_value = mal_get_reset_reason(&rcon_value);
					printPgmString("reset reason: ");
					switch (reset_reason_value) {
						case ResetReason_Configuration_Word_Mismatch_Reset : {
							printPgmString("ResetReason_Configuration_Word_Mismatch_Reset\r\n");
							break;
						}
						case ResetReason_WDT_Time_out_reset : {
							printPgmString("ResetReason_WDT_Time_out_reset\r\n");
							break;
						}
						case ResetReason_MCLR_reset_during_idle : {
							printPgmString("ResetReason_MCLR_reset_during_idle\r\n");
							break;
						}
						case ResetReason_MCLR_reset_during_sleep : {
							printPgmString("ResetReason_MCLR_reset_during_sleep\r\n");
							break;
						}
						case ResetReason_Software_reset_during_normal_operation : {
							printPgmString("ResetReason_Software_reset_during_normal_operation\r\n");
							break;
						}
						case ResetReason_Software_reset_during_normal_operation_bootloader : {
							printPgmString("ResetReason_Software_reset_during_normal_operation_bootloader\r\n");
							break;
						}
						case ResetReason_MCLR_reset_during_normal_operation : {
							printPgmString("ResetReason_MCLR_reset_during_normal_operation\r\n");
							break;
						}
						case ResetReason_Brown_out_Reset : {
							printPgmString("ResetReason_Brown_out_Reset\r\n");
							break;
						}
						case ResetReason_Power_on_Reset : {
							printPgmString("ResetReason_Power_on_Reset\r\n");
							break;
						}
						case ResetReason_Unknown : {
							printPgmString("ResetReason_Unknown\r\n");
							break;
						}
						default : {
							printPgmString("Unknown\r\n");
							break;
						}
					}
					grbl_report_sys_log_0_state++;
					break;
				}
				case 3 : {
					unsigned int rcon_value = 0;
					mal_get_reset_reason(&rcon_value);
					printPgmString("reset reason raw: ");
					print_uint32_base16(rcon_value);
					printPgmString("\r\n");
					grbl_report_sys_log_0_state++;
					break;
				}
				case 4 : {
					printPgmString("system_log:\r\n");
					xterm = 0;
					grbl_report_sys_log_0_state++;
					break;
				}
				case 5 : {
					unsigned int cnt = (sizeof(system_log) / sizeof(*system_log));
					if (xterm >= cnt) {
						grbl_report_sys_log_0_state++;
					} else {
						switch (system_log[(cnt - 1) - xterm]) {
							case sm_empty : {
								//printPgmString("sm_empty");
								//printPgmString("\r\n");
								break;
							}
							case sm_unlock : {
								printPgmString("sm_unlock");
								printPgmString("\r\n");
								break;
							}
							case sm_abort_input : {
								printPgmString("sm_abort_input");
								printPgmString("\r\n");
								break;
							}
							case sm_internal_stepper_enable : {
								printPgmString("sm_internal_stepper_enable");
								printPgmString("\r\n");
								break;
							}
							case sm_internal_stepper_disable : {
								printPgmString("sm_internal_stepper_disable");
								printPgmString("\r\n");
								break;
							}
							case sm_home : {
								printPgmString("sm_home");
								printPgmString("\r\n");
								break;
							}
							case sm_limit_x : {
								printPgmString("sm_limit_x");
								printPgmString("\r\n");
								break;
							}
							case sm_limit_y : {
								printPgmString("sm_limit_y");
								printPgmString("\r\n");
								break;
							}
							case sm_limit_z : {
								printPgmString("sm_limit_z");
								printPgmString("\r\n");
								break;
							}
							case sm_home_finished : {
								printPgmString("sm_home_finished");
								printPgmString("\r\n");
								break;
							}
							case sm_home_spindle_on : {
								printPgmString("sm_home_spindle_on ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_home_spindle_off : {
								printPgmString("sm_home_spindle_off ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_probe_start : {
								printPgmString("sm_probe_start ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_probe_touch : {
								printPgmString("sm_probe_touch ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_st_reset : {
								printPgmString("sm_st_reset ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_plan_reset : {
								printPgmString("sm_plan_reset ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_st_go_idle : {
								printPgmString("sm_st_go_idle ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_st_wake_up : {
								printPgmString("sm_st_wake_up ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_g28 : {
								printPgmString("sm_g28 ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_st_update_plan_block_parameters : {
								printPgmString("sm_st_update_plan_block_parameters ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_EXEC_MOTION_CANCEL : {
								printPgmString("sm_EXEC_MOTION_CANCEL ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_init_log : {
								printPgmString("sm_init_log ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_idle_log : {
								printPgmString("sm_idle_log ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_suspend_log : {
								printPgmString("sm_suspend_log ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_suspend_delay_log : {
								printPgmString("sm_suspend_delay_log ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_sys_state : {
								printPgmString("sm_sys_state ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							case sm_sys_suspend : {
								printPgmString("sm_sys_suspend ");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
							default : {
								printPgmString("system_log UNKOWN");
								print_uint32_base16(system_log_caller[(cnt - 1) - xterm]);
								printPgmString("\r\n");
								break;
							}
						}
					}
					xterm++;
					break;
				}
				case 6 : {
					printPgmString("Exceptions: 0x");
					print_uint32_base16(exceptions_getException());
					printPgmString("\r\n");
					xterm = 0;
					grbl_report_sys_log_0_state++;
					break;
				}
				case 7 : {
					printPgmString("_excep_addr: ");
					print_uint32_base16(exceptionLog[xterm]._excep_addr );
					printPgmString(" _excep_code: ");
					print_uint32_base16(exceptionLog[xterm]._excep_code );
					printPgmString(" magicWord: ");
					print_uint32_base16(exceptionLog[xterm].magicWord );
					printPgmString("\r\n");
					xterm++;
				
					if (xterm == (sizeof(exceptionLog) / sizeof(*exceptionLog)) ) {
						grbl_report_sys_log_0_state++;
					}
					break;
				}
				case 8 : {
					unsigned int tempVal1 = get_enc28j60_spi_corruption1_cnt();
					unsigned int tempVal2 = get_enc28j60_spi_corruption2_cnt();
					printPgmString(" enc28j60_spi_corruption1_cnt: ");
					print_uint32_base16(tempVal1 );
					printPgmString("\r\n");
					printPgmString(" enc28j60_spi_corruption2_cnt: ");
					print_uint32_base16(tempVal2 );
					printPgmString("\r\n");
					report_status_message(STATUS_OK);

					grbl_report_sys_log_0_state++;
					break;
				}
				default : {
					grbl_report_sys_log_0_state = 0;
					grbl_report_sys_log_0 = 0;
				}
			}
		}
		if (grbl_report_sys_log_1) {
			static unsigned int grbl_report_sys_log_1_state = 0;
			switch (grbl_report_sys_log_1_state) {
				case 0 : {
					{
						printPgmString("Probe HW: ");
						#ifdef PROBE_TRIS
							if (PROBE_PIN) {
								printPgmString("1");
							} else {
								printPgmString("0");
							}
						#else
								printPgmString("N/A");
						#endif
						printPgmString("\r\n");
					}
					{
						printPgmString("Limit X: ");
						#ifdef LIMIT_X_PIN
							if (LIMIT_X_PIN) {
								printPgmString("1");
							} else {
								printPgmString("0");
							}
						#else
								printPgmString("N/A");
						#endif
						printPgmString("\r\n");
					}
					{
						printPgmString("Limit Y: ");
						#ifdef LIMIT_Y_PIN
							if (LIMIT_Y_PIN) {
								printPgmString("1");
							} else {
								printPgmString("0");
							}
						#else
								printPgmString("N/A");
						#endif
						printPgmString("\r\n");
					}
					{
						printPgmString("Limit Z: ");
						#ifdef LIMIT_Z_PIN
							if (LIMIT_Z_PIN) {
								printPgmString("1");
							} else {
								printPgmString("0");
							}
						#else
								printPgmString("N/A");
						#endif
						printPgmString("\r\n");
					}
					
					{
						printPgmString("Abort/Emergency: ");
						#ifdef CONTROL_ABORT_PIN
							if (CONTROL_ABORT_PIN) {
								printPgmString("1");
							} else {
								printPgmString("0");
							}
						#else
								printPgmString("N/A");
						#endif
						printPgmString("\r\n");
					}
					{
						printPgmString("Abort/Emergency Function: ");
						if (system_control_get_state_ABORT()) {
							printPgmString("1");
						} else {
							printPgmString("0");
						}
						printPgmString("\r\n");
					}
					
					grbl_report_sys_log_1_state++;
					break;
				}
				default : {
					grbl_report_sys_log_1_state = 0;
					grbl_report_sys_log_1 = 0;
				}
			}
		}
	}
}

void isr_grbl_report_1ms(void) {
	static unsigned int do_grbl_report_50ms_cnt = 0;
	do_grbl_report_50ms_cnt ++;
	if (do_grbl_report_50ms_cnt >= 50) {
		do_grbl_report_50ms_cnt = 0;
		do_grbl_report_50ms = 1;
	}
}

void grbl_report_dump_eeprom(void) {
	grbl_report_eeprom_dump = 1;
}

// Handles the primary confirmation protocol response for streaming interfaces and human-feedback.
// For every incoming line, this method responds with an 'ok' for a successful command or an
// 'error:'  to indicate some error event with the line or some critical system error during
// operation. Errors events can originate from the g-code parser, settings module, or asynchronously
// from a critical error, such as a triggered hard limit. Interface should always monitor for these
// responses.
void report_status_message(uint8 status_code)
{
  switch(status_code) {
    case STATUS_OK: // STATUS_OK
      printPgmString(("ok\r\n")); break;
    default:
      printPgmString(("error:"));
      print_uint8_base10(status_code);
      report_util_line_feed();
  }
}

// Prints alarm messages.
void report_alarm_message(uint8 alarm_code) {
	printPgmString(("ALARM:"));
	print_uint8_base10(alarm_code);
	report_util_line_feed();
}

// Prints feedback messages. This serves as a centralized method to provide additional
// user feedback for things that are not of the status/alarm message protocol. These are
// messages such as setup warnings, switch toggling, and how to exit alarms.
// NOTE: For interfaces, messages are always placed within brackets. And if silent mode
// is installed, the message number codes are less than zero.
void report_feedback_message(uint8 message_code) {
	printPgmString(("[MSG:"));
	switch(message_code) {
		case MESSAGE_CRITICAL_EVENT:
			printPgmString(("Reset to continue")); break;
		case MESSAGE_ALARM_LOCK:
			printPgmString(("'$H'|'$X' to unlock")); break;
		case MESSAGE_ALARM_UNLOCK:
			printPgmString(("Caution: Unlocked")); break;
		case MESSAGE_ENABLED:
			printPgmString(("Enabled")); break;
		case MESSAGE_DISABLED:
			printPgmString(("Disabled")); break;
		case MESSAGE_SAFETY_DOOR_AJAR:
			printPgmString(("Check Door")); break;
		case MESSAGE_CHECK_LIMITS:
			printPgmString(("Check Limits")); break;
		case MESSAGE_PROGRAM_END:
			printPgmString(("Pgm End")); break;
		case MESSAGE_RESTORE_DEFAULTS:
			printPgmString(("Restoring defaults")); break;
		case MESSAGE_SPINDLE_RESTORE:
			printPgmString(("Restoring spindle")); break;
		case MESSAGE_SLEEP_MODE:
			printPgmString(("Sleeping")); break;
		case MESSAGE_EEP_MASTER_ERROR:
			printPgmString(("Master EEPROM backup read failed")); break;
		case MESSAGE_EEP_BACKUP_ERROR:
			printPgmString(("Backup EEPROM backup read failed. Data Lost!")); break;
		case MESSAGE_ABORT_CYCLE:
			printPgmString(("Emergency Stop! Abort cycle! Position lost!")); break;
		case MESSAGE_ALARM_UNLOCK_FAILED:
			printPgmString(("Failed to clear alarms due to pending problem!")); break;
	}
	report_util_feedback_line_feed();
}


// Welcome message
void report_init_message(void)
{
  printPgmString(("\r\nGrbl " GRBL_VERSION " ['$' for help]\r\n"));
}

// Grbl help message
void report_grbl_help(void) {
	printPgmString(("[HLP:$$ $# $G $I $N $x=val $Nx=line $J=line $SLP $C $X $H ~ ! ? ctrl-x]\r\n"));    
}

void report_grbl_debug_0(void) {
	grbl_report_sys_log_0 = 1;
}

void report_grbl_debug_1(void) {
	grbl_report_sys_log_1 = 1;
}

// Grbl global settings print out.
// NOTE: The numbering scheme here must correlate to storing in settings.c
void report_grbl_settings(void) {
  // Print Grbl settings.
  report_util_uint8_setting(0,settings.pulse_microseconds);
  report_util_uint8_setting(1,settings.stepper_idle_lock_time);
  report_util_uint8_setting(2,settings.step_invert_mask);
  report_util_uint8_setting(3,settings.dir_invert_mask);
  report_util_uint8_setting(4,bit_istrue(settings.flags,BITFLAG_EMPTY));
  report_util_uint8_setting(5,bit_istrue(settings.flags,BITFLAG_INVERT_LIMIT_PINS));
  report_util_uint8_setting(6,bit_istrue(settings.flags,BITFLAG_INVERT_PROBE_PIN));
  report_util_uint8_setting(10,settings.status_report_mask);
  report_util_float_setting(11,settings.junction_deviation,N_DECIMAL_SETTINGVALUE);
  report_util_float_setting(12,settings.arc_tolerance,N_DECIMAL_SETTINGVALUE);
  report_util_uint8_setting(13,bit_istrue(settings.flags,BITFLAG_REPORT_INCHES));
  report_util_uint8_setting(20,bit_istrue(settings.flags,BITFLAG_SOFT_LIMIT_ENABLE));
  report_util_uint8_setting(21,bit_istrue(settings.flags,BITFLAG_HARD_LIMIT_ENABLE));
  report_util_uint8_setting(22,bit_istrue(settings.flags,BITFLAG_HOMING_ENABLE));
  report_util_uint8_setting(23,settings.homing_dir_mask);
  report_util_float_setting(24,settings.homing_feed_rate,N_DECIMAL_SETTINGVALUE);
  report_util_float_setting(25,settings.homing_seek_rate,N_DECIMAL_SETTINGVALUE);
  report_util_uint8_setting(26,settings.homing_debounce_delay);
  report_util_float_setting(27,settings.homing_pulloff,N_DECIMAL_SETTINGVALUE);
  report_util_float_setting(30,settings.rpm_max,N_DECIMAL_RPMVALUE);
  report_util_float_setting(31,settings.rpm_min,N_DECIMAL_RPMVALUE);
  report_util_uint8_setting(32,bit_istrue(settings.flags,BITFLAG_LASER_MODE));
  // Print axis settings
  uint8 idx, set_idx;
  uint8 val = AXIS_SETTINGS_START_VAL;
  for (set_idx=0; set_idx<AXIS_N_SETTINGS; set_idx++) {
    for (idx=0; idx<N_AXIS; idx++) {
      switch (set_idx) {
        case 0: report_util_float_setting(val+idx,settings.steps_per_mm[idx],N_DECIMAL_SETTINGVALUE); break;
        case 1: report_util_float_setting(val+idx,settings.max_rate[idx],N_DECIMAL_SETTINGVALUE); break;
        case 2: report_util_float_setting(val+idx,settings.acceleration[idx]/(60*60),N_DECIMAL_SETTINGVALUE); break;
        case 3: report_util_float_setting(val+idx,-settings.max_travel[idx],N_DECIMAL_SETTINGVALUE); break;
      }
    }
    val += AXIS_SETTINGS_INCREMENT;
  }
}


// Prints current probe parameters. Upon a probe command, these parameters are updated upon a
// successful probe or upon a failed probe with the G38.3 without errors command (if supported).
// These values are retained until Grbl is power-cycled, whereby they will be re-zeroed.
void report_probe_parameters(void)
{
  // Report in terms of machine position.
  printPgmString(("[PRB:"));
  float print_position[N_AXIS];
  system_convert_array_steps_to_mpos(print_position,sys_probe_position);
  report_util_axis_values(print_position);
  putChar_grbl(':');
  print_uint8_base10(sys.probe_succeeded);
  report_util_feedback_line_feed();
}


// Prints Grbl NGC parameters (coordinate offsets, probing)
void report_ngc_parameters()
{
  float coord_data[N_AXIS];
  uint8 coord_select;
  for (coord_select = 0; coord_select <= SETTING_INDEX_NCOORD; coord_select++) {
    if (!(settings_read_coord_data(coord_select,coord_data))) {
      report_status_message(STATUS_SETTING_READ_FAIL);
      return;
    }
    printPgmString(("[G"));
    switch (coord_select) {
      case 6: printPgmString(("28")); break;
      case 7: printPgmString(("30")); break;
      default: print_uint8_base10(coord_select+54); break; // G54-G59
    }
    putChar_grbl(':');
    report_util_axis_values(coord_data);
    report_util_feedback_line_feed();
  }
  printPgmString(("[G92:")); // Print G92,G92.1 which are not persistent in memory
  report_util_axis_values(gc_state.coord_offset);
  report_util_feedback_line_feed();
  printPgmString(("[TLO:")); // Print tool length offset value
  printFloat_CoordValue(gc_state.tool_length_offset);
  report_util_feedback_line_feed();
  report_probe_parameters(); // Print probe parameters. Not persistent in memory.
}


// Print current gcode parser mode state
void report_gcode_modes(void) {
	printPgmString(("[GC:G"));
	if (gc_state.modal.motion >= MOTION_MODE_PROBE_TOWARD) {
		printPgmString(("38."));
		print_uint8_base10(gc_state.modal.motion - (MOTION_MODE_PROBE_TOWARD-2));
	} else {
		print_uint8_base10(gc_state.modal.motion);
	}
	
	report_util_gcode_modes_G();
	print_uint8_base10(gc_state.modal.coord_select+54);
	
	report_util_gcode_modes_G();
	print_uint8_base10(gc_state.modal.plane_select+17);
	
	report_util_gcode_modes_G();
	print_uint8_base10(21-gc_state.modal.units);
	
	report_util_gcode_modes_G();
	print_uint8_base10(gc_state.modal.distance+90);
	
	report_util_gcode_modes_G();
	print_uint8_base10(94-gc_state.modal.feed_rate);
	
	if (gc_state.modal.program_flow) {
		report_util_gcode_modes_M();
		switch (gc_state.modal.program_flow) {
			case PROGRAM_FLOW_PAUSED : putChar_grbl('0'); break;
			// case PROGRAM_FLOW_OPTIONAL_STOP : putChar_grbl('1'); break; // M1 is ignored and not supported.
			case PROGRAM_FLOW_COMPLETED_M2 : 
			case PROGRAM_FLOW_COMPLETED_M30 : 
				print_uint8_base10(gc_state.modal.program_flow);
				break;
		}
	}
	
	report_util_gcode_modes_M();
	switch (gc_state.modal.spindle) {
		case SPINDLE_ENABLE_CW : putChar_grbl('3'); break;
		case SPINDLE_ENABLE_CCW : putChar_grbl('4'); break;
		case SPINDLE_DISABLE : putChar_grbl('5'); break;
	}
	
	report_util_gcode_modes_M();
	#ifdef ENABLE_M7
		if (gc_state.modal.coolant) { // Note: Multiple coolant states may be active at the same time.
		if (gc_state.modal.coolant & PL_COND_FLAG_COOLANT_MIST) { putChar_grbl('7'); }
		if (gc_state.modal.coolant & PL_COND_FLAG_COOLANT_FLOOD) { putChar_grbl('8'); }
		} else { putChar_grbl('9'); }
	#else
		if (gc_state.modal.coolant) {
			putChar_grbl('8'); 
		}else {
			putChar_grbl('9'); 
		}
	#endif
	
	printPgmString((" T"));
	print_uint8_base10(gc_state.tool_requested);
	
	printPgmString((" F"));
	printFloat_RateValue(gc_state.feed_rate);
	
	printPgmString((" S"));
	printFloat(gc_state.spindle_speed,N_DECIMAL_RPMVALUE);
	
	report_util_feedback_line_feed();
}

// Prints build info line
void report_build_info(void)
{
  printPgmString(("[VER:" GRBL_VERSION "." GRBL_VERSION_BUILD ":"));
  report_util_feedback_line_feed();
  printPgmString(("[OPT:")); // Generate compile-time build option list
  putChar_grbl('V');
  #ifdef USE_LINE_NUMBERS
    putChar_grbl('N');
  #endif
  #ifdef ENABLE_M7
    putChar_grbl('M');
  #endif
  #ifdef LIMITS_TWO_SWITCHES_ON_AXES
    putChar_grbl('L');
  #endif
  #ifdef ALLOW_FEED_OVERRIDE_DURING_PROBE_CYCLES
    putChar_grbl('A');
  #endif
  #ifdef SPINDLE_ENABLE_OFF_WITH_ZERO_SPEED
    putChar_grbl('0');
  #endif
  #ifndef ENABLE_RESTORE_EEPROM_WIPE_ALL // NOTE: Shown when disabled.
    putChar_grbl('*');
  #endif
  #ifndef ENABLE_RESTORE_EEPROM_DEFAULT_SETTINGS // NOTE: Shown when disabled.
    putChar_grbl('$');
  #endif
  #ifndef ENABLE_RESTORE_EEPROM_CLEAR_PARAMETERS // NOTE: Shown when disabled.
    putChar_grbl('#');
  #endif
  putChar_grbl('I');
  #ifndef FORCE_BUFFER_SYNC_DURING_EEPROM_WRITE // NOTE: Shown when disabled.
    putChar_grbl('E');
  #endif
  #ifndef FORCE_BUFFER_SYNC_DURING_WCO_CHANGE // NOTE: Shown when disabled.
    putChar_grbl('W');
  #endif
  putChar_grbl('L');

  // NOTE: Compiled values, like override increments/max/min values, may be added at some point later.
  putChar_grbl(',');
  print_uint8_base10(BLOCK_BUFFER_SIZE-1);
  putChar_grbl(',');
  print_uint32_base10(RX_BUFFER_SIZE);

  report_util_feedback_line_feed();
}


// Prints the character string line Grbl has received from the user, which has been pre-parsed,
// and has been sent into protocol_execute_line() routine to be executed by Grbl.
void report_echo_line_received(char *line)
{
  printPgmString(("[echo: ")); printString(line);
  report_util_feedback_line_feed();
}

// Prints build info line
void report_stack_usage(void)
{
	unsigned int stack_usage = getStackFillnessMax();
	unsigned int stack_size = getStackSize();
	printPgmString("STACK:");
	print_uint32_base10(stack_usage);
	printPgmString("/");
	print_uint32_base10(stack_size);
	report_util_line_feed();
}

void report_mac_address(void) {
	printPgmString("MAC: ");
	print_uint8_base16(SerializedMACAddress[0]);
	putChar_grbl('-');
	print_uint8_base16(SerializedMACAddress[1]);
	putChar_grbl('-');
	print_uint8_base16(SerializedMACAddress[2]);
	putChar_grbl('-');
	print_uint8_base16(SerializedMACAddress[3]);
	putChar_grbl('-');
	print_uint8_base16(SerializedMACAddress[4]);
	putChar_grbl('-');
	print_uint8_base16(SerializedMACAddress[5]);
	report_util_line_feed();
}

 // Prints real-time data. This function grabs a real-time snapshot of the stepper subprogram
 // and the actual location of the CNC machine. Users may change the following function to their
 // specific needs, but the desired real-time data report must be as short as possible. This is
 // requires as it minimizes the computational overhead and allows grbl to keep running smoothly,
 // especially during g-code programs with fast, short line segments and high frequency reports (5-20Hz).
void report_realtime_status(void) {
	uint8 idx;
	int32_t current_position[N_AXIS]; // Copy current state of the system position variable
	memcpy(current_position,sys_position,sizeof(sys_position));
	float print_position[N_AXIS];
	system_convert_array_steps_to_mpos(print_position,current_position);

	// Report current machine state and sub-states
	putChar_grbl('<');
	switch (sys.state) {
		case STATE_IDLE: printPgmString(("Idle")); break;
		case STATE_CYCLE: printPgmString(("Run")); break;
		case STATE_HOLD:
			if (!(sys.suspend & SUSPEND_JOG_CANCEL)) {
			printPgmString(("Hold:"));
				if (sys.suspend & SUSPEND_HOLD_COMPLETE) {  // Ready to resume
					putChar_grbl('0'); 
				} else {
					putChar_grbl('1'); // Actively holding
				}
				break;
			} // Continues to print jog state during jog cancel.
		case STATE_JOG: printPgmString(("Jog")); break;
		case STATE_HOMING: printPgmString(("Home")); break;
		case STATE_ALARM: printPgmString(("Alarm")); break;
		case STATE_SAFETY_DOOR:
			printPgmString(("Door:"));
			if (sys.suspend & SUSPEND_INITIATE_RESTORE) {
				putChar_grbl('3'); // Restoring
			} else {
				if (sys.suspend & SUSPEND_RETRACT_COMPLETE) {
					if (sys.suspend & SUSPEND_SAFETY_DOOR_AJAR) {
						putChar_grbl('1'); // Door ajar
					} else {
						putChar_grbl('0');
					} // Door closed and ready to resume
				} else {
					putChar_grbl('2'); // Retracting
				}
			}
		break;
	}

	float wco[N_AXIS];
	if (bit_isfalse(settings.status_report_mask,BITFLAG_RT_STATUS_POSITION_TYPE) || (sys.report_wco_counter == 0) ) {
		for (idx=0; idx< N_AXIS; idx++) {
			// Apply work coordinate offsets and tool length offset to current position.
			wco[idx] = gc_state.coord_system[idx]+gc_state.coord_offset[idx];
			if (idx == TOOL_LENGTH_OFFSET_AXIS) {
				wco[idx] += gc_state.tool_length_offset;
			}
			if (bit_isfalse(settings.status_report_mask,BITFLAG_RT_STATUS_POSITION_TYPE)) {
				print_position[idx] -= wco[idx];
			}
		}
	}

	// Report machine position
	if (bit_istrue(settings.status_report_mask,BITFLAG_RT_STATUS_POSITION_TYPE)) {
		printPgmString(("|MPos:"));
	} else {
		printPgmString(("|WPos:"));
	}
	report_util_axis_values(print_position);

	#ifdef USE_LINE_NUMBERS
		#ifdef REPORT_FIELD_LINE_NUMBERS
			// Report current line number
			plan_block_t * cur_block = plan_get_current_block();
			if (cur_block != NULL) {
				uint32 ln = cur_block->line_number;
				if (ln > 0) {
					printPgmString(("|Ln:"));
					printInteger(ln);
				}
			}
		#endif
	#endif

	// Report realtime feed speed
	#ifdef REPORT_FIELD_CURRENT_FEED_SPEED
		printPgmString(("|FS:"));
		printFloat_RateValue(st_get_realtime_rate());
		putChar_grbl(',');
		printFloat(sys.spindle_speed,N_DECIMAL_RPMVALUE);
	#endif

	#ifdef REPORT_FIELD_PIN_STATE
		uint8 lim_pin_state = limits_get_state_filtered();
		uint8 ctrl_pin_state_ABORT = system_control_get_state_ABORT();
		uint8 ctrl_pin_state_FEED_HOLD = system_control_get_state_FEED_HOLD();
		uint8 ctrl_pin_state_CYCLE_START = system_control_get_state_CYCLE_START();
		uint8 ctrl_pin_state_SAFETY_DOOR = system_control_get_state_SAFETY_DOOR();
	
		uint8 prb_pin_state = probe_get_state();
		if (
			lim_pin_state | 
			ctrl_pin_state_ABORT |
			ctrl_pin_state_FEED_HOLD |
			ctrl_pin_state_CYCLE_START |
			ctrl_pin_state_SAFETY_DOOR | 
			prb_pin_state
		) {
			printPgmString(("|Pn:"));
			if (prb_pin_state) { putChar_grbl('P'); }
			if (lim_pin_state) {
				if (bit_istrue(lim_pin_state,bit(X_AXIS))) { putChar_grbl('X'); }
				if (bit_istrue(lim_pin_state,bit(Y_AXIS))) { putChar_grbl('Y'); }
				if (bit_istrue(lim_pin_state,bit(Z_AXIS))) { putChar_grbl('Z'); }
			}
			if (
				ctrl_pin_state_ABORT |
				ctrl_pin_state_FEED_HOLD |
				ctrl_pin_state_CYCLE_START |
				ctrl_pin_state_SAFETY_DOOR
			) {
				if (ctrl_pin_state_ABORT) { }
				if (ctrl_pin_state_SAFETY_DOOR) { putChar_grbl('D'); }
				if (ctrl_pin_state_FEED_HOLD) { putChar_grbl('H'); }
				if (ctrl_pin_state_CYCLE_START) { putChar_grbl('S'); }
			}
		}
	#endif

	#ifdef REPORT_FIELD_WORK_COORD_OFFSET
		if (sys.report_wco_counter > 0) {
			sys.report_wco_counter--; 
		} else {
			if (sys.state & (STATE_HOMING | STATE_CYCLE | STATE_HOLD | STATE_JOG | STATE_SAFETY_DOOR)) {
				sys.report_wco_counter = (REPORT_WCO_REFRESH_BUSY_COUNT - 1); // Reset counter for slow refresh
			} else { 
				sys.report_wco_counter = (REPORT_WCO_REFRESH_IDLE_COUNT - 1); 
			}
			if (sys.report_ovr_counter == 0) {  // Set override on next report.
				sys.report_ovr_counter = 1; 
			}
			printPgmString(("|WCO:"));
			report_util_axis_values(wco);
		}
	#endif

	#ifdef REPORT_FIELD_OVERRIDES
		if (sys.report_ovr_counter > 0) {
			sys.report_ovr_counter--;
		} else {
			if (sys.state & (STATE_HOMING | STATE_CYCLE | STATE_HOLD | STATE_JOG | STATE_SAFETY_DOOR)) {
				sys.report_ovr_counter = (REPORT_OVR_REFRESH_BUSY_COUNT-1); // Reset counter for slow refresh
			} else {
				sys.report_ovr_counter = (REPORT_OVR_REFRESH_IDLE_COUNT-1);
			}
			printPgmString(("|Ov:"));
			print_uint8_base10(sys.f_override);
			putChar_grbl(',');
			print_uint8_base10(sys.r_override);
			putChar_grbl(',');
			print_uint8_base10(sys.spindle_speed_ovr);

			uint8 sp_state = spindle_get_state();
			uint8 cl_state = coolant_get_state();
			if (sp_state || cl_state) {
				printPgmString(("|A:"));
				if (sp_state) { // != SPINDLE_STATE_DISABLE
					if (sp_state == SPINDLE_STATE_CW) { // CW
						putChar_grbl('S');
					} else { // CCW
						putChar_grbl('C');
					}
				}
				if (cl_state & COOLANT_STATE_FLOOD) {
					putChar_grbl('F'); 
				}
				#ifdef ENABLE_M7
					if (cl_state & COOLANT_STATE_MIST) {
						putChar_grbl('M');
					}
				#endif
			}  
		}
	#endif

	putChar_grbl('>');
	report_util_line_feed();
}
