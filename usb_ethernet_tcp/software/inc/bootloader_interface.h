#ifndef _BOOTLOADER_INTERFACE_H_
#define _BOOTLOADER_INTERFACE_H_

	extern void bootloader_interface_clearRequest(void);
	extern void bootloader_interface_setRequest(void);
	extern unsigned int bootloader_get_bootloader_was_reset_called(void);
	extern void bootloader_reset(void);
	
#endif
