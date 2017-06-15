/*
  protocol.c - controls Grbl execution protocol and procedures
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
#include <string.h>
#include <stdio.h>
#include "grbl.h"

#include "k_stdtype.h"
#include "mal.h"
#include "tmr.h"
#include "ringBuffer.h"
#include "ethernet.h"

#define RX_LEN_GRBL_PROTOCOL 128
// Define line flags. Includes comment type tracking and line overflow detection.
#define LINE_FLAG_OVERFLOW bit(0)
#define LINE_FLAG_COMMENT_PARENTHESES bit(1)
#define LINE_FLAG_COMMENT_SEMICOLON bit(2)

typedef enum _grbl_line_sm_states {
	sm_processing_chars = 0,
	sm_line_found_pre_sync_check,
	sm_line_found_g_wait_for_processed,
	sm_line_found_wait_pre_sync,
	sm_line_found_jog_wait_for_processed,
	sm_line_wait_home_end,
	sm_after_command_executer,
	sm_synch_motion,
} grbl_line_sm_states;

char line_buffer[LINE_BUFFER_SIZE]; // Line to be executed. Zero-terminated.
grbl_line_sm_states lineStateMachine = sm_processing_chars;

uint8 requestSynchMotion = 0;
Timer protocol_dwellTime;
uint8 gc_execute_line_result = 0;
int32_t gc_execute_line_number = 0;
int32_t jog_line_number = 0;
sint16 protocol_execute_line_status = 0;
float protocol_set_requestDwell_dwellTime = 0.0f;

RingBuffer myRingBuffer_grbl_protocol_rx;
unsigned char grbl_protocol_rx[RX_LEN_GRBL_PROTOCOL];
unsigned long missedRxChar_Grbl_Protocol = 0;
uint8 grbl_protocol_line_flags = 0;
uint8 grbl_protocol_char_counter = 0;

uint8 process_single_char_command(uint8 data);
uint8 process_multi_char_command(uint8 data, char *line);
sint16 protocol_execute_line(char *line, int32_t *number); // Executes an internal system command, defined as a string starting with a '$'
int getChar_grbl_protocol(void);
void putChar_grbl_protocol(unsigned char data);
	
void init_protocol_main_loop(void) {
	lineStateMachine = sm_processing_chars;

	requestSynchMotion = 0;
	gc_execute_line_result = 0;
	gc_execute_line_number = 0;
	jog_line_number = 0;
	protocol_execute_line_status = 0;
	protocol_set_requestDwell_dwellTime = 0.0f;

	missedRxChar_Grbl_Protocol = 0;

	grbl_protocol_line_flags = 0;
	grbl_protocol_char_counter = 0;
	
	ringBuffer_initBuffer(&myRingBuffer_grbl_protocol_rx, grbl_protocol_rx, sizeof(grbl_protocol_rx) / sizeof(*grbl_protocol_rx));
	init_timer(&protocol_dwellTime);
	lineStateMachine = sm_processing_chars;
}

void do_protocol_main_loop(void) {
	{
		int ch = getChar_grbl();
		if (ch != -1) {
			uint8 dataCh = ch;
			if (process_single_char_command(dataCh) != 0) {
				//Found a Single Command Char
				//Nothing to be done, already processed
			} else {
				//Pass char to state machine
				putChar_grbl_protocol(dataCh);
			}
		}
	}
	switch (lineStateMachine) {
		case sm_processing_chars : {
			int ch = getChar_grbl_protocol();
			if (ch != -1) {
				uint8 dataCh = ch;
				if (process_multi_char_command(dataCh, line_buffer) != 0) {
					//Found a Jog or G Line
					if (line_buffer[0] == '$') {
						lineStateMachine = sm_line_found_jog_wait_for_processed;
					} else {
						lineStateMachine = sm_line_found_pre_sync_check;
					}
				}
			}			
			break;
		}
		case sm_line_found_pre_sync_check : {
			if (gc_check_for_sync_needed(line_buffer)) {
				lineStateMachine = sm_line_found_wait_pre_sync;
			} else {
				lineStateMachine = sm_line_found_g_wait_for_processed;
			}
			break;
		}
		case sm_line_found_g_wait_for_processed : {
			if (plan_check_full_buffer()) {
				//Here we have to wait that the buffer becomes not full
			} else {
				// Parse and execute g-code block.
				gc_execute_line_result = gc_execute_line(line_buffer, &gc_execute_line_number);
				lineStateMachine = sm_after_command_executer;
			}
			break;
		}
		case sm_line_found_wait_pre_sync : {
			if (plan_get_current_block() || st_isRunning()) {
				//Here we have to wait since the next command needs to be finished every command before it
			} else {
				lineStateMachine = sm_line_found_g_wait_for_processed;
			}
			break;
		}
		case sm_line_found_jog_wait_for_processed : {
			if (plan_check_full_buffer()) {
				//Here we have to wait that the buffer becomes not full
			} else {
				// Parse and execute jog block.
				protocol_execute_line_status = protocol_execute_line(line_buffer, &jog_line_number);
				if (isMc_homing_cycleRunning()) {
					lineStateMachine = sm_line_wait_home_end;
				} else {
					if (protocol_execute_line_status != STATUS_ASYNCH_OK) {
						report_status_message(protocol_execute_line_status, jog_line_number);
					}
					lineStateMachine = sm_processing_chars;
				}
			}
			break;
		}
		case sm_line_wait_home_end : {
			if (isMc_homing_cycleRunning()) {
			} else {
				report_status_message(protocol_execute_line_status, 0);
				system_set_sys_state(STATE_IDLE); // Set to IDLE when complete.
				system_log_st_go_idle(3);
				st_go_idle(); // Set steppers to the settings idle state before returning.
				lineStateMachine = sm_processing_chars;
			}
			break;
		}
		case sm_after_command_executer : {
			if (protocol_get_requestSynchMotion()) {
				protocol_clear_requestSynchMotion();
				lineStateMachine = sm_synch_motion;
			} else if (protocol_get_requestDwell()) {
				//wait until dwell is finished
			} else if (isArcGeneratingRunning() != 0) {
				//wait until arc generator is finished
			} else if (isLimits_go_homeRunning() != 0) {
				//wait until Limit Home generator is finished
			} else if (isMc_homing_cycleRunning() != 0) {
				//wait until Mc Home is finished
			} else if (st_isDelayRunning() != 0) {
				//wait until delay is done
			} else if (isProbing_cycleRunning() != 0) {
				//wait until probing is done
			} else if (isToolChangeRunning() != 0) {
				//wait until delay is done
			} else {
				report_status_message(gc_execute_line_result, gc_execute_line_number);
				lineStateMachine = sm_processing_chars;
			}
			break;
		}
		case sm_synch_motion : {
			if (plan_get_current_block() || (sys.state == STATE_CYCLE)) {
				//wait until cycle is finished
			} else {
				if (protocol_get_requestDwell()) {//if request dwell is active we have to activate the delay time now
					protocol_update_requestDwell();
				}
				if (isToolChangeRunning() != 0) {//if tool change request is pending, now we have to activate this
					updateToolChangeRunning();
				}
				lineStateMachine = sm_after_command_executer;
			}
			break;
		}
		default : {
			lineStateMachine = sm_processing_chars;
			break;
		}
	}
}

void isr_protocol_main_loop_1ms(void) {
}

void protocol_set_requestSynchMotion(void) {
	requestSynchMotion = 1;
}

uint8 protocol_get_requestSynchMotion(void) {
	return requestSynchMotion;
}

void protocol_clear_requestSynchMotion(void) {
	requestSynchMotion = 0;
}

void protocol_set_requestDwell(float milliseconds) {
	protocol_set_requestDwell_dwellTime = milliseconds;
}

void protocol_update_requestDwell(void) {
	uint32 newValue = (protocol_set_requestDwell_dwellTime);
	protocol_set_requestDwell_dwellTime = 0.0f;
	write_timer(&protocol_dwellTime, newValue);
}

uint8 protocol_get_requestDwell(void) {
	uint8 result = 0;
	if ((read_timer(&protocol_dwellTime) != 0) || (protocol_set_requestDwell_dwellTime != 0.0f)) {
		result = 1;
	}
	return result;
}

uint8 process_single_char_command(uint8 data) {
	uint8 result = 0;
	
	switch (data) {
		case CMD_STACK_REPORT : {
			report_stack_usage();
			result = 1;
			break;
		}
		case CMD_RESET : {
			//We need to support reset since the UGS relay on it with the stop command
			grbl_reset_asyn_execute = 2;
			result = 1;
			break; // Call motion control reset routine.
		}
		case CMD_BOOTLOADER : {
			grbl_bootloader_trigger(1);
			result = 1;
			break; // Call motion control reset routine.
		}
		case CMD_STATUS_REPORT : {
			system_set_exec_state_flag(EXEC_STATUS_REPORT); 
			result = 1;
			break; // Set as true
		}
		case CMD_CYCLE_START : {
			system_set_exec_state_flag(EXEC_CYCLE_START); 
			result = 1;
			break; // Set as true
		}
		case CMD_FEED_HOLD : {
			system_set_exec_state_flag(EXEC_FEED_HOLD); 
			result = 1;
			break; // Set as true
		}
		default : {
			if (data > 0x7F) { // Real-time control characters are extended ACSII only.
				result = 1;
				switch(data) {
					case CMD_SAFETY_DOOR: {
						system_set_exec_state_flag(EXEC_SAFETY_DOOR); 
						break; // Set as true
					}
					case CMD_JOG_CANCEL: {
						if (sys.state & STATE_JOG) { // Block all other states from invoking motion cancel.
							system_set_exec_state_flag(EXEC_MOTION_CANCEL); 
						}
						break; 
					}
					case CMD_FEED_OVR_RESET: {
						system_set_exec_motion_override_flag(EXEC_FEED_OVR_RESET); 
						break;
					}
					case CMD_FEED_OVR_COARSE_PLUS: {
						system_set_exec_motion_override_flag(EXEC_FEED_OVR_COARSE_PLUS); 
						break;
					}
					case CMD_FEED_OVR_COARSE_MINUS: {
						system_set_exec_motion_override_flag(EXEC_FEED_OVR_COARSE_MINUS); 
						break;
					}
					case CMD_FEED_OVR_FINE_PLUS: {
						system_set_exec_motion_override_flag(EXEC_FEED_OVR_FINE_PLUS); 
						break;
					}
					case CMD_FEED_OVR_FINE_MINUS: {
						system_set_exec_motion_override_flag(EXEC_FEED_OVR_FINE_MINUS); 
						break;
					}
					case CMD_RAPID_OVR_RESET: {
						system_set_exec_motion_override_flag(EXEC_RAPID_OVR_RESET); 
						break;
					}
					case CMD_RAPID_OVR_MEDIUM: {
						system_set_exec_motion_override_flag(EXEC_RAPID_OVR_MEDIUM); 
						break;
					}
					case CMD_RAPID_OVR_LOW: {
						system_set_exec_motion_override_flag(EXEC_RAPID_OVR_LOW); 
						break;
					}
					case CMD_SPINDLE_OVR_RESET: {
						system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_RESET); 
						break;
					}
					case CMD_SPINDLE_OVR_COARSE_PLUS: {
						system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_COARSE_PLUS); 
						break;
					}
					case CMD_SPINDLE_OVR_COARSE_MINUS: {
						system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_COARSE_MINUS); 
						break;
					}
					case CMD_SPINDLE_OVR_FINE_PLUS: {
						system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_FINE_PLUS); 
						break;
					}
					case CMD_SPINDLE_OVR_FINE_MINUS: {
						system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_FINE_MINUS);
						break;
					}
					case CMD_SPINDLE_OVR_STOP: {
						system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_STOP); 
						break;
					}
					case CMD_COOLANT_FLOOD_OVR_TOGGLE: {
						system_set_exec_accessory_override_flag(EXEC_COOLANT_FLOOD_OVR_TOGGLE); 
						break;
					}
					#ifdef ENABLE_M7
					case CMD_COOLANT_MIST_OVR_TOGGLE: {
						system_set_exec_accessory_override_flag(EXEC_COOLANT_MIST_OVR_TOGGLE); 
						break;
					}
					#endif
				}
				// Throw away any unfound extended-ASCII character by not passing it to the serial buffer.
			}
		}
	}	
	return result;
}

uint8 process_multi_char_command(uint8 data, char *line) {
	uint8 result = 0;
	uint8 strlen_line = 0;
	char c = data;

	if (grbl_protocol_line_flags & LINE_FLAG_COMMENT_PARENTHESES) {
		if (c == ')') {
			// End of '()' comment. Resume line allowed.
			grbl_protocol_line_flags &= ~(LINE_FLAG_COMMENT_PARENTHESES); 
		}
	} else {
		if (grbl_protocol_line_flags & LINE_FLAG_COMMENT_SEMICOLON) {
			if ((c == '\n') || (c == '\r')) {
				grbl_protocol_line_flags &= ~(LINE_FLAG_COMMENT_SEMICOLON);
			}
		}
		if (grbl_protocol_line_flags == 0) {
			if ((c == '\n') || (c == '\r')) { // End of line reached
				line[grbl_protocol_char_counter] = 0; // Set string termination character.
				strlen_line = grbl_protocol_char_counter;
				grbl_protocol_char_counter = 0;
				#ifdef REPORT_ECHO_LINE_RECEIVED
					report_echo_line_received(line);
				#endif

				// Direct and execute one line of formatted input, and report status of execution.
				if (grbl_protocol_line_flags & LINE_FLAG_OVERFLOW) {
					// Report line overflow error.
					report_status_message(STATUS_OVERFLOW, 0);
					// Reset tracking data for next line.
					memset(line, 0, LINE_BUFFER_SIZE);
				}
				grbl_protocol_line_flags = 0;
				
				if (line[0] == 0) {
					// Empty or comment line. For syncing purposes.
					report_status_message(STATUS_OK, 0);
					// Reset tracking data for next line.
					memset(line, 0, LINE_BUFFER_SIZE);
				} else if ( (strlen("BOOTLOADER") <= strlen_line) && (strcmp(line, "BOOTLOADER") == 0) ) {
					grbl_bootloader_trigger(1);
					memset(line, 0, LINE_BUFFER_SIZE);
				} else if ( (strlen("RESET1701") <= strlen_line) && (strcmp(line, "RESET1701") == 0) ) {
					grbl_bootloader_trigger(0);
					memset(line, 0, LINE_BUFFER_SIZE);
				} else if ( (strlen("DUMPEEPROM") <= strlen_line) && (strcmp(line, "DUMPEEPROM") == 0) ) {
					grbl_report_dump_eeprom();
					memset(line, 0, LINE_BUFFER_SIZE);
				} else if ( (strlen("GETEEPROM:aaaaaaaa") <= strlen_line) && (strncmp(line, "GETEEPROM:", 10) == 0) ) {
					grbl_eeprom_get_eeprom((unsigned char *)&line[10]);
					report_status_message(STATUS_OK, 0);
					memset(line, 0, LINE_BUFFER_SIZE);
				} else if ( (strlen("SETEEPROM:aaaaaaaa:dddddddd") <= strlen_line) && (strncmp(line, "SETEEPROM:", 10) == 0) ) {
					grbl_eeprom_set_eeprom((unsigned char *)&line[10]);
					report_status_message(STATUS_OK, 0);
					memset(line, 0, LINE_BUFFER_SIZE);
				} else if ( (strlen("MAC:xx-xx-xx-xx-xx-xx") <= strlen_line) && (strncmp(line, "MAC:", 4) == 0) ) {
					ethernet_set_mac_address((unsigned char *)&line[4]);
					grbl_eeprom_trigger();
					report_status_message(STATUS_OK, 0);
					memset(line, 0, LINE_BUFFER_SIZE);
				} else if (line[0] == '$') {
					// Grbl '$' system command
					result = 1;
				} else if (sys.state & (STATE_ALARM | STATE_JOG)) {
					// Everything else is gcode. Block if in alarm or jog mode.
					report_status_message(STATUS_SYSTEM_GC_LOCK, 0);
					// Reset tracking data for next line.
					memset(line, 0, LINE_BUFFER_SIZE);
				} else if (line[0] == '\n') {
					result = 1;
				} else if (line[0] == '\r') {
					//Skip empty line
				} else {
					result = 1;
				}
			} else {
				if (c <= ' ') {
					// Throw away whitepace and control characters
				} else if (c == '/') {
					// Block delete NOT SUPPORTED. Ignore character.
					// NOTE: If supported, would simply need to check the system if block delete is enabled.
				} else if (c == '(') {
					// Enable comments flag and ignore all characters until ')' or EOL.
					// NOTE: This doesn't follow the NIST definition exactly, but is good enough for now.
					// In the future, we could simply remove the items within the comments, but retain the
					// comment control characters, so that the g-code parser can error-check it.
					grbl_protocol_line_flags |= LINE_FLAG_COMMENT_PARENTHESES;
				} else if (c == ';') {
					// NOTE: ';' comment to EOL is a LinuxCNC definition. Not NIST.
					grbl_protocol_line_flags |= LINE_FLAG_COMMENT_SEMICOLON;
				} else if (grbl_protocol_char_counter >= (LINE_BUFFER_SIZE - 1) ) {
					// Detect line buffer overflow and set flag.
					grbl_protocol_line_flags |= LINE_FLAG_OVERFLOW;
				} else if (c >= 'a' && c <= 'z') { // Upcase lowercase
					line[grbl_protocol_char_counter] = c - 'a' + 'A';
					grbl_protocol_char_counter++;
				} else {
					line[grbl_protocol_char_counter] = c;
					grbl_protocol_char_counter++;
				}
			}
		}
	}

	

	//need to process further
	//Pass char to next part of code



	if (grbl_protocol_line_flags) {
		// Throw away all (except EOL) comment characters and overflow characters.
	} else {

	}
	return result;
}

// Directs and executes one line of formatted input from protocol_process. While mostly
// incoming streaming g-code blocks, this also executes Grbl internal commands, such as
// settings, initiating the homing cycle, and toggling switch states. This differs from
// the realtime command module by being susceptible to when Grbl is ready to execute the
// next line during a cycle, so for switches like block delete, the switch only effects
// the lines that are processed afterward, not necessarily real-time during a cycle,
// since there are motions already stored in the buffer. However, this 'lag' should not
// be an issue, since these commands are not typically used during a cycle.
sint16 protocol_execute_line(char *line, int32_t *number) {
	uint8 char_counter = 1;
	float parameter = 0.0f;
	float value = 0.0f;
	switch( line[char_counter] ) {
		case 0 : {
			report_grbl_help();
			break;
		}
		case 'J' : { // Jogging
			// Execute only if in IDLE or JOG states.
			if ((sys.state != STATE_IDLE) && (sys.state != STATE_JOG)) {
				return(STATUS_IDLE_ERROR);
			}
			if (line[2] != '=') {
				return(STATUS_INVALID_STATEMENT);
			}
			return(gc_execute_line(line, number)); // NOTE: $J= is ignored inside g-code parser and used to detect jog motions.
			break;
		}
		case '$': case 'G': case 'C': case 'X': case 'D': {
			if ( line[2] != 0 ) {
				switch( line[1] ) {
					case 'D' : {
						if (line[2] == '1') {
							report_grbl_debug_1();
						} else if (line[2] == '0') {
							report_grbl_debug_0();
						} else {
							report_grbl_debug_0();
						}
						return (STATUS_ASYNCH_OK);
						break;
					}
				}
				return(STATUS_INVALID_STATEMENT); 
			}
			switch( line[1] ) {
				case '$' : { // Prints Grbl settings
					if ( sys.state & (STATE_CYCLE | STATE_HOLD) ) {  // Block during cycle. Takes too long to print.
						return(STATUS_IDLE_ERROR); 
					} else {
						report_grbl_settings(); 
					}
					break;
				}
				case 'D' : {
					report_grbl_debug_1();
					return (STATUS_ASYNCH_OK);
					break;
				}
				case 'G' : {// Prints gcode parser state
					// TODO: Move this to realtime commands for GUIs to request this data during suspend-state.
					report_gcode_modes();
					break;
				}
				case 'X' : {// Disable alarm lock [ALARM]
					if (sys.state == STATE_ALARM) {
					// Block if safety door is ajar.
						if (system_check_safety_door_ajar()) {
							return(STATUS_CHECK_DOOR);
						}
						system_set_exec_state_flag(EXEC_CLEAR_ALARM);
					} // Otherwise, no effect.
					break;
				}
			}
			break;
		}
		default : {
			// Block any system command that requires the state as IDLE/ALARM. (i.e. EEPROM, homing)
			if ( !(sys.state == STATE_IDLE || sys.state == STATE_ALARM) ) {
				return(STATUS_IDLE_ERROR); 
			}
			switch( line[1] ) {
				case 'S' : { // Prints Grbl settings
					report_stack_usage(); 
					break;
				}
				case '#' : {// Print Grbl NGC parameters
					if ( line[2] != 0 ) {
						return(STATUS_INVALID_STATEMENT); 
					} else {
						report_ngc_parameters(); 
					}
					break;
				}
				case 'H' : {// Perform homing cycle [IDLE/ALARM]
					if (bit_isfalse(settings.flags,BITFLAG_HOMING_ENABLE)) {
						return(STATUS_SETTING_DISABLED); 
					}
					if (system_check_safety_door_ajar()) { // Block if safety door is ajar.
						return(STATUS_CHECK_DOOR); 
					}
					if (get_stepper_enable_emergency_stop_active() == STEPPER_ENABLE_MACHINE_STOP_STATE) {// Check if motors are enabled
						return(STATUS_IDLE_ERROR);
					}
					system_set_sys_state(STATE_HOMING); // Set system state variable
					if (line[2] == 0) {
						mc_homing_cycle(HOMING_CYCLE_ALL);
					} else {
						return(STATUS_INVALID_STATEMENT);
					}
					break;
				}
				case 'I' : {// Print or store build info. [IDLE/ALARM]
					if ( line[++char_counter] == 0 ) {
						report_build_info();
					}
					break;
				}
				case 'R' : { // Restore defaults [IDLE/ALARM]
					if ((line[2] != 'S') || (line[3] != 'T') || (line[4] != '=') || (line[6] != 0)) {
						return(STATUS_INVALID_STATEMENT);
					}
					switch (line[5]) {
						#ifdef ENABLE_RESTORE_EEPROM_DEFAULT_SETTINGS
							case '$': {
								settings_restore(SETTINGS_RESTORE_DEFAULTS); 
								break;
							}
						#endif
						#ifdef ENABLE_RESTORE_EEPROM_CLEAR_PARAMETERS
							case '#': {
								settings_restore(SETTINGS_RESTORE_PARAMETERS); 
								break;
							}
						#endif
						#ifdef ENABLE_RESTORE_EEPROM_WIPE_ALL
							case '*': {
								settings_restore(SETTINGS_RESTORE_ALL); 
								break;
							}
						#endif
						default: {
							return(STATUS_INVALID_STATEMENT);
						}
					}
					report_feedback_message(MESSAGE_RESTORE_DEFAULTS);
					break;
				}
				default : {// Storing setting methods [IDLE/ALARM]
					if(!read_float(line, &char_counter, &parameter)) {
						return(STATUS_BAD_NUMBER_FORMAT);
					}
					if(line[char_counter++] != '=') {
						return(STATUS_INVALID_STATEMENT);
					}
					{ // Store global setting.
						if(!read_float(line, &char_counter, &value)) {
							return(STATUS_BAD_NUMBER_FORMAT);
						}
						if((line[char_counter] != 0) || (parameter > 255)) {
							return(STATUS_INVALID_STATEMENT);
						}
						return(settings_store_global_setting((uint8)parameter, value));
					}
				}
			}
		}
	}
	return(STATUS_OK); // If '$' command makes it to here, then everything's ok.
}

int getChar_grbl_protocol(void) {
	int result = -1;
	unsigned char data = 0;
	if (ringBuffer_getItem(&myRingBuffer_grbl_protocol_rx, &data) != 0) {
		result = data;
	}
	return result;
}

void putChar_grbl_protocol(unsigned char data) {
	if (ringBuffer_addItem(&myRingBuffer_grbl_protocol_rx, data) != -1) {
	} else {
		missedRxChar_Grbl_Protocol++;
	}
}
