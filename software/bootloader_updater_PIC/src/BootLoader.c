/*********************************************************************
 *
 *                  PIC32 Boot Loader
 *
 *********************************************************************
 * FileName:        Bootloader.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Compiler:        MPLAB C32
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * $Id:  $
 * $Name: $
 *
 **********************************************************************/
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "Bootloader.h"
#include "Framework.h"
#include <stdlib.h>
#include <xc.h>
#include "my_plib.h"

#define BOOTLOADER_TRIGGER 0xDEAD1234

typedef struct _ExceptionLog {
	unsigned int _excep_code;
	unsigned int _excep_addr;
	unsigned int magicWord;
} ExceptionLog;

#ifdef USE_GRBL_STUFFS
	#define N_AXIS 3
#endif
#define NOINITRAM_SIZE 128

#define NOINITRAM_BASE_ADDRESS_1 0xA0000300
#define NOINITRAM_CRC_SIZE 4
#define NOINITRAM_BASE_ADDRESS_2 (NOINITRAM_BASE_ADDRESS_1 + NOINITRAM_CRC_SIZE + NOINITRAM_CRC_SIZE + NOINITRAM_SIZE + 8)

#pragma GCC diagnostic error "-W"
volatile unsigned int noinitram_crc_1[2] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_1)));
volatile unsigned char noinitram_1[NOINITRAM_SIZE] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_1 + NOINITRAM_CRC_SIZE + NOINITRAM_CRC_SIZE + 8)));

volatile unsigned int noinitram_crc_2[2] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_2)));
volatile unsigned char noinitram_2[NOINITRAM_SIZE] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_2 + NOINITRAM_CRC_SIZE + NOINITRAM_CRC_SIZE + 8)));

#ifdef USE_GRBL_STUFFS
	#define GRBL_EEPROM_SYS_POSITION_ADDRESS_1 0xA0000440
	#define GRBL_EEPROM_SYS_POSITION_ADDRESS_2 0xa0000450
	#define GRBL_EEPROM_SYS_POSITION_ADDRESS_3 0xA0000460
	volatile int32_t grbl_eeprom_sys_position_1[N_AXIS + 1] __attribute__ ((persistent, address(GRBL_EEPROM_SYS_POSITION_ADDRESS_1)));
	volatile int32_t grbl_eeprom_sys_position_2[N_AXIS + 1] __attribute__ ((persistent, address(GRBL_EEPROM_SYS_POSITION_ADDRESS_2)));
	volatile int32_t grbl_eeprom_sys_position_3[N_AXIS + 1] __attribute__ ((persistent, address(GRBL_EEPROM_SYS_POSITION_ADDRESS_3)));
#endif

volatile unsigned int bootloader_request_A __attribute__ ((persistent, address(0xA0000F00)));
volatile unsigned int bootloader_request_B __attribute__ ((persistent, address(0xA0000F10)));
volatile unsigned int bootloader_was_reset_called __attribute__ ((persistent, address(0xA0000F20)));
volatile ExceptionLog exceptionLog[16] __attribute__ ((persistent, address(0xA0000200)));
#pragma GCC diagnostic pop

