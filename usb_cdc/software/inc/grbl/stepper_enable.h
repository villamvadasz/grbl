#ifndef _STEPPER_ENABLE_H_
#define _STEPPER_ENABLE_H_

	extern void init_stepper_enable(void);
	extern void do_stepper_enable(void);
	extern void isr_stepper_enable_1ms(void);

	extern void stepper_enable_emergency_stop(void);
	extern uint8 stepper_enable_emergency_stop_cleared(unsigned int auth); //auth is 3
	
	extern void stepper_enable_enable(void);
	extern void stepper_enable_disable(void);

#endif
