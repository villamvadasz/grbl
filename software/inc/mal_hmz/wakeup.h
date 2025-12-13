#ifndef _WAKEUP_H_
#define _WAKEUP_H_

	extern void init_wakeup(void);
	extern void deinit_wakeup(void);
	extern void do_wakeup(void);
	extern void isr_wakeup_1ms(void);

#endif
