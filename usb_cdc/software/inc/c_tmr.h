#ifndef _C_TMR_H_
#define _C_TMR_H_

#include "c_main.h"
#include "main.h"

#define TIMERCNT 8 //number how much timers are supported
#define PRESCALE       		256
#define TMR_DISABLE_TMR2_100US

#ifndef SYS_FREQ
	#error SYS_FREQ not defined
#endif
#ifndef PB_DIV
	#error PB_DIV not defined
#endif
#ifndef PRESCALE
	#error PRESCALE not defined
#endif

#define PR1_CONFIG	(SYS_FREQ/PB_DIV/PRESCALE/1000)
#define PR2_CONFIG	(SYS_FREQ/PB_DIV/PRESCALE/10000)

#define TMR1_FUNCTIONS()	isr_main_1ms();
#define TMR2_FUNCTIONS()	isr_main_100us();
#define TMR3_FUNCTIONS()		;
#define TMR4_FUNCTIONS()		;

#endif
