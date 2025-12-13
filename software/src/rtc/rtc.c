#include <string.h>
#include <stdio.h>
#include "rtc.h"

#define MONTH_MIN 1
#define MONTH_MAX 12
#define MONTH_LEAP 0

#define DEFAULT_YEAR 2021
#define DEFAULT_MONTH 12
#define DEFAULT_DAY 4
#define DEFAULT_HOUR 17
#define DEFAULT_MIN 3
#define DEFAULT_SEC 0

unsigned char do_sec1s = 0;
TimeDate timeDate = {0,42,19,2,1,2009,0,0};
const unsigned char day_in_month [13] = {29,31,28,31,30,31,30,31,31,30,31,30,31};
char rtc_compiler_date[] = __DATE__;	//"Feb 12 1996" "Feb  2 1996"
char rtc_compiler_time[] = __TIME__;	//"23:59:01"


unsigned int rtc_isDateValid(TimeDate *ptr);

void init_rtc(void) {
	//UTC time
	TimeDate timeDate;
	timeDate.sec = 0;
	timeDate.min = 37;
	timeDate.hour = 20;
	timeDate.day = 14;
	timeDate.month = 1;
	timeDate.year = 2020;
	rtc_setTime(&timeDate);
}

void do_rtc(void) {
	if (do_sec1s == 1) {
		do_sec1s = 0;
		//inc_sec(&timeDate);
		timeDate.sec++;
		if (timeDate.sec >= 60) {
			timeDate.sec = 0;
			timeDate.min++;
			if (timeDate.min >= 60) {
				timeDate.min = 0;
				timeDate.hour ++;
				if (timeDate.hour >= 24) {
					timeDate.hour = 0;
					timeDate.day++;
					if ((rtc_get_day_in_month(timeDate.year, timeDate.month) + 1) <= timeDate.day) {
						timeDate.day = 1;
						timeDate.month++;
						if (timeDate.month > 12) {
							timeDate.month = 1;
							timeDate.year ++;
						}
					}
					rtc_update_dofw_and_week(&timeDate);
				}
			}
		}
	}
}

void isr_rtc_1ms(void) {
	static unsigned int rtc1sCnt = 0;
	rtc1sCnt ++;
	if (rtc1sCnt >= 1000) {
		rtc1sCnt = 0;
		do_sec1s = 1;
	}
}

void isr_rtc_1s(void) {
	do_sec1s = 1;
}

unsigned int rtc_getTime(TimeDate *ptr) {
	unsigned int result = 0;
	if (ptr != NULL) {
		*ptr = timeDate;
		rtc_update_dofw_and_week(ptr);
		result = 1;
	}
	return result;
}

unsigned int rtc_setTime(TimeDate *ptr) {
	unsigned int result = 0;
	if (ptr != NULL) {
		if (rtc_isDateValid(ptr) != 0) {
			timeDate = *ptr;
			rtc_update_dofw_and_week(&timeDate);
			result = 1;
		}
	}
	return result;
}

void rtc_inc_year(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->year ++;
		rtc_update_dofw_and_week(ptr);
	}
}

void rtc_dec_year(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->year --;
		rtc_update_dofw_and_week(ptr);
	}
}

void rtc_inc_month(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->month++;
		if (ptr->month > 12) {
			ptr->month = 1;
			rtc_inc_year(ptr);
		} else {
			rtc_update_dofw_and_week(ptr);
		}
	}
}

void rtc_dec_month(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->month--;
		if (ptr->month < 1) {
			rtc_dec_year(ptr);
			ptr->month = 12;
		} else {
			rtc_update_dofw_and_week(ptr);
		}
	}
}

void rtc_inc_day(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->day++;
		if (ptr->day >= (rtc_get_day_in_month(ptr->year, ptr->month)+1)) {
			ptr->day = 1;
			rtc_inc_month(ptr);
		} else {
			rtc_update_dofw_and_week(ptr);
		}
	}
}

void rtc_dec_day(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->day--;
		if (ptr->day < 1) {
			rtc_dec_month(ptr);
			ptr->day = rtc_get_day_in_month(ptr->year, ptr->month);
		} else {
			rtc_update_dofw_and_week(ptr);
		}
	}
}

void rtc_inc_hour(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->hour++;
		if (ptr->hour >= 24) {
			ptr->hour = 0;
			rtc_inc_day(ptr);
		}
	}
}

void rtc_dec_hour(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->hour--;
		if (ptr->hour < 0) {
			ptr->hour = 23;
			rtc_dec_day(ptr);
		}
	}
}

void rtc_inc_min(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->min++;
		if (ptr->min >= 60) {
			ptr->min = 0;
			rtc_inc_hour(ptr);
		}
	}
}

void rtc_dec_min(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->min--;
		if (ptr->min < 0) {
			ptr->min = 59;
			rtc_dec_hour(ptr);
		}
	}
}

