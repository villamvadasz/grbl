#ifndef _MAL_H_
#define _MAL_H_

#include <sys/attribs.h>
#include <sys/kmem.h>
#include <p32xxxx.h>
#include <xc.h>

	typedef enum _ResetReason {
		ResetReason_Unknown = 0,
		ResetReason_Power_on_Reset,
		ResetReason_Brown_out_Reset,
		ResetReason_MCLR_reset_during_normal_operation,
		ResetReason_Software_reset_during_normal_operation,
		ResetReason_MCLR_reset_during_sleep,
		ResetReason_MCLR_reset_during_idle,
		ResetReason_WDT_Time_out_reset,
		ResetReason_Configuration_Word_Mismatch_Reset,
	} ResetReason;

	extern volatile unsigned int gieTemp;
	extern volatile unsigned int isrLockCnt;

	#define MAL_NOP() Nop()
	#define ClearWDT() (WDTCONSET = _WDTCON_WDTCLR_MASK)
	#define DisableWDT() (WDTCONCLR = _WDTCON_ON_MASK)
	#define EnableWDT() (WDTCONSET = _WDTCON_ON_MASK)
	#define MAL_SYNC() _sync()

	#define lock_isr() 	{isrLockCnt++; asm volatile("di    %0" : "=r"(gieTemp));}
	#define unlock_isr() {if(gieTemp & 0x00000001) {asm volatile("ei");}else{asm volatile("di");} isrLockCnt--;}
	#define clear_isr(reg_name, if_flag_offset)	(reg_name = (1 << if_flag_offset))

	extern void mal_reset(void);
	extern ResetReason mal_reset_reason(void);


#endif
