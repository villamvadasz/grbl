#ifndef _USB_H_
#define _USB_H_

	#include "tmr.h"

	extern Timer usb_delay_timer;
        
	extern void init_usb(void);
	extern void do_usb(void);
	extern void isr_usb(void);
	extern void isr_usb_dma(void);
	extern void isr_usb_1ms(void);
	
#endif
