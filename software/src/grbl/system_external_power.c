#include "grbl.h"
#include "k_stdtype.h"

#define SYSTEM_EXTERNAL_POWER_LIMIT 500

volatile unsigned int do_system_exteral_power_1ms = 0;
unsigned int system_external_power_counter = SYSTEM_EXTERNAL_POWER_LIMIT;
unsigned char system_external_power_alarm_state = 1;

void system_init_exteral_power(void) {
	#ifdef EXTERNAL_POWER_TRIS
		EXTERNAL_POWER_TRIS = 1;
	#endif
	#ifdef EXTERNAL_POWER_PORT_PUE
		EXTERNAL_POWER_PORT_PUE = 1;
	#endif
	#ifdef EXTERNAL_POWER_PORT_CN
		#warning TODO Feature is not implemented yet for external power
		EXTERNAL_POWER_PORT_CN = 1;
	#endif
	system_external_power_counter = SYSTEM_EXTERNAL_POWER_LIMIT;
	system_external_power_alarm_state = 1;
	system_set_exec_alarm(EXEC_ALARM_EXTERNAL_POWER_MISSING);
}

void do_system_exteral_power(void) {
	if (do_system_exteral_power_1ms) {
		do_system_exteral_power_1ms = 0;
		{
			if (system_get_external_power() == 0) {
				if (system_external_power_counter < SYSTEM_EXTERNAL_POWER_LIMIT) {
					system_external_power_counter++;
				}
			} else {
				if (system_external_power_counter != 0) {
					system_external_power_counter--;
				}
			}
			if (system_external_power_counter >= SYSTEM_EXTERNAL_POWER_LIMIT) {
				if (system_external_power_alarm_state == 0) {
					system_set_exec_alarm(EXEC_ALARM_EXTERNAL_POWER_MISSING);
				}
				system_external_power_alarm_state = 1;
			} else if (system_external_power_counter == 0) {
				system_external_power_alarm_state = 0;
			}
		}
	}
}

void isr_system_exteral_power_1ms(void) {
	do_system_exteral_power_1ms = 1;
}

unsigned int system_get_external_power(void) {
	unsigned int result = 0;
	#ifdef EXTERNAL_POWER_PORT
		if (EXTERNAL_POWER_PORT != 0) {
			result = 0;
		} else {
			result = 1;
		}
	#else
		result = 1;//If PIN is not available, we assume that we have always external power
	#endif
	return result;
}

unsigned int system_get_external_power_disengage_alarm(void) {
	unsigned int result = 0;
	if (system_external_power_alarm_state == 0) {
		result = 1;
	}
	return result;
}
