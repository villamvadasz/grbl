#ifndef _GRBL_TOOLCHANGE_H_
#define _GRBL_TOOLCHANGE_H_

	#include "k_stdtype.h"

	extern void init_grbl_toolchange(void);
	extern void do_grbl_toolchange(void);
	extern void isr_grbl_toolchange_1ms(void);
	
	extern void setToolChangeRunning(uint16 toolNumber, uint8 load_or_load_and_change);
	extern void updateToolChangeRunning(void);
	extern uint8 isToolChangeRunning(void); 

#endif
