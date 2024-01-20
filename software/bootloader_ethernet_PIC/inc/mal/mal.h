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
		ResetReason_Software_reset_during_normal_operation_bootloader,
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
	#define set_isr(reg_name, if_flag_offset)	(reg_name = (1 << if_flag_offset))

	extern int mal_DmaSuspend(void);
	extern void mal_DmaResume(int susp);


	#define	mSYSTEMUnlock(intStat, dmaSusp)	do{lock_isr(); dmaSusp=mal_DmaSuspend(); SYSKEY = 0, SYSKEY = 0xAA996655, SYSKEY = 0x556699AA;}while(0)

	#define mSYSTEMLock(intStat, dmaSusp)	do{SYSKEY = 0x33333333; mal_DmaResume(dmaSusp); unlock_isr();}while(0)

	#define	mSysUnlockOpLock(op)	do{int dmaSusp; lock_isr(); dmaSusp=mal_DmaSuspend(); SYSKEY = 0, SYSKEY = 0xAA996655, SYSKEY = 0x556699AA; (op); SYSKEY = 0x33333333; mal_DmaResume(dmaSusp); unlock_isr();}while(0)

	extern void mal_reset(void);
	extern ResetReason mal_get_reset_reason(unsigned int * rcon);

	extern void init_mal(void);

	extern unsigned char hexStringToChar(unsigned char *str);
	extern unsigned int hexStringToInt(unsigned char *str);
	extern unsigned int mal_get_reset_counter(void);

#endif