#ifndef BOOTLOADER_IS_AN_UPDATER_APPLICATION
	#ifdef __32MX440F256H__
		#ifdef XTAL_8MHZ
			#pragma config POSCMOD  = XT
			#pragma config UPLLEN   = ON
			#pragma config FPLLMUL  = MUL_20
			#pragma config FPLLIDIV = DIV_2			// PLL Input Divider. PLL input need 4MHz...5MHz
			#pragma config UPLLIDIV = DIV_2			// USB PLL Input Divider. PLL input need 4MHz
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#ifdef XTAL_10MHZ
			#pragma config POSCMOD  = XT
			#pragma config UPLLEN   = OFF			// Not possible to generate USB clock from 10MHz
			#pragma config FPLLMUL  = MUL_16
			#pragma config FPLLIDIV = DIV_2			// PLL Input Divider. PLL input need 4MHz...5MHz
			#pragma config UPLLIDIV = DIV_2			// USB PLL Input Divider. PLL input need 4MHz. Not possible
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#ifdef XTAL_20MHZ
			#pragma config POSCMOD  = HS
			#pragma config UPLLEN   = ON
			#pragma config FPLLMUL  = MUL_20
			#pragma config UPLLIDIV = DIV_5			// USB PLL Input Divider. PLL input need 4MHz
			#pragma config FPLLIDIV = DIV_5			// PLL Input Divider. PLL input need 4MHz...5MHz
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#if (PB_DIV == 2)
			#pragma config FPBDIV   = DIV_2
		#endif	
		#if (PB_DIV == 4)
			#pragma config FPBDIV   = DIV_4
		#endif	
		#ifdef INT_OSC_8MHZ
			#ifndef XTAL_8MHZ
				#error Internal 8MHz need 8MHz configuration (XTAL_8MHZ)
			#endif
			#pragma config FNOSC = FRCPLL
		#else 
			#pragma config FNOSC = PRIPLL
		#endif
		#pragma config FWDTEN   = OFF				// Watchdog Timer
		#pragma config WDTPS	= PS1				// Watchdog Timer Postscale
		#pragma config FCKSM	= CSECMD			// Clock Switching & Fail Safe Clock Monitor
		#pragma config OSCIOFNC = OFF				// CLKO Enable
		#pragma config IESO	    = OFF				// Internal/External Switch-over
		#pragma config FSOSCEN  = OFF				// Secondary Oscillator Enable (KLO was off)
		#pragma config CP	    = OFF				// Code Protect
		#pragma config BWP	    = OFF				// Boot Flash Write Protect
		#pragma config PWP	    = OFF				// Program Flash Write Protect
	#endif
	#ifdef __32MX470F512H__
		#ifdef XTAL_8MHZ
			#pragma config POSCMOD  = XT
			#pragma config UPLLEN   = ON
			#pragma config FPLLMUL  = MUL_20
			#pragma config FPLLIDIV = DIV_2			// PLL Input Divider. PLL input need 4MHz...5MHz
			#pragma config UPLLIDIV = DIV_2			// USB PLL Input Divider. PLL input need 4MHz
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#ifdef XTAL_10MHZ
			#pragma config POSCMOD  = XT
			#pragma config UPLLEN   = OFF			// Not possible to generate USB clock from 10MHz
			#pragma config FPLLMUL  = MUL_16
			#pragma config FPLLIDIV = DIV_2			// PLL Input Divider. PLL input need 4MHz...5MHz
			#pragma config UPLLIDIV = DIV_2			// USB PLL Input Divider. PLL input need 4MHz. Not possible
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#ifdef XTAL_20MHZ
			#pragma config POSCMOD  = HS
			#pragma config UPLLEN   = ON
			#pragma config FPLLMUL  = MUL_20
			#pragma config UPLLIDIV = DIV_5			// USB PLL Input Divider. PLL input need 4MHz
			#pragma config FPLLIDIV = DIV_5			// PLL Input Divider. PLL input need 4MHz...5MHz
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#if (PB_DIV == 2)
			#pragma config FPBDIV   = DIV_2
		#endif	
		#if (PB_DIV == 4)
			#pragma config FPBDIV   = DIV_4
		#endif	
		#ifdef INT_OSC_8MHZ
			#ifndef XTAL_8MHZ
				#error Internal 8MHz need 8MHz configuration (XTAL_8MHZ)
			#endif
			#pragma config FNOSC = FRCPLL
		#else 
			#pragma config FNOSC = PRIPLL
		#endif
		#pragma config FWDTEN   = OFF
		#pragma config WDTPS    = PS1				// Watchdog Timer Postscaler (1:1048576)
		#pragma config FCKSM    = CSECMD			// Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
		#pragma config OSCIOFNC = OFF				// CLKO Output Signal Active on the OSCO Pin (Disabled)
		#pragma config IESO     = OFF				// Internal/External Switch Over (Enabled)
		#pragma config FSOSCEN  = OFF				// Secondary Oscillator Enable (Disable SOSC)
		#pragma config CP = OFF
		#pragma config BWP = OFF
		#pragma config PWP = OFF
		#pragma config WINDIS   = OFF
		#pragma config FWDTWINSZ = WINSZ_25
		#pragma config DEBUG = OFF
		#pragma config JTAGEN = OFF
		#pragma config ICESEL = ICS_PGx2
		#pragma config FSRSSEL = PRIORITY_7			// Peripheral Module Disable Configuration (Allow only one reconfiguration)
		#pragma config PMDL1WAY = OFF				// Peripheral Module Disable Configuration (Allow only one reconfiguration)
		#pragma config IOL1WAY = OFF				// Peripheral Pin Select Configuration (Allow multiple reconfigurations)
		#pragma config FUSBIDIO = ON				// USB USBID Selection (Controlled by the USB Module)
		#pragma config FVBUSONIO = ON
	#endif
	#ifdef __32MX460F512L__
		#ifdef XTAL_8MHZ
			#pragma config POSCMOD  = XT
			#pragma config UPLLEN   = ON
			#pragma config FPLLMUL  = MUL_20
			#pragma config FPLLIDIV = DIV_2			// PLL Input Divider. PLL input need 4MHz...5MHz
			#pragma config UPLLIDIV = DIV_2			// USB PLL Input Divider. PLL input need 4MHz
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#ifdef XTAL_10MHZ
			#pragma config POSCMOD  = XT
			#pragma config UPLLEN   = OFF			// Not possible to generate USB clock from 10MHz
			#pragma config FPLLMUL  = MUL_16
			#pragma config FPLLIDIV = DIV_2			// PLL Input Divider. PLL input need 4MHz...5MHz
			#pragma config UPLLIDIV = DIV_2			// USB PLL Input Divider. PLL input need 4MHz. Not possible
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#ifdef XTAL_20MHZ
			#pragma config POSCMOD  = HS
			#pragma config UPLLEN   = ON
			#pragma config FPLLMUL  = MUL_20
			#pragma config UPLLIDIV = DIV_5			// USB PLL Input Divider. PLL input need 4MHz
			#pragma config FPLLIDIV = DIV_5			// PLL Input Divider. PLL input need 4MHz...5MHz
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#if (PB_DIV == 2)
			#pragma config FPBDIV   = DIV_2
		#endif	
		#if (PB_DIV == 4)
			#pragma config FPBDIV   = DIV_4
		#endif	
		#ifdef INT_OSC_8MHZ
			#ifndef XTAL_8MHZ
				#error Internal 8MHz need 8MHz configuration (XTAL_8MHZ)
			#endif
			#pragma config FNOSC = FRCPLL
		#else 
			#pragma config FNOSC = PRIPLL
		#endif
		#pragma config FWDTEN   = OFF				// Watchdog Timer
		#pragma config WDTPS	= PS1				// Watchdog Timer Postscale
		#pragma config FCKSM	= CSECMD			// Clock Switching & Fail Safe Clock Monitor
		#pragma config OSCIOFNC = OFF				// CLKO Enable
		#pragma config IESO	    = OFF				// Internal/External Switch-over
		#pragma config FSOSCEN  = OFF				// Secondary Oscillator Enable (KLO was off)
		#pragma config CP	    = OFF				// Code Protect
		#pragma config BWP	    = OFF				// Boot Flash Write Protect
		#pragma config PWP	    = OFF				// Program Flash Write Protect
	#endif
	#ifdef __32MX470F512L__
		#ifdef XTAL_8MHZ
			#pragma config POSCMOD  = XT
			#pragma config UPLLEN   = ON
			#pragma config FPLLMUL  = MUL_20
			#pragma config FPLLIDIV = DIV_2			// PLL Input Divider. PLL input need 4MHz...5MHz
			#pragma config UPLLIDIV = DIV_2			// USB PLL Input Divider. PLL input need 4MHz
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#ifdef XTAL_10MHZ
			#pragma config POSCMOD  = XT
			#pragma config UPLLEN   = OFF			// Not possible to generate USB clock from 10MHz
			#pragma config FPLLMUL  = MUL_16
			#pragma config FPLLIDIV = DIV_2			// PLL Input Divider. PLL input need 4MHz...5MHz
			#pragma config UPLLIDIV = DIV_2			// USB PLL Input Divider. PLL input need 4MHz. Not possible
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#ifdef XTAL_20MHZ
			#pragma config POSCMOD  = HS
			#pragma config UPLLEN   = ON
			#pragma config FPLLMUL  = MUL_20
			#pragma config UPLLIDIV = DIV_5			// USB PLL Input Divider. PLL input need 4MHz
			#pragma config FPLLIDIV = DIV_5			// PLL Input Divider. PLL input need 4MHz...5MHz
			#ifdef SYS_FREQ_10MHZ
				#pragma config FPLLODIV = DIV_8
			#endif
			#ifdef SYS_FREQ_20MHZ
				#pragma config FPLLODIV = DIV_4
			#endif
			#ifdef SYS_FREQ_40MHZ
				#pragma config FPLLODIV = DIV_2
			#endif
			#ifdef SYS_FREQ_80MHZ
				#pragma config FPLLODIV = DIV_1
			#endif
		#endif
		#if (PB_DIV == 2)
			#pragma config FPBDIV   = DIV_2
		#endif	
		#if (PB_DIV == 4)
			#pragma config FPBDIV   = DIV_4
		#endif	
		#ifdef INT_OSC_8MHZ
			#ifndef XTAL_8MHZ
				#error Internal 8MHz need 8MHz configuration (XTAL_8MHZ)
			#endif
			#pragma config FNOSC = FRCPLL
		#else 
			#pragma config FNOSC = PRIPLL
		#endif
		#pragma config FWDTEN   = OFF
		#pragma config WDTPS    = PS1				// Watchdog Timer Postscaler (1:1048576)
		#pragma config FCKSM    = CSECMD			// Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
		#pragma config OSCIOFNC = OFF				// CLKO Output Signal Active on the OSCO Pin (Disabled)
		#pragma config IESO     = OFF				// Internal/External Switch Over (Enabled)
		#pragma config FSOSCEN  = OFF				// Secondary Oscillator Enable (Disable SOSC)
		#pragma config CP = OFF
		#pragma config BWP = OFF
		#pragma config PWP = OFF
		#pragma config WINDIS   = OFF
		#pragma config FWDTWINSZ = WINSZ_25
		#pragma config DEBUG = OFF
		#pragma config JTAGEN = OFF
		#pragma config ICESEL = ICS_PGx2
		#pragma config FSRSSEL = PRIORITY_7			// Peripheral Module Disable Configuration (Allow only one reconfiguration)
		#pragma config PMDL1WAY = OFF				// Peripheral Module Disable Configuration (Allow only one reconfiguration)
		#pragma config IOL1WAY = OFF				// Peripheral Pin Select Configuration (Allow multiple reconfigurations)
		#pragma config FUSBIDIO = ON				// USB USBID Selection (Controlled by the USB Module)
		#pragma config FVBUSONIO = ON
	#endif
