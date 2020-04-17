#ifndef _C_SLEEP_H_
#define _C_SLEEP_H_

	#error You have included an example of c_sleep.h
	
	#define sleepNotifyDriverAboutSleep()	;
	#define sleepNotifyAppAboutSleep()	;
	
	#define beforeSleepUser()	;
	#define afterSleepUser()	;

	typedef enum _SleepAppHandle {
		SleepAppHandle_System = 0,
		SleepAppHandle_ESP8266,
	} SleepAppHandle;

	typedef enum _SleepDriverHandle {
		SleepDriverHandle_System = 0,
	} SleepDriverHandle;
	
#endif
