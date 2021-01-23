#ifndef _SLEEP_H_
#define _SLEEP_H_

	#include "k_stdtype.h"

	//API functions
	extern uint8 sleepSetRequestRunApp(uint32 handle);
	extern void sleepClearRequestRunApp(uint32 handle);
	extern uint8 sleepGetRequestRunApp(void);

	extern uint8 sleepSetRequestRunDriver(uint32 handle);
	extern void sleepClearRequestRunDriver(uint32 handle);
	extern uint8 sleepGetRequestRunDriver(void);

	extern uint8 sleepGetMainLoopRun(void);

	//Callback for drivers
	extern void sleepNotifyDriverAboutSleep(void);
	extern void sleepNotifyAppAboutSleep(void);

	extern void sleepExecuteSleep(void);

	//Low Level Functions
	extern void idle_Request(void);

	//sint32 time : -1 Wait for interrupt, no periodical wake up (interrupt)
	//sint32 time : 0 Endless polling with WDT (polling)
	//sint32 time : n Wait n times the WDT wake up (timed sleep)
	extern void sleep_Request(sint32 time);
	extern uint8 backToSleep(void); //Callout

	extern void init_sleep(void);
	extern void do_sleep(void);
	extern void isr_sleep_1ms(void);
	extern void deinit_sleep(void);

#endif
