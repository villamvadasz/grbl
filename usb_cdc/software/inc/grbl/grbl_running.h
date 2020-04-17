#ifndef _GRBL_RUNNING_H_
#define _GRBL_RUNNING_H_

#include "k_stdtype.h"

extern uint32 grbl_running_running_minutes;
extern uint32 grbl_running_running_hours;
extern uint32 grbl_running_startup_cnt;
extern uint32 grbl_running_spindle_running_minutes;

extern void init_grbl_running(void);
extern void do_grbl_running(void);
extern void isr_grbl_running_1ms(void);
		
#endif