#endif

#define SWITCH_PRESSED 0



#ifdef __cplusplus
extern "C" {
#endif
BOOL CheckTrigger(void);
void JumpToApp(void);
BOOL ValidAppPresent(void);
#ifdef __cplusplus
}
#endif


void __attribute__((nomips16)) WriteCoreTimer(unsigned int timer)
{
    /* get the count reg */
    asm volatile("mtc0   %0, $9": "+r"(timer));

}

unsigned int __attribute__((nomips16)) ReadCoreTimer(void)
{
    unsigned int timer;

    // get the count reg
    asm volatile("mfc0   %0, $9" : "=r"(timer));

    return timer;
}


unsigned int __attribute__((nomips16))  INTEnableInterrupts(void)
{
    unsigned int status = 0;

    asm volatile("ei    %0" : "=r"(status));

    return status;
}

void __attribute__ ((nomips16)) INTEnableSystemMultiVectoredInt(void)
{
    unsigned int val;

    // set the CP0 cause IV bit high
    asm volatile("mfc0   %0,$13" : "=r"(val));
    val |= 0x00800000;
    asm volatile("mtc0   %0,$13" : "+r"(val));

    INTCONSET = _INTCON_MVEC_MASK;

    // set the CP0 status IE bit high to turn on interrupts
    INTEnableInterrupts();

}

