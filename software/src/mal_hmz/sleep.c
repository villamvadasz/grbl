#include "sleep.h"
#include "c_sleep.h"

#include "mal.h"
#include "k_stdtype.h"
#include "tmr.h"
#include "eep_manager.h"

#define SLEEP_MINIMUM_DELAY 100

sint32 sleepTime = 0;
uint32 sleepRequestApp = 0;
uint32 sleepRequestDriver = 0;
Timer sleepAppRequest_delay = 0;
Timer sleepDriverRequest_delay = 0;
uint8 sleepGetMainLoopRun_result = 1;
uint8 sleepEdgeDetector = 0;
uint8 sleepPointOfNoReturn = 0;
uint8 sleep_stay_in_loop_value = 0;

volatile uint32 do_isr_sleep_1ms = 0;

void executeSleep(void);
void __attribute__((nomips16)) PowerSaveSleep(void);

void init_sleep(void) {
	sleepGetMainLoopRun_result = 1;
	init_timer(&sleepAppRequest_delay);
	init_timer(&sleepDriverRequest_delay);

	sleepEdgeDetector = 1;
	write_timer(&sleepDriverRequest_delay, SLEEP_MINIMUM_DELAY);
	write_timer(&sleepAppRequest_delay, SLEEP_MINIMUM_DELAY);

	sleep_stay_in_loop_value = 1;
}

void deinit_sleep(void) {
}

void do_sleep(void) {
	if (do_isr_sleep_1ms) {
		do_isr_sleep_1ms = 0;
		{
			if ((sleep_GetRequestRunApp() != 0) || (sleep_GetRequestRunDriver() != 0)) {
				sleepGetMainLoopRun_result = 1;
			} else if ((read_timer(&sleepAppRequest_delay) == 0) && (read_timer(&sleepDriverRequest_delay) == 0)) {
				sleepGetMainLoopRun_result = 0;
			} else {
			}
			
			if (sleep_GetRequestRunApp() != 0) {
				write_timer(&sleepAppRequest_delay, SLEEP_MINIMUM_DELAY);
				sleepEdgeDetector = 1;
			} else if (read_timer(&sleepAppRequest_delay) == 0) {
				if (eep_manager_IsBusy() == 0) {
					if (sleepEdgeDetector) {
						sleepEdgeDetector = 0;
						sleepPointOfNoReturn = 1;
						sleepNotifyAppAboutSleep();//Point of no return
						sleepNotifyDriverAboutSleep();//Point of no return

						sleep_Request(0);
						sleep_stay_in_loop_value = 0;
					}
				}
			} else {
			}
			
			if (sleep_GetRequestRunDriver() != 0) {
				write_timer(&sleepDriverRequest_delay, SLEEP_MINIMUM_DELAY);
			} else {
			}
		}
	}
}

void isr_sleep_1ms(void) {
	do_isr_sleep_1ms = 1;
}

//API functions
uint8 sleep_SetRequestRunApp(uint32 handle) {
	uint8 result = 0;
	if (sleepPointOfNoReturn == 0) {
		if (handle < 32) {
			sleepRequestApp |= (1 << handle);
			result = 1;
		}
	}
	return result;
}

void sleep_ClearRequestRunApp(uint32 handle) {
	if (handle < 32) {
		sleepRequestApp &= ~(1 << handle);
	}
}

uint8 sleep_GetRequestRunApp(void) {
	uint8 result = 0;
	if (sleepRequestApp != 0) {
		result = 1;
	}
	return result;
}

//Driver function
uint8 sleep_SetRequestRunDriver(uint32 handle) {
	uint8 result = 0;
	if (handle < 32) {
		sleepRequestDriver |= (1 << handle);
		result = 1;
	}
	return result;
}

void sleep_ClearRequestRunDriver(uint32 handle) {
	if (handle < 32) {
		sleepRequestDriver &= ~(1 << handle);
	}
}

uint8 sleep_GetRequestRunDriver(void) {
	uint8 result = 0;
	if (sleepRequestDriver != 0) {
		 result = 1;
	}
	return result;
}

uint8 sleep_GetMainLoopRun(void) {
	uint8 result = 0;
	result = sleepGetMainLoopRun_result;
	return result;
}

void sleep_Request(sint32 time) {
	sleepTime = time;
}

void sleep_ExecuteSleep(void) {
	uint32 x = 0;
	beforeSleepUser();

	if (sleepTime < 0) { // -1	//Endless sleep
		ClearWDT();
		DisableWDT();
		ClearWDT();
		PowerSaveSleep();
	} else if (sleepTime == 0) {//Watchdog timed sleep
		ClearWDT();
		EnableWDT();
		ClearWDT();
		PowerSaveSleep();
	} else {
		for (x = 0; x < sleepTime; x++) {
			ClearWDT();
			EnableWDT();
			ClearWDT();
			PowerSaveSleep();
			DisableWDT();
		}
	}
	afterSleepUser();
}

void __attribute__((nomips16)) PowerSaveSleep(void) {
	#warning TODO implement
 	//mSysUnlockOpLock(OSCCONSET = (1 << _OSCCON_SLPEN_POSITION));
    //asm("WAIT");
}

unsigned int sleep_stay_in_loop(void) {
	unsigned int result = 0;
	result = sleep_stay_in_loop_value;
	return result;
}

void sleep_after_deinit(void) {
	sleep_ExecuteSleep();
	mal_reset();
}
