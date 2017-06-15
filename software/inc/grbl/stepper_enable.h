#ifndef _STEPPER_ENABLE_H_
#define _STEPPER_ENABLE_H_

	#define STEPPER_ENABLE_MACHINE_MOVE_STATE 0xcafeABCD
	#define STEPPER_ENABLE_MACHINE_STOP_STATE 0x12345678

	extern void init_stepper_enable(void);
	extern void do_stepper_enable(void);
	extern void isr_stepper_enable_1ms(void);

	extern uint32 get_stepper_enable_emergency_stop_active(void);
	extern void stepper_enable_emergency_stop(void);
	extern uint8 stepper_enable_emergency_stop_cleared(unsigned int auth); //auth is 3
	
	extern void stepper_enable_enable(unsigned int caller);
	extern void stepper_enable_disable(unsigned int caller);

#endif
