#include "pwm.h"
#include "c_pwm.h"

#include "mal.h"
#include "k_stdtype.h"
#include "c_main.h"

#define PWM_MAX_DUTY 0x3FF

unsigned int maxDuty = PWM_MAX_DUTY;
float pwm_duty[16];

#ifdef __32MZ2048ECG144__
	float maxFreq =(GetPeripheral3Clock() / 1.0) / ((float)(PWM_MAX_DUTY + 1)); //39062Hz 39kHz
	#define GetPeripheralClock GetPeripheral3Clock
#else
	float maxFreq =(GetPeripheralClock() / 1.0) / ((float)(PWM_MAX_DUTY + 1)); //39062Hz 39kHz
#endif

void setPWMRaw(uint16 channel, uint16 duty);

void init_pwm(void) {
	maxFreq =(GetPeripheralClock() / 1.0) / ((float)(maxDuty + 1)); //39062Hz 39kHz

	#ifdef PWM_USE_TMR3
		OC1CONbits.OCTSEL = 1; //1 - T3 0 - T2 //OC_TIMER3_SRC
	#endif
	#ifdef PWM_USE_TMR2
		OC1CONbits.OCTSEL = 0; //1 - T3 0 - T2 //OC_TIMER3_SRC
	#endif
	OC1CONbits.OC32 = 0; //OC_TIMER_MODE16
	OC1CONbits.OCM = 0x0; //disabled
	OC1R = maxDuty;
	OC1RS = maxDuty;
	OC1CONbits.ON = 0;//OC_ON

	#ifdef PWM_USE_TMR3
		OC2CONbits.OCTSEL = 1; //1 - T3 0 - T2 //OC_TIMER3_SRC
	#endif
	#ifdef PWM_USE_TMR2
		OC2CONbits.OCTSEL = 0; //1 - T3 0 - T2 //OC_TIMER3_SRC
	#endif
	OC2CONbits.OC32 = 0; //OC_TIMER_MODE16
	OC2CONbits.OCM = 0x0; //disabled
	OC2R = maxDuty;
	OC2RS = maxDuty;
	OC2CONbits.ON = 0;//OC_ON

	#ifdef PWM_USE_TMR3
		OC3CONbits.OCTSEL = 1; //1 - T3 0 - T2 //OC_TIMER3_SRC
	#endif
	#ifdef PWM_USE_TMR2
		OC3CONbits.OCTSEL = 0; //1 - T3 0 - T2 //OC_TIMER3_SRC
	#endif
	OC3CONbits.OC32 = 0; //OC_TIMER_MODE16
	OC3CONbits.OCM = 0x0; //disabled
	OC3R = maxDuty;
	OC3RS = maxDuty;
	OC3CONbits.ON = 0;//OC_ON

	#ifdef PWM_USE_TMR3
		OC4CONbits.OCTSEL = 1; //1 - T3 0 - T2 //OC_TIMER3_SRC
	#endif
	#ifdef PWM_USE_TMR2
		OC4CONbits.OCTSEL = 0; //1 - T3 0 - T2 //OC_TIMER3_SRC
	#endif
	OC4CONbits.OC32 = 0; //OC_TIMER_MODE16
	OC4CONbits.OCM = 0x0; //disabled
	OC4R = maxDuty;
	OC4RS = maxDuty;
	OC4CONbits.ON = 0;//OC_ON

	#ifdef PWM_USE_TMR3
		OC5CONbits.OCTSEL = 1; //1 - T3 0 - T2 //OC_TIMER3_SRC
	#endif
	#ifdef PWM_USE_TMR2
		OC5CONbits.OCTSEL = 0; //1 - T3 0 - T2 //OC_TIMER3_SRC
	#endif
	OC5CONbits.OC32 = 0; //OC_TIMER_MODE16
	OC5CONbits.OCM = 0x0; //disabled
	OC5R = maxDuty;
	OC5RS = maxDuty;
	OC5CONbits.ON = 0;//OC_ON

	#ifdef PWM_USE_TMR3
		T3CONbits.TCKPS = 0; //T3_PS_1_1
		//T3CONbits.T32 = 0; // 16bit mode
		T3CONbits.TCS = 0; //T3_SOURCE_INT
		TMR3 = 0;
		PR3 = maxDuty;
		T3CONbits.ON = 1;
	#endif
	#ifdef PWM_USE_TMR2
		T2CON = 0;
		T2CONbits.TCKPS = 0; //T3_PS_1_1
		T2CONbits.T32 = 0; // 16bit mode
		//T2CONbits.TCS = 0; //T3_SOURCE_INT compiler bug missing from header file
		TMR2 = 0;
		PR2 = maxDuty;
		T2CONbits.ON = 1;
	#endif
}

