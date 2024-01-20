#ifndef _MY_PLIB_H_
#define _MY_PLIB_H_

	#include <xc.h>
	#include "int.h"

	typedef char sint8;
	typedef unsigned char uint8;
	typedef short sint16;
	typedef unsigned short uint16;
	typedef int sint32;
	typedef unsigned int uint32;
	typedef long long sint64;
	typedef unsigned long long uint64;


	typedef sint32 int32_t;
	typedef unsigned long _paddr_t; /* a physical address */
	typedef unsigned long _vaddr_t; /* a virtual address */

	#define KVA_TO_PA(v) 	((_paddr_t)(v) & 0x1fffffff)
	#define PA_TO_KVA0(pa)	((void *) ((pa) | 0x80000000))
	#define PA_TO_KVA1(pa)	((void *) ((pa) | 0xa0000000))

	extern void __attribute__((noreturn)) SoftReset(void);
	extern void __attribute__ ((nomips16)) INTEnableSystemMultiVectoredInt(void);
	extern unsigned int __attribute__((nomips16)) ReadCoreTimer(void);
	extern void __attribute__((nomips16)) WriteCoreTimer(unsigned int timer);
	extern unsigned int __attribute__((nomips16)) INTDisableInterrupts(void);
	extern void __attribute__((nomips16))  INTRestoreInterrupts(unsigned int status);

/*********************************************************************
 * Function:        int DmaSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:		None
 *
 * Output:          true if the DMA was previously suspended, false otherwise
 *
 *
 * Side Effects:    None
 *
 * Overview:        The function suspends the DMA controller.
 *
 * Note:            After the execution of this function the DMA operation is supposed to be suspended.
 *                  I.e. the function has to wait for the suspension to take place!
 *
 * Example:			int susp=DmaSuspend();
 ********************************************************************/
static __inline__ int __attribute__((always_inline)) DmaSuspend(void)
{
	int suspSt;
	if(!(suspSt=DMACONbits.SUSPEND))
	{
		DMACONSET=_DMACON_SUSPEND_MASK;		// suspend
		while(!(DMACONbits.SUSPEND));	// wait to be actually suspended
	}
	return suspSt;
}



/*********************************************************************
 * Function:        void DmaResume(int susp)
 *
 * PreCondition:    None
 *
 * Input:		the desired DMA suspended state.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		The function restores the DMA controller activity to the old suspended mode.
 *
 * Note:            None.
 *
 * Example:			int isSusp=DmaSuspend(); {....}; DmaResume(isSusp);
 ********************************************************************/
static __inline__ void __attribute__((always_inline)) DmaResume(int susp)
{
	if(susp)
	{
		DmaSuspend();
	}
	else
	{
		DMACONCLR=_DMACON_SUSPEND_MASK;		// resume DMA activity
	}
}
	
/*********************************************************************
 * Function:        void mSYSTEMUnlock(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    The system will be unlocked, the interrupts disabled and the DMA
 * 			operation suspended.
 * 			The previous interrupt and DMA status is saved in the passed parameters,
 * 			supposed to be integers.
 *
 * Note:            The first write is "garbage" to ensure that the
 *                  unlocking is correctly preformed
 ********************************************************************/
#ifdef _DMAC
	#define	mSYSTEMUnlock(intStat, dmaSusp)	do{intStat=INTDisableInterrupts(); dmaSusp=DmaSuspend(); \
						SYSKEY = 0, SYSKEY = 0xAA996655, SYSKEY = 0x556699AA;}while(0)
#else
	#define	mSYSTEMUnlock(intStat, dmaSusp)	do{intStat=INTDisableInterrupts(); \
						SYSKEY = 0, SYSKEY = 0xAA996655, SYSKEY = 0x556699AA;}while(0)
#endif	// _DMAC


/*********************************************************************
 * Function:        void mSYSTEMLock(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    The system will be locked and the status of the interrupts and the DMA restored
 * 			from the integer parameters passed.
 ********************************************************************/
#ifdef _DMAC
	#define mSYSTEMLock(intStat, dmaSusp)	do{SYSKEY = 0x33333333; DmaResume(dmaSusp); INTRestoreInterrupts(intStat);}while(0)
#else
	#define mSYSTEMLock(intStat, dmaSusp)	do{SYSKEY = 0x33333333; INTRestoreInterrupts(intStat);}while(0)
#endif // _DMAC

/*********************************************************************
 * Function:        void mSysUnlockOpLock(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    The system will be unlocked, the operation(s) passed as parameter will be executed
 * 			and the system will be locked again.
 *
 * Note:            None
 ********************************************************************/
#ifdef _DMAC
	#define	mSysUnlockOpLock(op)	do{int intStat, dmaSusp; intStat=INTDisableInterrupts(); dmaSusp=DmaSuspend(); \
				SYSKEY = 0, SYSKEY = 0xAA996655, SYSKEY = 0x556699AA; \
				(op); \
				SYSKEY = 0x33333333; \
				DmaResume(dmaSusp); INTRestoreInterrupts(intStat);}while(0)
#else
	#define	mSysUnlockOpLock(op)	do{int intStat; intStat=INTDisableInterrupts(); \
				SYSKEY = 0, SYSKEY = 0xAA996655, SYSKEY = 0x556699AA; \
				(op); \
				SYSKEY = 0x33333333; \
				INTRestoreInterrupts(intStat);}while(0)
#endif	// _DMAC
	

#endif