void rtc_inc_sec(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->sec++;
		if (ptr->sec >= 60) {
			ptr->sec = 0;
			rtc_inc_min(ptr);
		}
	}
}

void rtc_dec_sec(TimeDate *ptr) {
	if (ptr != NULL) {
		ptr->sec--;
		if (ptr->sec < 0) {
			ptr->sec = 59;
			rtc_dec_min(ptr);
		}
	}
}

void rtc_update_dofw_and_week(TimeDate *ptr) {
	if (ptr != NULL) {
		unsigned int x_year = 1900;
		unsigned int y_month = 1;

		if (ptr->year >= 2000) {
			ptr->dofw_calendar = 5;//first day of year 2000 Szombat
			for (x_year = 2000; x_year < ptr->year; x_year++) {
				for (y_month = 1; y_month <= 12; y_month ++) {
					ptr->dofw_calendar += rtc_get_day_in_month(x_year, y_month);
					ptr->dofw_calendar %= 7;
				}
			}
		}
		{
			unsigned char i = 0;
			ptr->week_calendar = 0;
			if (ptr->dofw_calendar < 3) {
				ptr->week_calendar ++;
			}

			for (i = 1; i < ptr->month; i++) {
				ptr->dofw_calendar += rtc_get_day_in_month(ptr->year, i) % 7;
				ptr->week_calendar += rtc_get_day_in_month(ptr->year, i) / 7;
			}
			ptr->dofw_calendar += (ptr->day - 1) % 7;
			ptr->week_calendar += (ptr->day - 1) / 7;
			ptr->week_calendar += ptr->dofw_calendar / 7;
			ptr->dofw_calendar = ptr->dofw_calendar % 7;
		}
	}
}

unsigned char rtc_is_leap_year(unsigned int year) {
	unsigned char result = 0;
	if ((year % 400) == 0) {
		result = 1;
	} else {
		if ((year % 100) == 0) {
			result = 0;
		} else {
			if ((year % 4) == 0) {
				result = 1;
			} else {
				result = 0;
			}
		}
	}
	return result;
}

unsigned int rtc_get_day_in_year(unsigned int year) {
	unsigned int result = 1;
	if (rtc_is_leap_year(year)) {
		result = 366;
	} else {
		result = 365;
	}
	return result;
}

unsigned char rtc_get_day_in_month(unsigned int year, unsigned int month) {
	unsigned char result = 1;
	if ((month >= MONTH_MIN) && (month <= MONTH_MAX)) {
		if ((month == 2) && (rtc_is_leap_year(year) == 1)) {
			result = day_in_month[MONTH_LEAP];
		} else {
			result = day_in_month[month];
		}
	}
	return result;
}

unsigned int rtc_isDateValid(TimeDate *ptr) {
	unsigned int result = 0;
	if (ptr != NULL) {
		if ((ptr->sec < 60) && (ptr->sec >= 0)) {
			if ((ptr->min < 60) && (ptr->min >= 0)) {
				if ((ptr->hour < 24) && (ptr->hour >= 0)) {
					if ((ptr->month <= 12) && (ptr->month >= 0)) {
						if ((ptr->day <= rtc_get_day_in_month(ptr->year, ptr->month)) && (ptr->day >= 0)) {
							result = 1;
						}
					}
				}
			}
		}
	}
	return result;
}

