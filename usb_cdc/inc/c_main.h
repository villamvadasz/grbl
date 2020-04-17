#ifndef _C_MAIN_H_
#define _C_MAIN_H_

#define XTAL_8MHZ
//#define XTAL_10MHZ
//#define XTAL_20MHZ
//#define INT_OSC_8MHZ

#define SYS_FREQ 			(80000000L)
#define PB_DIV         		2

#define GetSystemClock()		SYS_FREQ			// Hz
#define GetInstructionClock()	(GetSystemClock()/1)	// 
#define GetPeripheralClock()	(GetSystemClock()/PB_DIV)	// Divisor is dependent on the value of FPBDIV set(configuration bits).

#endif
