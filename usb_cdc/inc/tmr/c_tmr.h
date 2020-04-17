#ifndef _C_TMR_H_
#define _C_TMR_H_

#error You have included an example of c_tmr.h

#define TIMERCNT 8 //number how much timers are supported
#define PRESCALE       		256
//#define TMR_USE_TMR4_1MS
//#define TMR_USE_TMR1_SOSC
//#define TMR_USE_TMR4_10US
//#define TMR_DISABLE_TMR2_100US


#define PR1_CONFIG	(SYS_FREQ/PB_DIV/PRESCALE/1000)
#define PR2_CONFIG	(SYS_FREQ/PB_DIV/PRESCALE/10000)
#define PR4_CONFIG	(SYS_FREQ/PB_DIV/100000)

#define TMR1_FUNCTIONS()		asd();\
								asd123()
#define TMR2_FUNCTIONS()		asd();\
								asd123()
#define TMR3_FUNCTIONS()		asd();\
								asd123()
#define TMR4_FUNCTIONS()		asd();\
								asd123()

#endif
