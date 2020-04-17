#ifndef _DEE_H_
#define _DEE_H_

#include "k_stdtype.h"

extern void dee_write_eeprom(uint32 adr, uint32 data);
extern uint32 dee_read_eeprom(uint32 adr);

extern void init_dee(void);
extern void deinit_dee(void);
extern void do_dee(void);
extern void isr_dee_1ms(void);


#endif
