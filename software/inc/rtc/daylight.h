#ifndef _DAYLIGHT_H_
#define _DAYLIGHT_H_

	typedef enum _DayLightSavingEnum {
		DayLightGui_Winter = 0,
		DayLightGui_Summer = 1,
		DayLightGui_Empty = 0xFF,
	} DayLightSavingEnum;

	extern unsigned int daylight_SavingsBegin(unsigned int year);
	extern unsigned int daylight_SavingsEnd(unsigned int year);
	extern DayLightSavingEnum daylight_isDaylightSavings(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int min, unsigned int sec);

#endif
