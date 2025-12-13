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


	#define  SPI_ENABLE             (0x00008000)            /* Enable module */
	#define  SPI_DISABLE            (0x00000000)        /* Disable module */
	
	#define  SPI_IDLE_STOP          (0x00002000)        /* Discontinue module operation in idle mode */
	#define  SPI_IDLE_CON           (0x00000000)        /* Continue module operation in idle mode */
	
	#define  SPI_RX_OVFLOW          (0x00000040)            /* receive overflow bit */
	#define  SPI_RX_OVFLOW_CLR      (0x00000000)    /* Clear receive overflow bit */
	
	#define  FRAME_POL_ACTIVE_HIGH  (0x20000000)                    /* Frame pulse active high */
	#define  FRAME_POL_ACTIVE_LOW   (0x00000000)    /* Frame pulse active low */
	
	#define FRAME_SYNC_EDGE_COINCIDE (0x00020000)                   /* frame pulse coincides with the first bit clock */
	#define FRAME_SYNC_EDGE_PRECEDE (0x00000000)    /* frame pulse precedes the first bit clock */
	
	#define FIFO_BUFFER_ENABLE      (0x00000000)                    /* no enhanced buffer functionality */
	#define FIFO_BUFFER_DISABLE (0x00000000)        /* ineffective */
	
	#define  FRAME_ENABLE_ON        (0x80000000)        /* Frame SPI support enable */
	#define  FRAME_ENABLE_OFF       (0x00000000)  /* Frame SPI support Disable */
	
	#define  FRAME_SYNC_INPUT       (0x40000000)        /* Frame sync pulse Input (slave) */
	#define  FRAME_SYNC_OUTPUT      (0x00000000) /* Frame sync pulse Output (master) */
	
	#define  ENABLE_SCK_PIN         (0x00000000)            /* SCK pin is not handled here */
	#define  DISABLE_SCK_PIN        (0x00000000)    /* SCK pin is not handled here */
	
	#define  DISABLE_SDO_PIN        (0x00001000)        /* SDO pin is not used by module */
	#define  ENABLE_SDO_PIN         (0x00000000)    /* SDO pin is  used by module */
	
	#define  SPI_MODE32_ON          (0x00000800)            /* Communication is 32 bits wide */
	#define  SPI_MODE32_OFF         (0x00000000)        /* Communication is selected by SPI_MODE16_ON/OFF */
	
	#define  SPI_MODE16_ON          (0x00000400)            /* If SPI_MODE32_OFF, Communication is 16 bits wide */
	#define  SPI_MODE16_OFF         (0x00000000)            /* If SPI_MODE32_OFF, Communication is 8 bits wide */
	
	#define  SPI_MODE8_ON           (0x00000000)            /* If SPI_MODE32_OFF and SPI_MODE16_OFF, Communication is 8 bits wide */
	
	#define  SPI_SMP_ON             (0x00000200)        /* Input data sampled at end of data output time */
	#define  SPI_SMP_OFF            (0x00000000)    /* Input data sampled at middle of data output time */
	
	#define  SPI_CKE_ON             (0x00000100)        /* Transmit happens from active clock state to idle clock state*/
	#define  SPI_CKE_OFF            (0x00000000)    /* Transmit happens on transition from idle clock state to active clock state */
	
	#define  SLAVE_ENABLE_ON        (0x00000080)            /* Slave Select enbale */
	#define  SLAVE_ENABLE_OFF       (0x00000000)    /* Slave Select not used by module */
	
	#define  CLK_POL_ACTIVE_LOW     (0x00000040)                /* Idle state for clock is high, active is low */
	#define  CLK_POL_ACTIVE_HIGH    (0x00000000)    /* Idle state for clock is low, active is high */
	
	#define  MASTER_ENABLE_ON       (0x00000020)            /* Master Mode */
	#define  MASTER_ENABLE_OFF      (0x00000000)    /* Slave Mode */
	
	#define  SEC_PRESCAL_1_1        (0x0000001c)        /* Secondary Prescale 1:1   */
	#define  SEC_PRESCAL_2_1        (0x00000018)        /* Secondary Prescale 2:1   */
	#define  SEC_PRESCAL_3_1        (0x00000014)        /* Secondary Prescale 3:1   */
	#define  SEC_PRESCAL_4_1        (0x00000010)        /* Secondary Prescale 4:1   */
	#define  SEC_PRESCAL_5_1        (0x0000000c)        /* Secondary Prescale 5:1   */
	#define  SEC_PRESCAL_6_1        (0x00000008)        /* Secondary Prescale 6:1   */
	#define  SEC_PRESCAL_7_1        (0x00000004)        /* Secondary Prescale 7:1   */
	#define  SEC_PRESCAL_8_1        (0x00000000)        /* Secondary Prescale 8:1   */
	
	#define  PRI_PRESCAL_1_1        (0x00000003)        /* Primary Prescale 1:1     */
	#define  PRI_PRESCAL_4_1        (0x00000002)        /* Primary Prescale 4:1     */
	#define  PRI_PRESCAL_16_1       (0x00000001)        /* Primary Prescale 16:1    */
	#define  PRI_PRESCAL_64_1       (0x00000000)        /* Primary Prescale 64:1    */
	
    void    OpenSPI1(unsigned int config1, unsigned int config2 );
    void    OpenSPI2(unsigned int config1, unsigned int config2 );
	#define putcSPI1(data_out)  do{while(!SPI1STATbits.SPITBE); SPI1BUF=(data_out); MAL_SYNC();}while(0)
	#define putcSPI2(data_out)  do{while(!SPI2STATbits.SPITBE); SPI2BUF=(data_out); MAL_SYNC(); }while(0)
    unsigned int getcSPI1(void);
    unsigned int    getcSPI2(void);

#endif
