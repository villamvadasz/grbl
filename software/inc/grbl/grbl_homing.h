#ifndef _GRBL_HOMING_H_
#define _GRBL_HOMING_H_

	extern void init_homing(void);
	extern void do_homing(void);
	extern void mc_homing_cycle(uint8 cycle_mask);
	extern uint8 grbl_homing_mc_homing_cycle_running(void);
	
	extern void grbl_homing_EXEC_SAFETY_DOOR_event(void);
	extern void grbl_homing_EXEC_CYCLE_STOP_event(void);

#endif
