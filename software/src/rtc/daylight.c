#include "daylight.h"

unsigned int daylight_SavingsBegin(unsigned int year) {//March  
	unsigned int result = 0;
	result = (31 - ((5 * year / 4 + 4) % 7));
	return result;
}

unsigned int daylight_SavingsEnd(unsigned int year) {//October  
	unsigned int result = 0;
	result = (31 - ((5 * year / 4 + 1) % 7));
	return result;
}

DayLightSavingEnum daylight_isDaylightSavings(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int min, unsigned int sec) {
	DayLightSavingEnum result = DayLightGui_Winter;
	unsigned int dayLightBegin = daylight_SavingsBegin(year);
	unsigned int dayLightEnd = daylight_SavingsEnd(year);
	if (
		(
			((month == 3) && (day == dayLightBegin) && (hour >= 1)) ||
			((month == 3) && (day > dayLightBegin)) ||
			(month > 3)
		) &&
		(
			((month == 10) && (day == dayLightEnd) && (hour < 1)) ||
			((month == 10) && (day < dayLightEnd)) ||
			((month < 10))
		)
	) {
		result = DayLightGui_Summer;
	}
	return result;
}
