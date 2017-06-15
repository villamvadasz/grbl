#ifndef _SYSTEM_EXTERNAL_POWER_H_
#define _SYSTEM_EXTERNAL_POWER_H_

	extern void system_init_exteral_power(void);
	extern void do_system_exteral_power(void);
	extern void isr_system_exteral_power_1ms(void);

	extern unsigned int system_get_external_power_disengage_alarm(void);
	
#endif
