#include "main.h"
#include "c_main.h"

#include "k_stdtype.h"
#include "mal.h"
#include "sleep.h"
#include "tmr.h"
#include "stackmeasure.h"
#include "bootloader_interface.h"

#include "c_app.h"
#include "c_task.h"

#include "ad.h"
#include "app.h"
#include "eep_manager.h"
#include "isr.h"
#include "serial_usb.h"
#include "task.h"
#include "usb.h"
#include "grbl.h"
#include "dee.h"
#include "pwm.h"

#define SW_YEAR 0x2020
#define SW_MONTH 0x03
#define SW_DAY 0x07
#define SW_TYPE TYPE_GRBL
#define SW_VERSION 0x0102

SoftwareIdentification softwareIdentification = {SW_YEAR, SW_MONTH, SW_DAY, SW_VERSION, SW_TYPE};
unsigned char VERSION_ID[] = "cnc_1_0_0";
const char VERSION_DATE[]  __attribute__ ((address(0x9D008100))) = __DATE__;
const char VERSION_TIME[]  __attribute__ ((address(0x9D008120))) = __TIME__;
const char VERSION_ID_FIX[]  __attribute__ ((address(0x9D008140))) = "cnc_1_0_0";

#ifdef __DEBUG
	#ifndef _STANDALONE_APPLICATION
		#define _STANDALONE_APPLICATION
	#endif
#endif

