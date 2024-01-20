#ifndef _AD_H_
#define _AD_H_

	#include "k_stdtype.h"

	extern uint16 getAd(unsigned char ch);

	extern void init_ad(void);
	extern void do_ad(void);
	extern void isr_ad_1ms(void);

#endif