/*********************************************************************
 * Function:        INTRestoreInterrupts(unsigned int status)
 *
 * PreCondition:    None
 *
 * Input:           value of the status registor
 *
 * Output:
 *
 * Side Effects:    Interrupts are restored to previous state
 *
 * Overview:        Interrupts are enabled by setting the IE bit
 *                  in the status register
 ********************************************************************/
void __attribute__((nomips16))  INTRestoreInterrupts(unsigned int status)
{
    if(status & 0x00000001)
        asm volatile("ei");
    else
        asm volatile("di");
}

/*********************************************************************
 * Function:        mINTDisableInterrupts()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Interrupts are disabled
 *
 * Overview:        Interrupts are disabled by clearing the IE bit
 *                  in the status register
 ********************************************************************/
unsigned int __attribute__((nomips16)) INTDisableInterrupts(void)
{
    unsigned int status = 0;

    asm volatile("di    %0" : "=r"(status));

    return status;
}

 void __attribute__((noreturn)) SoftReset(void)
{
	int	intStat;
	int	dmaSusp;
	volatile int* p = &RSWRST;

	mSYSTEMUnlock(intStat, dmaSusp);
	RSWRSTSET=_RSWRST_SWRST_MASK;
	*p;

	while(1);

}

#define PB_BUS_MAX_FREQ_HZ      80000000 //Max Peripheral bus speed
#define OSC_PB_DIV_8    (3 << _OSCCON_PBDIV_POSITION)
#define OSC_PB_DIV_4    (2 << _OSCCON_PBDIV_POSITION)
#define OSC_PB_DIV_2    (1 << _OSCCON_PBDIV_POSITION)
#define OSC_PB_DIV_1    (0 << _OSCCON_PBDIV_POSITION)