void rtc_LoadCompilerDefaultTime(TimeDate *ptr) {
	if (ptr != NULL) {
		//Please set here local time
		ptr->year = DEFAULT_YEAR;
		ptr->month = DEFAULT_MONTH;
		ptr->day = DEFAULT_DAY;
		ptr->hour = DEFAULT_HOUR;
		ptr->min = DEFAULT_MIN;
		ptr->sec = DEFAULT_SEC;
		
		{
			//char gui_compiler_date[] = __DATE__;	//"Feb 12 1996" "Feb  2 1996"
			{
				unsigned int parse_year = 0;
				unsigned int parse_month = 0;
				unsigned int parse_day = 0;
				
				parse_year = (rtc_compiler_date[7] - '0') * 1000;
				parse_year += (rtc_compiler_date[8] - '0') * 100;
				parse_year += (rtc_compiler_date[9] - '0') * 10;
				parse_year += (rtc_compiler_date[10] - '0') * 1;
				
				//Jan’, ‘Feb’, ‘Mar’, ‘Apr’, ‘May’, ‘Jun’, ‘Jul’, ‘Aug’, ‘Sep’, ‘Oct’, ‘Nov’, and ‘Dec’. 
				if ((rtc_compiler_date[0] == 'J') && (rtc_compiler_date[1] == 'a') && (rtc_compiler_date[2] == 'n')) {
					parse_month = 1;
				} else if ((rtc_compiler_date[0] == 'F') && (rtc_compiler_date[1] == 'e') && (rtc_compiler_date[2] == 'b')) {
					parse_month = 2;
				} else if ((rtc_compiler_date[0] == 'M') && (rtc_compiler_date[1] == 'a') && (rtc_compiler_date[2] == 'r')) {
					parse_month = 3;
				} else if ((rtc_compiler_date[0] == 'A') && (rtc_compiler_date[1] == 'p') && (rtc_compiler_date[2] == 'r')) {
					parse_month = 4;
				} else if ((rtc_compiler_date[0] == 'M') && (rtc_compiler_date[1] == 'a') && (rtc_compiler_date[2] == 'y')) {
					parse_month = 5;
				} else if ((rtc_compiler_date[0] == 'J') && (rtc_compiler_date[1] == 'u') && (rtc_compiler_date[2] == 'n')) {
					parse_month = 6;
				} else if ((rtc_compiler_date[0] == 'J') && (rtc_compiler_date[1] == 'u') && (rtc_compiler_date[2] == 'l')) {
					parse_month = 7;
				} else if ((rtc_compiler_date[0] == 'A') && (rtc_compiler_date[1] == 'u') && (rtc_compiler_date[2] == 'g')) {
					parse_month = 8;
				} else if ((rtc_compiler_date[0] == 'S') && (rtc_compiler_date[1] == 'e') && (rtc_compiler_date[2] == 'p')) {
					parse_month = 9;
				} else if ((rtc_compiler_date[0] == 'O') && (rtc_compiler_date[1] == 'c') && (rtc_compiler_date[2] == 't')) {
					parse_month = 10;
				} else if ((rtc_compiler_date[0] == 'N') && (rtc_compiler_date[1] == 'o') && (rtc_compiler_date[2] == 'v')) {
					parse_month = 11;
				} else if ((rtc_compiler_date[0] == 'D') && (rtc_compiler_date[1] == 'e') && (rtc_compiler_date[2] == 'c')) {
					parse_month = 12;
				}
				
				if (rtc_compiler_date[4] != ' ') {
					parse_day = (rtc_compiler_date[4] - '0') * 10;
				}
				parse_day += (rtc_compiler_date[5] - '0') * 1;
	
				ptr->year = parse_year;
				ptr->month = parse_month;
				ptr->day = parse_day;
			
			}
			//char gui_compiler_time[] = __TIME__;	//"23:59:01"
			{
				unsigned int parse_hour = 0;
				unsigned int parse_min = 0;
				unsigned int parse_sec = 0;
				parse_hour = (rtc_compiler_time[0] - '0') * 10;
				parse_hour += (rtc_compiler_time[1] - '0') * 1;
				
				parse_min = (rtc_compiler_time[3] - '0') * 10;
				parse_min += (rtc_compiler_time[4] - '0') * 1;
				
				parse_sec = (rtc_compiler_time[6] - '0') * 10;
				parse_sec += (rtc_compiler_time[7] - '0') * 1;
			
				ptr->hour = parse_hour;
				ptr->min = parse_min;
				ptr->sec = parse_sec;
			}
		}
	}
}


extern void putString_usb(char *str);
__attribute__(( weak )) void putString_usb(char *str) {}

void rtc_print_time(TimeDate *ptr) {
	if (ptr != NULL) {
		char debug_text[128];
		sprintf(debug_text, "%4.4u.%2.2u.%2.2u %2.2u:%2.2u:%2.2u", ptr->year, ptr->month, ptr->day, ptr->hour, ptr->min, ptr->sec);
		putString_usb(debug_text);
	}
}

int rtc_compare_date(TimeDate *ptrA, TimeDate *ptrB) {
	//0 same -1 A>B  1 A<B
	int result = -1;
	if ((ptrA != NULL) && (ptrB != NULL)) {
		if (ptrA->year > ptrB->year) {
			result = -1;
		} else if (ptrA->year < ptrB->year) {
			result = 1;
		} else {
			if (ptrA->month > ptrB->month) {
				result = -1;
			} else if (ptrA->month < ptrB->month) {
				result = 1;
			} else {
				if (ptrA->day > ptrB->day) {
					result = -1;
				} else if (ptrA->day < ptrB->day) {
					result = 1;
				} else {
					if (ptrA->hour > ptrB->hour) {
						result = -1;
					} else if (ptrA->hour < ptrB->hour) {
						result = 1;
					} else {
						if (ptrA->min > ptrB->min) {
							result = -1;
						} else if (ptrA->min < ptrB->min) {
							result = 1;
						} else {
							if (ptrA->sec > ptrB->sec) {
								result = -1;
							} else if (ptrA->sec < ptrB->sec) {
								result = 1;
							} else {
								result = 0;
							}
						}
					}
				}
			}
		}
	}
	return result;
}