#ifdef _STANDALONE_APPLICATION
	#ifdef __32MX440F256H__
        #pragma config UPLLEN   = ON        // USB PLL Enabled
        #pragma config FPLLMUL  = MUL_20        // PLL Multiplier
        #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
        #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
        #pragma config FPLLODIV = DIV_1         // PLL Output Divider
        #pragma config FPBDIV   = DIV_2         // Peripheral Clock divisor
        #pragma config FWDTEN   = OFF           // Watchdog Timer
        #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
        #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
        #pragma config OSCIOFNC = OFF           // CLKO Enable
        #pragma config POSCMOD  = XT            // Primary Oscillator
        #pragma config IESO     = OFF           // Internal/External Switch-over
        #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
        #pragma config FNOSC    = PRIPLL        // Oscillator Selection
        #pragma config CP       = OFF           // Code Protect
        #pragma config BWP      = OFF           // Boot Flash Write Protect
        #pragma config PWP      = OFF           // Program Flash Write Protect
	#endif
	#ifdef __32MX470F512H__
        // DEVCFG3
        // USERID = No Setting
        #pragma config FSRSSEL = PRIORITY_7            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
        #pragma config PMDL1WAY = OFF            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
        #pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow multiple reconfigurations)
        #pragma config FUSBIDIO = ON            // USB USBID Selection (Controlled by the USB Module)
        #pragma config FVBUSONIO = ON

        // DEVCFG2
        #ifdef XTAL_8MHZ
            #pragma config FPLLIDIV = DIV_2         // System PLL Input Divider (2x Divider) Crystall 8MHz
            #pragma config UPLLIDIV = DIV_2       // Crystall 8MHz
            #pragma config FPLLMUL = MUL_20        // System PLL Multiplier (PLL Multiply by xx)
            #pragma config FPLLODIV = DIV_1
            #pragma config UPLLEN = ON
            #pragma config POSCMOD = XT             // Primary Oscillator Configuration (External clock mode)
        #endif
        #ifdef XTAL_10MHZ
            #pragma config FPLLIDIV = DIV_2         // System PLL Input Divider (3x Divider) Crystall 10MHz
            #pragma config UPLLIDIV = DIV_5         // Crystall 10MHz
            #pragma config FPLLMUL = MUL_16        // System PLL Multiplier (PLL Multiply by xx)
            #pragma config FPLLODIV = DIV_1
            #pragma config UPLLEN = ON
            #pragma config POSCMOD = HS             // Primary Oscillator Configuration (External clock mode)
        #endif
        #ifdef XTAL_20MHZ
            #pragma config FPLLIDIV = DIV_5         // System PLL Input Divider (3x Divider) Crystall 20MHz
            #pragma config UPLLIDIV = DIV_5         // Crystall 20MHz
            #pragma config FPLLMUL = MUL_20        // System PLL Multiplier (PLL Multiply by xx)
            #pragma config FPLLODIV = DIV_1
            #pragma config UPLLEN = ON
            #pragma config POSCMOD = HS             // Primary Oscillator Configuration (External clock mode)
        #endif
        #ifdef INT_OSC_8MHZ
            #pragma config FNOSC = FRCPLL             // Oscillator Selection Bits (System PLL)
        #else 
            #pragma config FNOSC = PRIPLL             // Oscillator Selection Bits (System PLL)
        #endif

        // DEVCFG1
        #pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disable SOSC)
        #pragma config IESO = OFF                // Internal/External Switch Over (Enabled)
        #pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
        #pragma config FPBDIV = DIV_2
        #pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
        #pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
        #pragma config WINDIS = OFF
        #pragma config FWDTEN = OFF
        #pragma config FWDTWINSZ = WINSZ_25

        // DEVCFG0
        #pragma config DEBUG = OFF
        #pragma config JTAGEN = OFF
        #pragma config ICESEL = ICS_PGx2
        #pragma config PWP = OFF
        #pragma config BWP = OFF
        #pragma config CP = OFF
    #endif
	#ifdef __32MX460F512L__
        #pragma config UPLLEN   = ON        // USB PLL Enabled
        #pragma config FPLLMUL  = MUL_20        // PLL Multiplier
        #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
        #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
        #pragma config FPLLODIV = DIV_1         // PLL Output Divider
        #pragma config FPBDIV   = DIV_2         // Peripheral Clock divisor
        #pragma config FWDTEN   = OFF           // Watchdog Timer
        #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
        #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
        #pragma config OSCIOFNC = OFF           // CLKO Enable
        #pragma config POSCMOD  = HS            // Primary Oscillator
        #pragma config IESO     = OFF           // Internal/External Switch-over
        #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
        #pragma config FNOSC    = PRIPLL        // Oscillator Selection
        #pragma config CP       = OFF           // Code Protect
        #pragma config BWP      = OFF           // Boot Flash Write Protect
        #pragma config PWP      = OFF           // Program Flash Write Protect
	#endif
	#ifdef __32MX470F512L__
        // DEVCFG3
        // USERID = No Setting
        #pragma config FSRSSEL = PRIORITY_7            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
        #pragma config PMDL1WAY = OFF            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
        #pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow multiple reconfigurations)
        #pragma config FUSBIDIO = ON            // USB USBID Selection (Controlled by the USB Module)
        #pragma config FVBUSONIO = ON

        // DEVCFG2
        #ifdef XTAL_8MHZ
            #pragma config FPLLIDIV = DIV_2         // System PLL Input Divider (2x Divider) Crystall 8MHz
            #pragma config UPLLIDIV = DIV_2       // Crystall 8MHz
            #pragma config FPLLMUL = MUL_20        // System PLL Multiplier (PLL Multiply by xx)
            #pragma config FPLLODIV = DIV_1
            #pragma config UPLLEN = ON
            #pragma config POSCMOD = XT             // Primary Oscillator Configuration (External clock mode)
        #endif
        #ifdef XTAL_10MHZ
            #pragma config FPLLIDIV = DIV_2         // System PLL Input Divider (3x Divider) Crystall 10MHz
            #pragma config UPLLIDIV = DIV_5         // Crystall 10MHz
            #pragma config FPLLMUL = MUL_16        // System PLL Multiplier (PLL Multiply by xx)
            #pragma config FPLLODIV = DIV_1
            #pragma config UPLLEN = ON
            #pragma config POSCMOD = HS             // Primary Oscillator Configuration (External clock mode)
        #endif
        #ifdef XTAL_20MHZ
            #pragma config FPLLIDIV = DIV_5         // System PLL Input Divider (3x Divider) Crystall 20MHz
            #pragma config UPLLIDIV = DIV_5         // Crystall 20MHz
            #pragma config FPLLMUL = MUL_20        // System PLL Multiplier (PLL Multiply by xx)
            #pragma config FPLLODIV = DIV_1
            #pragma config UPLLEN = ON
            #pragma config POSCMOD = HS             // Primary Oscillator Configuration (External clock mode)
        #endif
        #ifdef INT_OSC_8MHZ
            #pragma config FNOSC = FRCPLL             // Oscillator Selection Bits (System PLL)
        #else 
            #pragma config FNOSC = PRIPLL             // Oscillator Selection Bits (System PLL)
        #endif

        // DEVCFG1
        #pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disable SOSC)
        #pragma config IESO = OFF                // Internal/External Switch Over (Enabled)
        #pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
        #pragma config FPBDIV = DIV_2
        #pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
        #pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
        #pragma config WINDIS = OFF
        #pragma config FWDTEN = OFF
        #pragma config FWDTWINSZ = WINSZ_25

        // DEVCFG0
        #pragma config DEBUG = OFF
        #pragma config JTAGEN = OFF
        #pragma config ICESEL = ICS_PGx2
        #pragma config PWP = OFF
        #pragma config BWP = OFF
        #pragma config CP = OFF
    #endif
