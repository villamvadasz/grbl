#include "app.h"
#include "k_stdtype.h"
#include "version.h"

#ifdef APP_BOOTLOADER_FOTA

#include "mal.h"
#include "aes_128.h"

#ifdef __32MX795F512H__
	#define APP_BOOTLOADER_UID "4D2E6F08"
#else
	#error Deriavate not supported
#endif

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "c_app.h"
#include "wget.h"
#include "wget_fsio.h"
#include "tmr.h"
#include "rtc.h"
#include "wifi.h"
#include "tokenize.h"
#include "ethernet.h"

#ifndef WIFI_USER_BOOTLOADER
	#define WIFI_USER_BOOTLOADER	0x00000000
#endif

#define APP_BOOTLOADER_CHECK_PERIOD_DAY 1	//[d]
#define APP_BOOTLOADER_TIMEOUT		60000 //[ms]
#define APP_BOOTLOADER_WGET_TIMEOUT		600000 //[ms]
#define APP_BOOTLOADER_WGET_RETRY		50//[cnt]

#define APP_BOOTLOADER_URL_BUFFER_LEN 256

//#define APP_BOOTLOADER_TRIGGER_TIMER

typedef enum _app_bootloader_states {
	APP_BL_STATE_INIT = 0,
	APP_BL_STATE_ERASE_STORAGE,
	APP_BL_STATE_STORAGE_ERROR,
	APP_BL_STATE_WAIT_NETWORK,
	APP_BL_STATE_WGET_NXT_FILE_0,
	APP_BL_STATE_WGET_NXT_FILE_1,
	APP_BL_STATE_WGET_NXT_FILE_2,
	APP_BL_STATE_WGET_SIG_FILE_0,
	APP_BL_STATE_WGET_HEX_FILE_0,
	APP_BL_STATE_WGET_HEX_FILE_1,
	APP_BL_STATE_WGET_HEX_FILE_2,
	APP_BL_STATE_TRIGGER_RESET,
	APP_BL_STATE_DONE,
} app_bootloader_states;

app_bootloader_states app_bootloader_state = APP_BL_STATE_INIT;
volatile uint32 do_app_bootloader_1ms = 0;
uint32 app_bootloader_msec_cnt = 0;
uint32 app_bootloader_sec_cnt = 0;
uint32 app_bootloader_min_cnt = 0;
uint32 app_bootloader_hour_cnt = 0;
uint32 app_bootloader_day_cnt = 0;
uint32 app_bootloader_check_for_new_sw = 0;
Timer app_bootloader_wget_tmr;
uint32 app_bootloader_addr = 0;
uint8 app_bootloader_data = 0;
TimeDate app_bootloader_timedate_current;
TimeDate app_bootloader_timedate_next;
uint32 app_bootloader_wget_url_retry = 5;
uint32 app_bootloader_last_nxt = 0;
char app_bootloader_wget_url[APP_BOOTLOADER_URL_BUFFER_LEN] = {0};

const char app_bootloader_uid_expected[] = APP_BOOTLOADER_UID;

const char app_bootloader_url[] = "http://kyrk.villamvadasz.hu/_pic_sw_bootloader";
char app_bootloader_next_version_file_name[] = "00000000.nxt";
const char app_bootloader_next_hex_file_name_format[] = "202502021423_4D2E6F08.hsx";//DATE_UID
char app_bootloader_next_hex_file_name[128] = "202502021423_4D2E6F08.hsx";//DATE_UID
unsigned int app_bootloader_next_hex_file_name_cnt = 0;
unsigned char app_bootloader_expected_aes[16] = {0};
char app_bootloader_expected_aes_string[33] = {0};

unsigned int app_bootloader_check_file_name(char *str, unsigned int exp_len);
unsigned int app_bootloader_check_nxt_entry(char *str, char *uid_exp, unsigned int serial_exp, unsigned char *signature_out, char *signature_out_string);

__attribute__(( weak )) void wifi_request(uint32 user) {}
__attribute__(( weak )) void wifi_release(uint32 user) {}
__attribute__(( weak )) uint8 wifi_is_connected(void) {return 1;}

void init_app_bootloader(void) {
	init_timer(&app_bootloader_wget_tmr);
	
	sprintf(app_bootloader_next_version_file_name, "%s.nxt", APP_BOOTLOADER_UID);
	
	#warning When working good enough, remove this
	//app_bootloader_check_for_new_sw = 1;

	app_bootloader_nvm_init();
}

