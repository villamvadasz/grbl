#include "sleep.h"
#include "c_sleep.h"

#include "mal.h"
#include "k_stdtype.h"
#include "tmr.h"

#define SLEEP_MINIMUM_DELAY 3500

sint32 sleepTime = 0;
uint32 sleepRequestLock = 0;

uint32 sleepRequestApp = 0;
uint32 sleepRequestDriver = 0;
Timer sleepAppRequest_delay = 0;
Timer sleepDriverRequest_delay = 0;
uint8 sleepGetMainLoopRun_result = 1;
uint8 sleepEdgeDetector = 0;
uint8 sleepPointOfNoReturn = 0;
uint8 sleepPointOfNoReturnRegisteredRequest = 0;
uint8 sleep_executeSleep = 0;

volatile uint8 do_isr_sleep_1ms = 0;

void executeSleep(void);
void __attribute__((nomips16)) PowerSaveSleep(void);
void __attribute__((nomips16)) PowerSaveIdle(void);

void init_sleep(void) {
	sleepGetMainLoopRun_result = 1;
	initTimer(&sleepAppRequest_delay);
	initTimer(&sleepDriverRequest_delay);
}

void deinit_sleep(void) {
}

void do_sleep(void) {
	if (sleep_executeSleep) {
		executeSleep();
	}
	if (do_isr_sleep_1ms) {
		do_isr_sleep_1ms = 0;
		{
			if ((sleepGetRequestRunApp() != 0) || (sleepGetRequestRunDriver() != 0)) {
				sleepGetMainLoopRun_result = 1;
			} else if ((readTimer(&sleepAppRequest_delay) == 0) && (readTimer(&sleepDriverRequest_delay) == 0)) {
				sleepGetMainLoopRun_result = 0;
			} else {
			}
			
			if (sleepGetRequestRunApp() != 0) {
				writeTimer(&sleepAppRequest_delay, SLEEP_MINIMUM_DELAY);
				sleepEdgeDetector = 1;
			} else if (readTimer(&sleepAppRequest_delay) == 0) {
				if (sleepEdgeDetector) {
					sleepEdgeDetector = 0;
					sleepPointOfNoReturn = 1;
					sleepNotifyAppAboutSleep();//Point of no return
					sleepNotifyDriverAboutSleep();//Point of no return
				}
			} else {
			}
			
			if (sleepGetRequestRunDriver() != 0) {
				writeTimer(&sleepDriverRequest_delay, SLEEP_MINIMUM_DELAY);
			} else {
			}
		}
	}
}

void isr_sleep_1ms(void) {
	do_isr_sleep_1ms = 1;
}

//API functions
uint8 sleepSetRequestRunApp(uint32 handle) {
	uint8 result = 0;
	if (sleepPointOfNoReturn == 0) {
		if (handle < 32) {
			sleepRequestApp |= (handle << 1);
			result = 1;
		}
	}
	return result;
}

void sleepClearRequestRunApp(uint32 handle) {
	if (handle < 32) {
		sleepRequestApp &= ~(handle << 1);
	}
}

uint8 sleepGetRequestRunApp(void) {
	uint8 result = 0;
	if (sleepRequestApp != 0) {
		result = 1;
	}
	return result;
}

//Driver function
uint8 sleepSetRequestRunDriver(uint32 handle) {
	uint8 result = 0;
	if (handle < 32) {
		sleepRequestDriver |= (handle << 1);
		result = 1;
	}
	return result;
}

void sleepClearRequestRunDriver(uint32 handle) {
	if (handle < 32) {
		sleepRequestDriver &= ~(handle << 1);
	}
}

uint8 sleepGetRequestRunDriver(void) {
	uint8 result = 0;
	if (sleepRequestDriver != 0) {
		 result = 1;
	}
	return result;
}

uint8 sleepGetMainLoopRun(void) {
	uint8 result = 0;
	result = sleepGetMainLoopRun_result;
	return result;
}

void sleepExecuteSleep(void) {
	executeSleep();
}

/*Legacy sleep*/

void idle_Request(void) {
	PowerSaveIdle();
}

void sleep_Request(sint32 time) {
	if (sleepRequestLock == 0) {
		sleepRequestLock = 1;
		sleepTime = time;
		#ifndef SLEEPREQUEST_DISABLE_TASK_EXECUTE
			sleep_executeSleep = 1;
		#endif
	}
}

void executeSleep(void) {
	uint32 x = 0;
	beforeSleepUser();

	if (sleepTime < 0) { // -1
		ClearWDT();
		DisableWDT();
		ClearWDT();
		while (backToSleep()) {
			PowerSaveSleep();
		}
	} else if (sleepTime == 0) {
		ClearWDT();
		EnableWDT();
		ClearWDT();
		while (backToSleep()) {
			PowerSaveSleep();
		}
	} else {
		for (x = 0; x < sleepTime; x++) {
			ClearWDT();
			EnableWDT();
			ClearWDT();
			backToSleep(); //dummy call to notify application
			PowerSaveSleep();
			DisableWDT();
		}
	}
	afterSleepUser();
	sleepRequestLock = 0;
}
