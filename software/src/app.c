#include <string.h>
#include <stdlib.h>
#include "app.h"
#include "mal.h"

#include "k_stdtype.h"
#include "sleep.h"
#include "grbl.h"
#include "fixedmemoryaddress.h"
#include "bootloader_interface.h"

typedef enum _App_States {
	App_State_Init = 0,
	App_State_Idle,
} App_States;

App_States app_State = App_State_Init;

uint8 do_app_1ms = 0;

void init_app(void) {
}

void do_app(void) {
	static unsigned char appSingleShoot = 1;
	if (appSingleShoot) {
		extern BYTE SerializedMACAddress[6];
		bootloader_write_mac(SerializedMACAddress);
	}

	if (do_app_1ms) {
		do_app_1ms = 0;
		switch (app_State) {
			case App_State_Init : {
				app_State = App_State_Idle;
				break;
			}
			case App_State_Idle : {
				break;
			}
			default : {
				break;
			}
		}
	}
}

void isr_app_1ms(void) {
	do_app_1ms = 1;
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
