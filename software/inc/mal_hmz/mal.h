#ifndef _MAL_H_
#define _MAL_H_

	#include <sys/attribs.h>
	#include <sys/kmem.h>
	#include <p32xxxx.h>
	#include <xc.h>
	
	typedef unsigned long _paddr_t; /* a physical address */
	typedef unsigned long _vaddr_t; /* a virtual address */

	/* 
	 * Translate a kernel virtual address in KSEG0 or KSEG1 to a real
	 * physical address and back.
	 */
	#define KVA_TO_PA(v) 	((_paddr_t)(v) & 0x1fffffff)
	#define PA_TO_KVA0(pa)	((void *) ((pa) | 0x80000000))
	#define PA_TO_KVA1(pa)	((void *) ((pa) | 0xa0000000))

	/* translate between KSEG0 and KSEG1 virtual addresses */
	#define KVA0_TO_KVA1(v)	((void *) ((unsigned)(v) | 0x20000000))
	#define KVA1_TO_KVA0(v)	((void *) ((unsigned)(v) & ~0x20000000))

	/* Test for KSEGS */
	#define IS_KVA(v)	((int)(v) < 0)
	#define IS_KVA0(v)	(((unsigned)(v) >> 29) == 0x4)
	#define IS_KVA1(v)	(((unsigned)(v) >> 29) == 0x5)
	#define IS_KVA01(v)	(((unsigned)(v) >> 30) == 0x2)


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
	#define ClearWDT() MAL_NOP()
	#define DisableWDT() MAL_NOP()
	#define EnableWDT() MAL_NOP()
	#define MAL_SYNC() _sync()

	#define lock_isr() 	{isrLockCnt++; gieTemp = __builtin_get_isr_state(); __builtin_disable_interrupts();}
	#define unlock_isr() { __builtin_set_isr_state(gieTemp); isrLockCnt--;}
	#define clear_isr(reg_name, if_flag_offset)	(reg_name = (1 << if_flag_offset))

	extern int DmaSuspend(void);
	extern void DmaResume(int susp);
	
	extern void mal_reset(void);
	extern ResetReason mal_get_reset_reason(unsigned int * rcon);

	extern void init_mal(void);

	extern unsigned char hexStringToChar(unsigned char *str);
	extern unsigned int hexStringToInt(unsigned char *str);
	extern unsigned int mal_get_reset_counter(void);

#endif
