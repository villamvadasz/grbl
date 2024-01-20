#ifndef _PWM_H_
#define _PWM_H_

	#include "k_stdtype.h"

	extern void setPwmFreq(uint16 channel, float freq);
	extern void setPwmDuty(uint16 channel, float duty);
	extern float getPwmDuty(uint16 channel);

	extern void init_pwm(void);
	extern void do_pwm(void);

#endif
