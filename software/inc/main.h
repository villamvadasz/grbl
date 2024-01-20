#ifndef _MAIN_H_
#define _MAIN_H_

	#include "version.h"

	extern unsigned long loopCnt;
	extern unsigned int tick_count;
	extern SoftwareIdentification softwareIdentification;

	extern void isr_main_1ms(void);
	extern void isr_main_100us(void);
	extern void isr_main_custom(void);

#endif
