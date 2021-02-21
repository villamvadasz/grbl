#include <string.h>
#include <stdlib.h>
#include "app.h"
#include "mal.h"

#include "k_stdtype.h"
#include "sleep.h"
#include "grbl.h"

void init_app(void) {
}

void do_app(void) {
	static unsigned char appSingleShoot = 1;
	if (appSingleShoot) {
		appSingleShoot = 0;
	}
}

void isr_app_1ms(void) {
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