#endif

volatile unsigned char do_loopCnt = 0;
unsigned long loopCntTemp = 0;
unsigned long loopCnt = 0;
unsigned int tick_count = 0;

unsigned long loopCntHistory[60];
unsigned int loopCntHistoryCnt = 0;
ResetReason resetReason = ResetReason_Unknown;
volatile unsigned int RCONtemp = 0;

int main (void) {
	#ifdef __32MX440F256H__
		DDPCON = 0; //Turn off JTAG and TRACE
	#else
	#ifdef __32MX470F512H__
        CM1CON = 0;
        CM2CON = 0;
		DDPCONbits.JTAGEN = 0;
	#else
	#ifdef __32MX460F512L__
		DDPCON = 0; //Turn off JTAG and TRACE
	#else
	#ifdef __32MX470F512L__
        CM1CON = 0;
        CM2CON = 0;
		DDPCONbits.JTAGEN = 0;
	#else
		#error TODO Implement
	#endif
	#endif
	#endif
	#endif

	RCONtemp = RCON;
	resetReason = mal_reset_reason();

	{
		#ifdef _PCACHE
			unsigned int cache_status = 0;
			unsigned int sys_clock = SYS_FREQ;
			unsigned int wait_states = 0;
		#endif

		//lock_isr(); no need since here interrupts are not enabled at all

		BMXCONCLR = _BMXCON_BMXWSDRM_MASK;

		#ifdef _PCACHE
			#define FLASH_SPEED_HZ          30000000 //Max Flash speed
			wait_states = 0;

			while(sys_clock > FLASH_SPEED_HZ)
			{
				wait_states++;
				sys_clock -= FLASH_SPEED_HZ;
			}

			CHECON = wait_states;

			cache_status = CHECON;
			cache_status |= (3 << _CHECON_PREFEN_POSITION);
			CHECON = cache_status;
			{
                void __attribute__ ((nomips16)) CheKseg0CacheOn();
				CheKseg0CacheOn();
			}
		#endif

		//unlock_isr();
	}

	#ifdef __32MX440F256H__
		AD1PCFG = 0xFFFF;
	#else
	#ifdef __32MX470F512H__
		ANSELB = 0;
		ANSELC = 0;
		ANSELD = 0;
		ANSELE = 0;
		ANSELF = 0;
		ANSELG = 0;
	#else
	#ifdef __32MX460F512L__
		AD1PCFG = 0xFFFF;
	#else
	#ifdef __32MX470F512L__
		ANSELB = 0;
		ANSELC = 0;
		ANSELD = 0;
		ANSELE = 0;
		ANSELF = 0;
		ANSELG = 0;
	#else
		#error TODO Implement
	#endif
	#endif
	#endif
	#endif
	
	init_stackmeasure();
	init_ad();

	init_pwm();
	init_dee();
	init_eep_manager();
	init_task();

	init_tmr(); //this should be the first beceaus it clears registered timers.

	init_usb();
	init_app();
	init_isr();
	init_grbl();
	do_loopCnt = 0;

	bootloader_interface_clearRequest();
	
	while (1) {
		if (do_loopCnt) {
			do_loopCnt = 0;
			loopCnt = loopCntTemp; //Risk is here that interrupt corrupts the value, but it is taken
			loopCntHistory[loopCntHistoryCnt] = loopCnt;
			loopCntHistoryCnt++;
			loopCntHistoryCnt %= ((sizeof(loopCntHistory)) / (sizeof(*loopCntHistory)));
			loopCntTemp = 0;
			if (loopCnt < 1000) { //1run/ms
				/*while (1) {
					Nop();
					ERROR_LED = 1;
				}*/
			}
		}

		do_stackmeasure();
		do_task();
		do_ad();
		do_eep_manager();
		do_app();
		do_tmr();
		do_usb();
		do_grbl();
		do_dee();

		//do_spi();
		//do_fatfs();
		//do_sd_spi();
		//idle_Request();
		loopCntTemp++;
	}
	return 0;
}

void isr_main_1ms(void) {
	static uint16 loopCntTmr = 0;

	tick_count++;

	loopCntTmr ++;
	if (loopCntTmr == 1000) {
		loopCntTmr = 0;
		do_loopCnt = 1;
	}

	isr_task_1ms();
	isr_eep_manager_1ms();
	isr_usb_1ms();
	isr_ad_1ms();
	isr_app_1ms();
	isr_grbl_1ms();
	isr_dee_1ms();
}

void isr_main_100us(void) {
	isr_app_100us();
}
