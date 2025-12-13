#ifndef _FATFS_H
#define _FATFS_H

#include "k_stdtype.h"
#include "ff.h"

extern void init_fatfs(void);
extern void deinit_fatfs(void);
extern void do_fatfs(void);
extern void isr_fatrtc_1ms(void);
extern void isr_fatfs_1ms(void);

extern uint8 fatfs_isDiskReady(void);


#endif
