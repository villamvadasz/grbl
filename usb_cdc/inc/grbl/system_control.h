#ifndef _SYSTEM_CONTROL_H_
#define _SYSTEM_CONTROL_H_

	extern void init_system_control(void);
	extern void do_system_control(void);
	extern void isr_system_control_1ms(void);
	extern void CONTROL_INT_vect(void);
	// Returns bitfield of control pin states, organized by CONTROL_PIN_INDEX. (1=triggered, 0=not triggered).
	extern uint8 system_control_get_state(void);

#endif