static inline void __attribute__((always_inline)) OSCSetPBDIV(unsigned int oscPbDiv)
{
    unsigned int dma_status;
    unsigned int int_status;
    __OSCCONbits_t oscBits;

    mSYSTEMUnlock(int_status, dma_status);

    oscBits.w=OSCCON;       // read to be in sync. flush any pending write
    oscBits.PBDIV=0;
    oscBits.w|=oscPbDiv;
    OSCCON=oscBits.w;       // write back
    oscBits.w=OSCCON;       // make sure the write occurred before returning from this function

    mSYSTEMLock(int_status, dma_status);
}

static inline unsigned int __attribute__((always_inline)) SYSTEMConfigPB(unsigned int sys_clock)
{
    unsigned int pb_div;
    unsigned int pb_clock;

    pb_clock = sys_clock;

    if(sys_clock > PB_BUS_MAX_FREQ_HZ)
    {
        pb_div=OSC_PB_DIV_2;
        pb_clock >>= 1;
    }
    else
    {
        pb_div=OSC_PB_DIV_1;
    }

    OSCSetPBDIV(pb_div);

    return pb_clock;
}

#define FLASH_SPEED_HZ          30000000 //Max Flash speed
#define mCheConfigure(val) (CHECON = (val))

static inline void __attribute__((always_inline)) SYSTEMConfigWaitStates(unsigned int sys_clock)
{
#ifdef _PCACHE
    unsigned int wait_states;
    unsigned int int_status;
#endif

#ifdef _PCACHE
    wait_states = 0;

    while(sys_clock > FLASH_SPEED_HZ)
    {
        wait_states++;
        sys_clock -= FLASH_SPEED_HZ;
    }

    int_status=INTDisableInterrupts();
    mCheConfigure(wait_states);
    INTRestoreInterrupts(int_status);

#endif
}

