#ifndef _TMR_H_
#define _TMR_H_

	#include "k_stdtype.h"

	typedef uint32 Timer;

	extern void delayms(uint32 dt);

	extern void init_timer(Timer *ptr);
	extern void add_timer(Timer *ptr);
	extern void remove_timer(Timer *ptr);
	extern uint32 read_timer(const Timer *ptr);
	extern void write_timer(Timer *ptr, uint32 newValue);

	extern void setTimer (uint32 d);
	extern uint32 getTimer (void);
	extern uint32 getGlobalTime(void);
	extern uint32 getGlobalTimeUs(void);
	extern uint32 getGlobalTimeUs_Lora(void);

	extern void init_tmr(void);
	extern void deinit_tmr(void);
	extern void do_tmr(void);
	extern void isr_tmr1(void);
	extern void isr_tmr2(void);
	extern void isr_tmr3(void);
	extern void isr_tmr4(void);

	
#endif
