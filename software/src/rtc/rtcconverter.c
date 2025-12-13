#include <string.h>
#include <time.h>
#include "k_stdtype.h"
#include "rtc.h"
#include "daylight.h"

#define OFFSET 2208988800ULL

void rtc_UTCtoLocalTime(TimeDate *ptr, sint8 timeZone, uint8 useDayLightSaving) {
	if (ptr != NULL) {
		//Substracting timezone
		uint16 i = 0;
		uint16 cnt = 0;
		uint8 decrementTime = 0;
		if (timeZone > 0) {
			decrementTime = 1;
			cnt = timeZone;
		} else if (timeZone < 0) {
			decrementTime = 0;
			cnt = -timeZone;
		}
		for (i = 0; i < cnt; i++) {
			if (decrementTime) {
				rtc_inc_hour(ptr);
			} else {
				rtc_dec_hour(ptr);
			}
		}
		if (useDayLightSaving) {
			//Substracting daylight saving
			if (daylight_isDaylightSavings(ptr->year, ptr->month, ptr->day, ptr->hour, ptr->min, ptr->sec) == DayLightGui_Summer) {
				rtc_inc_hour(ptr);
			}
		}
	}
}

void rtc_LocalTimeToUTC(TimeDate *ptr, sint8 timeZone, uint8 useDayLightSaving) {
	if (ptr != NULL) {
		//Substracting timezone
		uint16 i = 0;
		uint16 cnt = 0;
		uint8 decrementTime = 0;
		if (timeZone > 0) {
			decrementTime = 1;
			cnt = timeZone;
		} else if (timeZone < 0) {
			decrementTime = 0;
			cnt = -timeZone;
		}
		for (i = 0; i < cnt; i++) {
			if (decrementTime) {
				rtc_dec_hour(ptr);
			} else {
				rtc_inc_hour(ptr);
			}
		}
		if (useDayLightSaving) {
			//Substracting daylight saving
			if (daylight_isDaylightSavings(ptr->year, ptr->month, ptr->day, ptr->hour, ptr->min, ptr->sec) == DayLightGui_Summer) {
				rtc_dec_hour(ptr);
			}
		}
	}
}

void rtc_NtpToDate(uint32 ntp_seconds, TimeDate *ptr) {
	if (ptr != NULL) {
		uint32 days = 0;
		
		days = ntp_seconds / 86400;
		ntp_seconds -= (days * 86400);
		ptr->hour = ntp_seconds / 3600;
		ntp_seconds -= (ptr->hour * 3600);
		ptr->min = ntp_seconds / 60;
		ntp_seconds -= (ptr->min * 60);
		ptr->sec = ntp_seconds;
		
		ptr->year = 1970;
		while (1) {
			unsigned int days_in_year = rtc_get_day_in_year(ptr->year);
			if (days_in_year <= days) {
				ptr->year++;
				days -= days_in_year;
			} else {
				break;
			}
		}

		ptr->month = 1;
		while (1) {
			unsigned char days_in_month = rtc_get_day_in_month(ptr->year, ptr->month);
			if (days_in_month <= days) {
				ptr->month++;
				days -= days_in_month;
			} else {
				break;
			}
		}
		ptr->day = 1;
		ptr->day += days;
		rtc_update_dofw_and_week(ptr);
		
	}
}

void rtc_DateToNtp(TimeDate *ptr, uint32 *ntp_seconds) {
	if ((ptr != NULL) && (ntp_seconds != NULL)) {
		uint32 result = 0;
		uint32 x = 0;
		for ( x = 1970; x < (ptr->year); x++) {
			unsigned int days_in_year = rtc_get_day_in_year(x);
			result += days_in_year * (24uL*60uL*60uL);
		}
		
		for ( x = 1; x < (ptr->month); x++) {
			unsigned char days_in_month = rtc_get_day_in_month(ptr->year, x);
			result += days_in_month * (24uL*60uL*60uL);
		}
		
		result += (ptr->day - 1) * (24uL*60uL*60uL);
		result += ptr->hour * (60uL * 60uL);
		result += ptr->min * (60uL);
		result += ptr->sec;


		*ntp_seconds = result;
	}
}

double rtc_UnixToJ2000Days(time_t unixSecs) {
	double result = 0.0;
	result = rtc_UnixToJulianDays(unixSecs);
	result = rtc_JulianDaysToJ2000Days(result);
	return result;
}

double rtc_UnixToJulianDays(time_t unixSecs) {
	double result = 0.0;
	result = ( unixSecs / 86400.0 ) + 2440587.5;
	return result;
}

double rtc_JulianDaysToJ2000Days(double julian) {
	double result = 0.0;
	result = julian - 2451545.0;
	return result;
}

double rtc_UnixToDeg(time_t unixSecs) {
	double result = 0.0;
	
	int sec = (unixSecs % (24uL * 60uL * 60uL));
	result = sec;
	result /= (60.0 * 60.0);
	result *= 15.0;
	
	return result;
}

time_t rtc_ntp2unix(uint32 ntp_seconds) {
	time_t result = 0;
    result = ntp_seconds - OFFSET;

	return result;
}

uint32 rtc_unix2ntp(time_t timeval)
{
	uint32 result = 0;
	result = timeval + OFFSET;
	return result;

}
