#ifndef _APP_H_
#define _APP_H_

	#include "k_stdtype.h"

	extern void init_app(void);
	extern void do_app(void);

	extern void isr_app_1ms(void);
	extern void isr_app_100us(void);
	extern void isr_app_custom(void);
	extern void isr_t4(void);

	extern void app_bootloader_nvm_init(void);
	extern void do_app_bootloader_nvm(void);
	extern sint32 app_bootloader_nvm_check_erased(void);
	extern sint32 app_bootloader_nvm_erase_asynch(void);
	extern sint32 app_bootloader_nvm_set_signature(char *signature_str);
	
	extern void app_bootloader_aes_start(void);
	extern void app_bootloader_aes_putchar(char ch);
	extern void app_bootloader_aes_get_cmac(unsigned char *cmac);
	
	extern void init_app_bootloader(void);
	extern void do_app_bootloader(void);
	extern void isr_app_bootloader_1ms(void);

#endif
