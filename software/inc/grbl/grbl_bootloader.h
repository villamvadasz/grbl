#ifndef _GRBL_BOOTLOADER_H_
#define _GRBL_BOOTLOADER_H_

extern void init_grbl_bootloader(void);
extern void do_grbl_bootloader(void);
extern void isr_grbl_bootloader_1ms(void);

extern void grbl_bootloader_trigger(unsigned int value);
	
#endif
