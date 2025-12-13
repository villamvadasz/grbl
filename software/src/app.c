#include <string.h>
#include <stdlib.h>
#include "app.h"
#include "app_mqtt.h"
#include "app_rfe.h"
#include "mal.h"

#include "k_stdtype.h"
#include "sleep.h"
#include "grbl.h"
#include "fixedmemoryaddress.h"
#include "bootloader_interface.h"
#include "rtc.h"
#include "aes_128.h"
#include "version.h"
#include "random.h"

typedef enum _App_States {
	App_State_Init = 0,
	App_State_Idle,
} App_States;

App_States app_State = App_State_Init;

uint8 do_app_1ms = 0;
uint8 app_sntp_user_called = 0;
uint32 app_dwSNTPSeconds = 0;
TimeDate app_timedate;
uint32 do_app_1min = 0;

__attribute__(( weak )) void init_app_mqtt(void) {} 
__attribute__(( weak )) void do_app_mqtt(void) {} 
__attribute__(( weak )) void isr_app_mqtt_1ms(void) {} 

__attribute__(( weak )) void init_app_rfe(void) {} 
__attribute__(( weak )) void do_app_rfe(void) {} 
__attribute__(( weak )) void isr_app_rfe_1ms(void) {} 

__attribute__(( weak )) void init_app_bootloader(void) {}
__attribute__(( weak )) void do_app_bootloader(void) {}
__attribute__(( weak )) void isr_app_bootloader_1ms(void) {}

__attribute__(( weak )) int SetClockVars (unsigned int year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second) {return 0;}

void doAppUpdateFSIOClock(void);

void init_app(void) {
	init_app_mqtt();
	init_app_rfe();
	init_app_bootloader();
	
	rtc_LoadCompilerDefaultTime(&app_timedate);
	rtc_setTime(&app_timedate);
	
}

void do_app(void) {
	static unsigned char appSingleShoot = 1;
	do_app_bootloader();
	if (appSingleShoot) {
		extern BYTE SerializedMACAddress[6];
		bootloader_write_mac(SerializedMACAddress);
		appSingleShoot = 0;
	}

	do_app_mqtt();
	do_app_rfe();
	

	if (do_app_1min) {
		do_app_1min = 0;
		rtc_getTime(&app_timedate);
		doAppUpdateFSIOClock();
	}

	if (do_app_1ms) {
		do_app_1ms = 0;
		switch (app_State) {
			case App_State_Init : {
				app_State = App_State_Idle;
				break;
			}
			case App_State_Idle : {
				//uint32 value = random_getValue();
				break;
			}
			default : {
				break;
			}
		}
	}
}

void isr_app_1ms(void) {
	static unsigned int do_app_1min_cnt = 0;
	do_app_1ms = 1;
	do_app_1min_cnt++;
	if (do_app_1min_cnt >= 60000) {
		do_app_1min_cnt = 0;
		do_app_1min = 1;
	}
	isr_app_mqtt_1ms();
	isr_app_rfe_1ms();
	isr_app_bootloader_1ms();
}

void isr_app_100us(void) {
}

void isr_app_custom(void) {
}

uint8 backToSleep(void) {
	uint8 result = 0;
	return result;
}

void eepManager_NotifyUserFailedRead(int item, uint8 type) {
	grbl_NotifyUserFailedRead(item, type);
}

unsigned int app_get_noinitram_tester(void) {
	return 0;
}

void sntp_user_callout(uint32 dwSNTPSeconds) {
	uint32 ntp_seconds = dwSNTPSeconds;
	rtc_NtpToDate(ntp_seconds, &app_timedate);
	rtc_setTime(&app_timedate);

	app_sntp_user_called++;
	app_dwSNTPSeconds = dwSNTPSeconds;
}

void aes_LoadROMKey(unsigned char * rom_key) {
	if (rom_key != NULL) {
		memcpy(rom_key, VERSION_APP_KEY_ROM, 16);
	}
}

void doAppUpdateFSIOClock(void) {
	SetClockVars (app_timedate.year, app_timedate.month, app_timedate.day, app_timedate.hour, app_timedate.min, app_timedate.sec);
}