#define mBMXEnableDRMWaitState()	(BMXCONSET = _BMXCON_BMXWSDRM_MASK)
#define mBMXDisableDRMWaitState() 	(BMXCONCLR = _BMXCON_BMXWSDRM_MASK)

#define SYS_CFG_WAIT_STATES     0x00000001 //SYSTEMConfig wait states
#define SYS_CFG_PB_BUS          0x00000002 //SYSTEMConfig pb bus
#define SYS_CFG_PCACHE          0x00000004 //SYSTEMConfig cache
#define SYS_CFG_ALL             0xFFFFFFFF //SYSTEMConfig All
#define mCheGetCon() CHECON
#define CHE_CONF_PF_DISABLE  (0 << _CHECON_PREFEN_POSITION)
#define CHE_CONF_PF_C        (1 << _CHECON_PREFEN_POSITION )
#define CHE_CONF_PF_NC       (2 << _CHECON_PREFEN_POSITION)
#define CHE_CONF_PF_ALL      (3 << _CHECON_PREFEN_POSITION)
/*********************************************************************
 * Function:        void CheKseg0CacheOff(void)
 *
 * PreCondition:    None
 * Input:           None
 * Output:          none
 * Side Effects:    Sets the CCA field in the Config register of Co-
 * 					processor 0 to the value "010"b
 * Overview:        This routine is used to disable cacheability of KSEG0.
 *
 * Note:
 *
 ********************************************************************/
void __attribute__ ((nomips16)) CheKseg0CacheOff()
{
	register unsigned long tmp;

	asm("mfc0 %0,$16,0" :  "=r"(tmp));
	tmp = (tmp & ~7) | 2;
	asm("mtc0 %0,$16,0" :: "r" (tmp));
}

/*********************************************************************
 * Function:        void cheKseg0CacheOn(void)
 *
 * PreCondition:    None
 * Input:           None
 * Output:          none
 * Side Effects:    Sets the CCA field in the Config register of Co-
 * 					processor 0 to the value "011"b
 * Overview:        This routine is used to enable cacheability of KSEG0.
 *
 * Note:
 *
 ********************************************************************/
void __attribute__ ((nomips16)) CheKseg0CacheOn()
{
	register unsigned long tmp;
	asm("mfc0 %0,$16,0" :  "=r"(tmp));
	tmp = (tmp & ~7) | 3;
	asm("mtc0 %0,$16,0" :: "r" (tmp));
}

static inline unsigned int __attribute__((always_inline)) SYSTEMConfig(unsigned int sys_clock, unsigned int flags)
{
    unsigned int pb_clk;
    unsigned int int_status;
#ifdef _PCACHE
    unsigned int cache_status;
#endif

    int_status=INTDisableInterrupts();

    mBMXDisableDRMWaitState();

    if(flags & SYS_CFG_WAIT_STATES)
    {
        SYSTEMConfigWaitStates(sys_clock);
    }

    if(flags & SYS_CFG_PB_BUS)
    {
        SYSTEMConfigPB(sys_clock);
    }


#ifdef _PCACHE
    if(flags & SYS_CFG_PCACHE)
    {
        cache_status = mCheGetCon();
        cache_status |= CHE_CONF_PF_ALL;
        mCheConfigure(cache_status);
        CheKseg0CacheOn();
    }
#endif

    pb_clk = sys_clock;
    pb_clk >>= OSCCONbits.PBDIV;

    INTRestoreInterrupts(int_status);

    return pb_clk;

}

/********************************************************************
* Function: 	main()
*
* Precondition: 
*
* Input: 		None.
*
* Output:		None.
*
* Side Effects:	None.
*
* Overview: 	Main entry function. If there is a trigger or 
*				if there is no valid application, the device 
*				stays in firmware upgrade mode.
*
*			
* Note:		 	None.
********************************************************************/
#ifdef USE_GRBL_STUFFS
	#define SPINDLE_ENABLE_DDR TRISDbits.TRISD4
	#define SPINDLE_ENABLE_PORT LATDbits.LATD4
	#define SPINDLE_PWM_DDR TRISDbits.TRISD1
	#define SPINDLE_PWM_PORT LATDbits.LATD1
