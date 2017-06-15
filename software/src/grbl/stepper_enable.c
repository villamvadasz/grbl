#include "grbl.h"

#include "k_stdtype.h"

#define STEPPER_ENABLE_DELAY 50	//ms SW have this much time to slow down. Such a feature is not jet implemented

volatile uint8 do_stepper_enable_1ms = 0;

uint32 stepper_enable_emergency_stop_active = STEPPER_ENABLE_MACHINE_MOVE_STATE;
uint8 stepper_enable_delayed_enabling = 0;
uint32 stepper_enable_delayed_enabling_cnt = 0;

uint8 stepper_enable_emergency_stop_delay = 0;
uint32 stepper_enable_emergency_stop_delay_cnt = 0;

uint8 stepper_enable_abort_cnt = 0;

unsigned char stepper_logger_single_shoot = 0;
unsigned char stepper_enable_delayed_enabling_single_shoot = 1;

void init_stepper_enable(void) {
	do_stepper_enable_1ms = 0;

	stepper_enable_delayed_enabling = 0;

	stepper_enable_emergency_stop_delay = 0;
	stepper_enable_emergency_stop_delay_cnt = 0;

	stepper_enable_abort_cnt = 0;

	stepper_logger_single_shoot = 0;
	
	stepper_enable_delayed_enabling_single_shoot = 1;
	stepper_enable_delayed_enabling_cnt = 1000;
	stepper_enable_emergency_stop_active = STEPPER_ENABLE_MACHINE_MOVE_STATE;
	#ifdef STEPPERS_DISABLE_DDR
		STEPPERS_DISABLE_DDR = 0;
	#endif
	stepper_enable_disable(0); // Here we always disable the stepper motors to ensure that a cyclical reset does not enable the motor for short periods
}

void do_stepper_enable(void) {
	if (stepper_enable_delayed_enabling) {
		if (stepper_enable_delayed_enabling_single_shoot) {
			stepper_enable_delayed_enabling_single_shoot = 0;
			#ifdef STEPPERS_ALWAYS_ENABLED
				stepper_enable_enable(2);
			#else
				stepper_enable_disable(1);
			#endif
		}
	}
	{
		uint8 isr_tempVal = 0;
		lock_isr();
		if (do_stepper_enable_1ms) {
			do_stepper_enable_1ms = 0;
			isr_tempVal = 1;
		}
		unlock_isr();
		if (isr_tempVal) {
			if (system_control_get_state_ABORT() != 0) {
				if (stepper_enable_abort_cnt < 3) {
					stepper_enable_abort_cnt++;
				} else {
					//Signals Machine stops for 3 sampling
					if (get_stepper_enable_emergency_stop_active() != STEPPER_ENABLE_MACHINE_STOP_STATE) {
						stepper_enable_emergency_stop();
					}
				}
			} else {
				if (stepper_enable_abort_cnt > 0) {
					stepper_enable_abort_cnt--;
				}
			}
			
			
			if (stepper_enable_emergency_stop_active == STEPPER_ENABLE_MACHINE_STOP_STATE) {
				if (stepper_enable_emergency_stop_delay_cnt != 0) {
					stepper_enable_emergency_stop_delay_cnt --;
				} else {
					stepper_enable_emergency_stop_delay = 1;
				}
			}
			if (stepper_enable_delayed_enabling == 0) {
				if (stepper_enable_delayed_enabling_cnt != 0) {
					stepper_enable_delayed_enabling_cnt --;
				} else {
					stepper_enable_delayed_enabling = 1;
				}
			}
		}
	}

	if ((stepper_enable_emergency_stop_active == STEPPER_ENABLE_MACHINE_STOP_STATE) || (stepper_enable_emergency_stop_active == STEPPER_ENABLE_MACHINE_MOVE_STATE)) {
	} else {
		//Invalid value, force disable cyclically
		stepper_enable_disable(2);
	}
	if (stepper_enable_emergency_stop_delay) {
		stepper_enable_emergency_stop_delay = 0;
		stepper_enable_disable(3);
	}
}

void exceptions_user_callout_grbl(void) {
	stepper_enable_disable(4);
}

void isr_stepper_enable_1ms(void) {
	do_stepper_enable_1ms = 1;
}

uint32 get_stepper_enable_emergency_stop_active(void) {
	uint32 result = 0;
	result = stepper_enable_emergency_stop_active;
	return result;
}

void stepper_enable_emergency_stop(void) {
	stepper_enable_emergency_stop_active = STEPPER_ENABLE_MACHINE_STOP_STATE; //Called ON becasue Emergency Stop will be activated so that motor will be disabled

	stepper_enable_emergency_stop_delay_cnt = STEPPER_ENABLE_DELAY;
	stepper_enable_emergency_stop_delay = 0;
}

uint8 stepper_enable_emergency_stop_cleared(unsigned int auth) {
	uint8 result = 0;
	auth *= 3;
	if (auth == 9) {
		if (system_control_get_state_ABORT() == 0) {
			stepper_enable_emergency_stop_active = STEPPER_ENABLE_MACHINE_MOVE_STATE;
			#ifdef STEPPERS_ALWAYS_ENABLED
				stepper_enable_enable(3);
			#endif
			result = 1;
		}
	} else {
		stepper_enable_emergency_stop_active = 0; //It is 0 to force the main cycle do disable output cyclically
	}
	return result;
}

void stepper_enable_enable(unsigned int caller) {
	if (stepper_enable_emergency_stop_active == STEPPER_ENABLE_MACHINE_MOVE_STATE) {
		//This output is fixed, not possible to invert it in SW due to Emergency Stop functionality
		//Eeprom may be damaged then the disable would not work at all
		#ifdef STEPPERS_DISABLE_PORT
			STEPPERS_DISABLE_PORT = 0;
		#endif
		MAL_SYNC();
		if (stepper_logger_single_shoot == 0) {
			system_log_internal_stepper_enable(caller);
		}
		stepper_logger_single_shoot = 1;
	}
}

void stepper_enable_disable(unsigned int caller) {
	#ifdef STEPPERS_DISABLE_PORT
		STEPPERS_DISABLE_PORT = 1;
	#endif
	MAL_SYNC();
	if (stepper_logger_single_shoot != 0) {
		system_log_internal_stepper_disable(caller);
	}
	stepper_logger_single_shoot = 0;
}
