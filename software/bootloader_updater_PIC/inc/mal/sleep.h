#ifndef _SLEEP_H_
#define _SLEEP_H_

	#include "k_stdtype.h"

	//API functions
	extern uint8 sleep_SetRequestRunApp(uint32 handle);
	extern void sleep_ClearRequestRunApp(uint32 handle);
	extern uint8 sleep_GetRequestRunApp(void);

	extern uint8 sleep_SetRequestRunDriver(uint32 handle);
	extern void sleep_ClearRequestRunDriver(uint32 handle);
	extern uint8 sleep_GetRequestRunDriver(void);

	extern uint8 sleep_GetMainLoopRun(void);

	extern unsigned int sleep_stay_in_loop(void);
	extern void sleep_after_deinit(void);

	//Callback for drivers
	extern void sleep_NotifyDriverAboutSleep(void);
	extern void sleep_NotifyAppAboutSleep(void);

	extern void sleep_ExecuteSleep(void);

	//Low Level Functions
	extern void idle_request(void);

	//sint32 time : -1 Wait for interrupt, no periodical wake up (interrupt)
	//sint32 time : 0 Endless polling with WDT (polling)
	//sint32 time : n Wait n times the WDT wake up (timed sleep)
	extern void sleep_Request(sint32 time);

	extern void init_sleep(void);
	extern void do_sleep(void);
	extern void isr_sleep_1ms(void);
	extern void deinit_sleep(void);

	extern void beforeSleepUser(void);
	extern void afterSleepUser(void);

#endif