#endif
INT main(void)
{
	UINT pbClk;

	// Setup configuration
	pbClk = SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
	
	#ifdef USE_GRBL_STUFFS
		SPINDLE_ENABLE_PORT = 0;
		SPINDLE_ENABLE_DDR = 0; //Spindle must be disabled.
	
		SPINDLE_PWM_PORT = 1;
		SPINDLE_PWM_DDR = 0; //Spindle PWM be 10V
	#endif
	
	// Enter firmware upgrade mode if there is a trigger or if the application is not valid
	if(CheckTrigger() || !ValidAppPresent())
	{
		// Initialize the transport layer - UART/USB/Ethernet
		TRANS_LAYER_Init(pbClk);
		
		while(!FRAMEWORK_ExitFirmwareUpgradeMode()) // Be in loop till framework recieves "run application" command from PC
		{
			// Enter firmware upgrade mode.
			// Be in loop, looking for commands from PC
			TRANS_LAYER_Task(); // Run Transport layer tasks
			FRAMEWORK_FrameWorkTask(); // Run frame work related tasks (Handling Rx frame, process frame and so on)
			// Blink LED (Indicates the user that bootloader is running).
		}
		// Close trasnport layer.
		TRANS_LAYER_Close();
	}

	// No trigger + valid application = run application.
	JumpToApp();
	
	return 0;
}			


/********************************************************************
* Function: 	CheckTrigger()
*
* Precondition: 
*
* Input: 		None.
*
* Output:		TRUE: If triggered
				FALSE: No trigger
*
* Side Effects:	None.
*
* Overview: 	Checks if there is a trigger to enter 
				firmware upgrade mode.
*
*			
* Note:		 	None.
********************************************************************/
BOOL  CheckTrigger(void)
{
	BOOL result = FALSE;
	#ifdef BOOTLOADER_SWITCH_TRIGGER
		UINT SwitchStatus;
		SwitchStatus = ReadSwitchStatus();
		if(SwitchStatus == SWITCH_PRESSED)
		{
			// Switch is pressed
			result = TRUE;
		}
	#endif
	if ((bootloader_request_A == BOOTLOADER_TRIGGER) && (bootloader_request_B == ~BOOTLOADER_TRIGGER)) {
		result = TRUE;
	}
	#ifdef BOOTLOADER_DELAYED_AUTOAPPLICATION
		//Force trigger always
		result = TRUE;
	#endif
	return result;
}	


/********************************************************************
* Function: 	JumpToApp()
*
* Precondition: 
*
* Input: 		None.
*
* Output:		
*
* Side Effects:	No return from here.
*
* Overview: 	Jumps to application.
*
*			
* Note:		 	None.
********************************************************************/
void JumpToApp(void)
{	
	void (*fptr)(void);
	//Set the request explicit to be able to detect when application is not running properly and does unwanted resets
	bootloader_request_A = BOOTLOADER_TRIGGER;
	bootloader_request_B = ~BOOTLOADER_TRIGGER;
	fptr = (void (*)(void))USER_APP_RESET_ADDRESS;
	#ifdef BOOTLOADER_IS_AN_UPDATER_APPLICATION
		SoftReset();
	#else
		fptr();
	#endif
}	


/********************************************************************
* Function: 	ValidAppPresent()
*
* Precondition: 
*
* Input: 		None.
*
* Output:		TRUE: If application is valid.
*
* Side Effects:	None.
*
* Overview: 	Logic: Check application vector has 
				some value other than "0xFFFFFFFF"
*
*			
* Note:		 	None.
********************************************************************/
BOOL ValidAppPresent(void)
{
	DWORD *AppPtr;
	AppPtr = (DWORD *)USER_APP_RESET_ADDRESS;
	if(*AppPtr == 0xFFFFFFFF)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}			


/*********************End of File************************************/