void do_pwm(void) {
}

void setPwmFreq(uint16 channel, float freq) {
	if ((freq >= 0.0) && (freq <= maxFreq)) {
		float ratio = maxFreq / freq;
		if ((ratio >= 0.0) && (ratio <= 1.0)) {
			#ifdef PWM_USE_TMR3
				T3CONbits.TCKPS = 0; //T3_PS_1_1
			#endif
			#ifdef PWM_USE_TMR2
				T2CONbits.TCKPS = 0; //T3_PS_1_1
			#endif
		} if ((ratio > 1.0) && (ratio <= 2.0)) {
			#ifdef PWM_USE_TMR3
				T3CONbits.TCKPS = 1; //2
			#endif
			#ifdef PWM_USE_TMR2
				T2CONbits.TCKPS = 1; //2
			#endif
		} if ((ratio > 2.0) && (ratio <= 4.0)) {
			#ifdef PWM_USE_TMR3
				T3CONbits.TCKPS = 2; //4
			#endif
			#ifdef PWM_USE_TMR2
				T2CONbits.TCKPS = 2; //4
			#endif
		} if ((ratio > 4.0) && (ratio <= 8.0)) {
			#ifdef PWM_USE_TMR3
				T3CONbits.TCKPS = 3; //8
			#endif
			#ifdef PWM_USE_TMR2
				T2CONbits.TCKPS = 3; //8
			#endif
		} if ((ratio > 8.0) && (ratio <= 16.0)) {
			#ifdef PWM_USE_TMR3
				T3CONbits.TCKPS = 4; //16
			#endif
			#ifdef PWM_USE_TMR2
				T2CONbits.TCKPS = 4; //16
			#endif
		} if ((ratio > 16.0) && (ratio <= 32.0)) {
			#ifdef PWM_USE_TMR3
				T3CONbits.TCKPS = 5; //32
			#endif
			#ifdef PWM_USE_TMR2
				T2CONbits.TCKPS = 5; //32
			#endif
		} if ((ratio > 32.0) && (ratio <= 64.0)) {
			#ifdef PWM_USE_TMR3
				T3CONbits.TCKPS = 6; //64
			#endif
			#ifdef PWM_USE_TMR2
				T2CONbits.TCKPS = 6; //64
			#endif
		} if ((ratio > 64.0) && (ratio <= 256.0)) {
			#ifdef PWM_USE_TMR3
				T3CONbits.TCKPS = 7; //256
			#endif
			#ifdef PWM_USE_TMR2
				T2CONbits.TCKPS = 7; //256
			#endif
		}
	}
}

void setPwmDuty(uint16 channel, float duty) {
	if (duty > 100.0) {
		duty = 100.0;
	} else if (duty < 0.0) {
		duty = 0.0;
	}
	pwm_duty[channel] = duty;
	//duty = 100.0 - duty;
	duty *= maxDuty;
	duty /= 100.0;
	setPWMRaw(channel, (uint16)duty);
}

float getPwmDuty(uint16 channel) {
	float result = 0.0;
	result = pwm_duty[channel];
	return result;
}


void setPWMRaw(uint16 channel, uint16 duty) {
	if (duty > maxDuty) {
		duty = maxDuty;
	}
	switch (channel) {
		case 1 : {
			if (OC1CONbits.ON == 0) {
				OC1CONbits.OCM = 0x6;
				OC1CONbits.ON = 1;
			}
			OC1RS = duty;
			break;
		}
		case 2 : {
			if (OC2CONbits.ON == 0) {
				OC2CONbits.OCM = 0x6;
				OC2CONbits.ON = 1;
			}
			OC2RS = duty;
			break;
		}
		case 3 : {
			if (OC3CONbits.ON == 0) {
				OC3CONbits.OCM = 0x6;
				OC3CONbits.ON = 1;
			}
			OC3RS = duty;
			break;
		}
		case 4 : {
			if (OC4CONbits.ON == 0) {
				OC4CONbits.OCM = 0x6;
				OC4CONbits.ON = 1;
			}
			OC4RS = duty;
			break;
		}
		case 5 : {
			if (OC5CONbits.ON == 0) {
				OC5CONbits.OCM = 0x6;
				OC5CONbits.ON = 1;
			}
			OC5RS = duty;
			break;
		}
	}
}	