void do_app_bootloader(void) {
	if (do_app_bootloader_1ms) {
		do_app_bootloader_1ms = 0;
		do_app_bootloader_nvm();

		//1ms
		#ifdef APP_BOOTLOADER_TRIGGER_TIMER
			app_bootloader_msec_cnt++;
			if (app_bootloader_msec_cnt >= 1000) {
				app_bootloader_msec_cnt = 0;
				//1 sec
				app_bootloader_sec_cnt++;
				if (app_bootloader_sec_cnt >= 60) {
					app_bootloader_sec_cnt = 0;
					//1 min
					app_bootloader_min_cnt++;
					if (app_bootloader_min_cnt >= 60) {
						app_bootloader_min_cnt = 0;
						//1 hour
						app_bootloader_hour_cnt++;
						if (app_bootloader_hour_cnt >= 24) {
							app_bootloader_hour_cnt = 0;
							//1 day
							app_bootloader_day_cnt++;
							if (app_bootloader_day_cnt >= APP_BOOTLOADER_CHECK_PERIOD_DAY) {
								app_bootloader_day_cnt = 0;
								app_bootloader_check_for_new_sw = 1;
							}
						}
					}
				}
			}
		#endif
	}
	switch (app_bootloader_state) {
		case APP_BL_STATE_INIT : {
			if (ethernet_is_initialized()) {
				if (app_bootloader_check_for_new_sw) {
					sint32 result = app_bootloader_nvm_check_erased();
					app_bootloader_wget_url_retry = APP_BOOTLOADER_WGET_RETRY;
					if (result == -1) {//Busy
					} else if (result == 0) {//OK //erased state
						app_bootloader_check_for_new_sw = 0;
						wifi_request(WIFI_USER_BOOTLOADER);
						app_bootloader_state = APP_BL_STATE_WAIT_NETWORK;
					} else {
						app_bootloader_state = APP_BL_STATE_ERASE_STORAGE;
					}
				}
			}
			break;
		}
		case APP_BL_STATE_ERASE_STORAGE : {
			sint32 result = app_bootloader_nvm_erase_asynch();
			if (result == -1) {//Busy
			} else if (result == 0) {//OK //erased
				app_bootloader_state = APP_BL_STATE_DONE;
			} else {//Failed to erase
				app_bootloader_state = APP_BL_STATE_STORAGE_ERROR;
			}
			break;
		}
		case APP_BL_STATE_STORAGE_ERROR : {
			wifi_release(WIFI_USER_BOOTLOADER);
			//Stay here for ever
			break;
		}
		case APP_BL_STATE_WAIT_NETWORK : {
			if (wifi_is_connected()) {
				write_timer(&app_bootloader_wget_tmr, APP_BOOTLOADER_TIMEOUT);
				app_bootloader_state = APP_BL_STATE_WGET_NXT_FILE_0;
			}
			break;
		}
		case APP_BL_STATE_WGET_NXT_FILE_0 : {
			sprintf(app_bootloader_wget_url, "%s/%s", app_bootloader_url, app_bootloader_next_version_file_name);

			if (wget_url(app_bootloader_wget_url) == 0) {
				write_timer(&app_bootloader_wget_tmr, APP_BOOTLOADER_WGET_TIMEOUT);
				app_bootloader_next_hex_file_name_cnt = 0;
				memset(app_bootloader_next_hex_file_name, 0x00, sizeof(app_bootloader_next_hex_file_name));
				app_bootloader_state = 	APP_BL_STATE_WGET_NXT_FILE_1;
			} else {
				if (read_timer(&app_bootloader_wget_tmr) == 0) {
					app_bootloader_state = 	APP_BL_STATE_DONE;
				}
			}
			break;
		}
		case APP_BL_STATE_WGET_NXT_FILE_1 : {
			if (read_timer(&app_bootloader_wget_tmr) == 0) {
				wget_url_cancel();
				app_bootloader_state = 	APP_BL_STATE_DONE;
			} else {
				app_bootloader_last_nxt = 0;
				while (1) {
					volatile int ch = wget_getByte();
					if (ch == -1) {
						break; //come back here later
					} else if (ch == -2) {
						app_bootloader_last_nxt = 1;
						app_bootloader_state = APP_BL_STATE_WGET_NXT_FILE_2;
						break;
					} else if ((ch == '\r') || (ch == '\n')) {
						if (app_bootloader_next_hex_file_name_cnt == 0) {
						} else {
							app_bootloader_next_hex_file_name[app_bootloader_next_hex_file_name_cnt] = 0;
							app_bootloader_next_hex_file_name_cnt++;
							app_bootloader_state = APP_BL_STATE_WGET_NXT_FILE_2;
						}
						break;
					} else {
						app_bootloader_next_hex_file_name[app_bootloader_next_hex_file_name_cnt] = ch;
						app_bootloader_next_hex_file_name_cnt++;
						if (app_bootloader_next_hex_file_name_cnt >= ((sizeof(app_bootloader_next_hex_file_name)) / (sizeof(*app_bootloader_next_hex_file_name))) ) {
							wget_url_cancel();
							app_bootloader_state = 	APP_BL_STATE_DONE;
							break;
						}
					}
				}
			}
			break;
		}
		case APP_BL_STATE_WGET_NXT_FILE_2 : {
			//Here the received stuff will contain some answer from the server. WGET need to be changed to filter that out
			//File name to flash.hsx    UID      SERIAL   Signature
			//202502021423_4D2E6F08.hsx 4D2E6F08 DE000002 C37832B3141D8C75036FEA5F4377D4DA  app_bootloader_next_hex_file_name
			tokenize_trim(app_bootloader_next_hex_file_name);
			
			unsigned int nxt_entry_result = app_bootloader_check_nxt_entry(app_bootloader_next_hex_file_name, (char *)app_bootloader_uid_expected, VERSION_APP_SERIAL_NUMBER, app_bootloader_expected_aes, app_bootloader_expected_aes_string);
			if (nxt_entry_result == 0) {
				app_bootloader_next_hex_file_name_cnt = 0;
				if (app_bootloader_last_nxt) {
					app_bootloader_state = 	APP_BL_STATE_DONE;
				} else {
					app_bootloader_state = 	APP_BL_STATE_WGET_NXT_FILE_1;
				}
			} else {
				wget_url_cancel();
				write_timer(&app_bootloader_wget_tmr, APP_BOOTLOADER_TIMEOUT);
				app_bootloader_state = 	APP_BL_STATE_WGET_SIG_FILE_0;
			}
			break;
		}
		case APP_BL_STATE_WGET_SIG_FILE_0 : {
			sint32 result = app_bootloader_nvm_set_signature(app_bootloader_expected_aes_string);
			if (result == -1) {//Busy
			} else if (result == 0) {//OK
				app_bootloader_state = 	APP_BL_STATE_WGET_HEX_FILE_0;
			} else {//Failed to erase
				app_bootloader_state = APP_BL_STATE_STORAGE_ERROR;
			}
			break;
		}
		case APP_BL_STATE_WGET_HEX_FILE_0 : {
			extern const char hexFileName[];
			sprintf(app_bootloader_wget_url, "%s/%s", app_bootloader_url, app_bootloader_next_hex_file_name);

			if (wget_fsio_url(app_bootloader_wget_url, (char *)hexFileName) == 0) {
				write_timer(&app_bootloader_wget_tmr, APP_BOOTLOADER_WGET_TIMEOUT);

				app_bootloader_addr = 0;

				app_bootloader_next_hex_file_name_cnt = 0;
				app_bootloader_state = 	APP_BL_STATE_WGET_HEX_FILE_1;
			} else {
				if (read_timer(&app_bootloader_wget_tmr) == 0) {
					app_bootloader_state = 	APP_BL_STATE_DONE;
				}
			}
			break;
		}
		case APP_BL_STATE_WGET_HEX_FILE_1 : {
			if (read_timer(&app_bootloader_wget_tmr) == 0) {
				wget_url_cancel();
				app_bootloader_state = 	APP_BL_STATE_DONE;
			} else {
				if (wget_fsio_finished() == 0) {
					app_bootloader_state = APP_BL_STATE_WGET_HEX_FILE_2;
				}
			}
			break;
		}
		case APP_BL_STATE_WGET_HEX_FILE_2 : {
			app_bootloader_state = APP_BL_STATE_TRIGGER_RESET;
			break;
		}
		case APP_BL_STATE_TRIGGER_RESET : {
			//mal_reset();
			break;
		}
		case APP_BL_STATE_DONE : {
			wifi_release(WIFI_USER_BOOTLOADER);
			app_bootloader_state = 	APP_BL_STATE_INIT;
			break;
		}
		default : {
			wifi_release(WIFI_USER_BOOTLOADER);
			app_bootloader_state = 	APP_BL_STATE_INIT;
			break;
		}
	}
}

