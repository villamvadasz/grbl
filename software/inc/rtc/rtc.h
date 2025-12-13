#ifndef _RTC_H_
#define _RTC_H_

	#include <time.h>
	#include "k_stdtype.h"

	typedef struct _TimeDate {
		signed char sec;
		signed char min;
		signed char hour;
		signed char day;
		signed char month;
		signed int year;
		unsigned char dofw_calendar; //will be calculated
		unsigned char week_calendar; //will be calculated
	} TimeDate;

	extern void rtc_print_time(TimeDate *ptr);

	extern unsigned int rtc_getTime(TimeDate *ptr);//UTC
	extern unsigned int rtc_setTime(TimeDate *ptr);//UTC

	extern void rtc_LoadCompilerDefaultTime(TimeDate *ptr);

	extern void rtc_inc_year(TimeDate *ptr);
	extern void rtc_dec_year(TimeDate *ptr);
	extern void rtc_inc_month(TimeDate *ptr);
	extern void rtc_dec_month(TimeDate *ptr);
	extern void rtc_inc_day(TimeDate *ptr);
	extern void rtc_dec_day(TimeDate *ptr);
	extern void rtc_inc_hour(TimeDate *ptr);
	extern void rtc_dec_hour(TimeDate *ptr);
	extern void rtc_inc_min(TimeDate *ptr);
	extern void rtc_dec_min(TimeDate *ptr);
	extern void rtc_inc_sec(TimeDate *ptr);
	extern void rtc_dec_sec(TimeDate *ptr);

	extern void rtc_UTCtoLocalTime(TimeDate *ptr, sint8 timeZone, uint8 useDayLightSaving);
	extern void rtc_LocalTimeToUTC(TimeDate *ptr, sint8 timeZone, uint8 useDayLightSaving);
	extern void rtc_NtpToDate(uint32 ntp_seconds, TimeDate *ptr);
	extern void rtc_DateToNtp(TimeDate *ptr, uint32 *ntp_seconds);
	extern double rtc_UnixToJ2000Days(time_t unixSecs);
	extern double rtc_UnixToJulianDays(time_t unixSecs);
	extern double rtc_JulianDaysToJ2000Days(double julian);
	extern double rtc_UnixToDeg(time_t unixSecs);
	extern time_t rtc_ntp2unix(uint32 ntp_seconds);
	extern uint32 rtc_unix2ntp(time_t timeval);

	extern void rtc_update_dofw_and_week(TimeDate *ptr);

	extern unsigned char rtc_is_leap_year(unsigned int year);
	extern unsigned int rtc_get_day_in_year(unsigned int year);
	extern unsigned char rtc_get_day_in_month(unsigned int year, unsigned int month);
	
	extern int rtc_compare_date(TimeDate *ptrA, TimeDate *ptrB);//0 same -1 A>B  1 A<B

	extern void init_rtc(void);
	extern void do_rtc(void);
	extern void isr_rtc_1ms(void);
	extern void isr_rtc_1s(void);

	
#endif