void isr_app_bootloader_1ms(void) {
	do_app_bootloader_1ms = 1;
}

void app_bootloader_trigger_check(void) {
	app_bootloader_check_for_new_sw = 1;
}

unsigned int app_bootloader_check_file_name(char *str, unsigned int exp_len) {
	unsigned int result = 0;
	unsigned int x = 0;

	unsigned int cur_len = strlen(str);
	//202502021423_4D2E6F08.hsx
	if (cur_len != exp_len) {
		result++;
	} else {
		for (x = 0; x < exp_len - 4; x++) {
			if (x == 12) {
				if (str[x] != '_') {
					result++;
				}
			} else {
				if (isxdigit(str[x]) == 0) {
					result ++;
				}
			}
		}

		if (
			(str[exp_len - 4] != '.') ||
			(str[exp_len - 3] != 'h') ||
			(str[exp_len - 2] != 's') ||
			(str[exp_len - 1] != 'x') ||
			0
		) {
			result ++;
		}
	}

	return result;
}

unsigned int app_bootloader_check_nxt_entry(char *str, char *uid_exp, unsigned int serial_exp, unsigned char *signature_out, char *signature_out_string) {
	unsigned int result = 0;
	if ((str != NULL) && (uid_exp != NULL) && (signature_out != NULL) && (signature_out_string != NULL)) {
		//202502021423_4D2E6F08.hsx 4D2E6F08 DE000002 C37832B3141D8C75036FEA5F4377D4DA
		//12+        1+8+      4+ 1+8+     1+8+     1+16
		unsigned int len = strlen(str);
		if (len >= (12+1+8+4+1+8+1+8+1+16)) {
			char * tokenBuffer[8] = {0};
			int cnt = 8;
			cnt = tokenize(str, tokenBuffer, cnt, ' ', -1, -1);
			if (cnt >= 4) {
				unsigned int error = 0;
				unsigned int expected_len = strlen(app_bootloader_next_hex_file_name_format);
				tokenBuffer[1][-1] = 0;
				tokenBuffer[2][-1] = 0;
				tokenBuffer[3][-1] = 0;
				error = app_bootloader_check_file_name(tokenBuffer[0], expected_len);
				if (error == 0) {
					char *app_filename = tokenBuffer[0];
					char *app_uid = tokenBuffer[1];
					char *app_serial = tokenBuffer[2];
					char *app_signature = tokenBuffer[3];

					char tmp_date[13] = {0};
					char tmp_UID[9] = {0};
					char tmp_serial[9] = {0};
					char tmp_signature[33] = {0};

					//202502021423_4D2E6F08.hsx
					memcpy(tmp_date, &app_filename[0], 12);
					tmp_date[12] = 0;
					memcpy(tmp_UID, &app_uid[0], 8);
					tmp_UID[8] = 0;
					memcpy(tmp_serial, &app_serial[0], 8);
					tmp_serial[8] = 0;
					memcpy(tmp_signature, &app_signature[0], 32);
					tmp_signature[32] = 0;
					strcpy(signature_out_string, tmp_signature);

					//check if UID is correct
					if (strcmp(tmp_UID, uid_exp) == 0) {
						unsigned int serial_cur = hexStringToInt((unsigned char *)tmp_serial);
						if (serial_cur == serial_exp) {
							if (tokenize_hexstring_to_charvalue(tmp_signature, signature_out)) {
								rtc_LoadCompilerDefaultTime(&app_bootloader_timedate_current);
								app_bootloader_timedate_current.hour = 0;
								app_bootloader_timedate_current.min = 0;
								app_bootloader_timedate_current.sec = 0;
								app_bootloader_timedate_next.year =		(tmp_date[0] - '0') * 1000;
								app_bootloader_timedate_next.year +=	(tmp_date[1] - '0') * 100;
								app_bootloader_timedate_next.year +=	(tmp_date[2] - '0') * 10;
								app_bootloader_timedate_next.year +=	(tmp_date[3] - '0') * 1;
								app_bootloader_timedate_next.month =	(tmp_date[4] - '0') * 10;
								app_bootloader_timedate_next.month +=	(tmp_date[5] - '0') * 1;
								app_bootloader_timedate_next.day =		(tmp_date[6] - '0') * 10;
								app_bootloader_timedate_next.day +=		(tmp_date[7] - '0') * 1;
								app_bootloader_timedate_next.hour =		(tmp_date[8] - '0') * 10;
								app_bootloader_timedate_next.hour +=	(tmp_date[9] - '0') * 1;
								app_bootloader_timedate_next.min =		(tmp_date[10] - '0') * 10;
								app_bootloader_timedate_next.min +=		(tmp_date[11] - '0') * 1;

								//check if file name is newer than our current application
								if (rtc_compare_date(&app_bootloader_timedate_current, &app_bootloader_timedate_next) == -1) {
									result = 1;
								}
							}
						}
					}
				}
			}
		}
	}
	return result;
}

#endif
