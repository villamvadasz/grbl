#include "mal.h"

#include "c_main.h"

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
		#ifdef SYS_FREQ_10MHZ
		    #pragma config FPLLODIV = DIV_8         // PLL Output Divider
		#endif
		#ifdef SYS_FREQ_20MHZ
		    #pragma config FPLLODIV = DIV_4         // PLL Output Divider
		#endif
		#ifdef SYS_FREQ_80MHZ
		    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
		#endif
		#if (PB_DIV == 2)
	    	#pragma config FPBDIV   = DIV_2         // Peripheral Clock divisor
		#endif	
		#if (PB_DIV == 4)
	    	#pragma config FPBDIV   = DIV_4         // Peripheral Clock divisor
		#endif	
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
			#ifdef SYS_FREQ_10MHZ
			    #pragma config FPLLODIV = DIV_8         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_20MHZ
			    #pragma config FPLLODIV = DIV_4         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_80MHZ
			    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
			#endif
            #pragma config UPLLEN = ON
            #pragma config POSCMOD = XT             // Primary Oscillator Configuration (External clock mode)
        #endif
        #ifdef XTAL_10MHZ
            #pragma config FPLLIDIV = DIV_2         // System PLL Input Divider (3x Divider) Crystall 10MHz
            #pragma config UPLLIDIV = DIV_5         // Crystall 10MHz
            #pragma config FPLLMUL = MUL_16        // System PLL Multiplier (PLL Multiply by xx)
			#ifdef SYS_FREQ_10MHZ
			    #pragma config FPLLODIV = DIV_8         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_20MHZ
			    #pragma config FPLLODIV = DIV_4         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_80MHZ
			    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
			#endif
            #pragma config UPLLEN = ON
            #pragma config POSCMOD = HS             // Primary Oscillator Configuration (External clock mode)
        #endif
        #ifdef XTAL_20MHZ
            #pragma config FPLLIDIV = DIV_5         // System PLL Input Divider (3x Divider) Crystall 20MHz
            #pragma config UPLLIDIV = DIV_5         // Crystall 20MHz
            #pragma config FPLLMUL = MUL_20        // System PLL Multiplier (PLL Multiply by xx)
			#ifdef SYS_FREQ_10MHZ
			    #pragma config FPLLODIV = DIV_8         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_20MHZ
			    #pragma config FPLLODIV = DIV_4         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_80MHZ
			    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
			#endif
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
		#ifdef SYS_FREQ_10MHZ
		    #pragma config FPLLODIV = DIV_8         // PLL Output Divider
		#endif
		#ifdef SYS_FREQ_20MHZ
		    #pragma config FPLLODIV = DIV_4         // PLL Output Divider
		#endif
		#ifdef SYS_FREQ_80MHZ
		    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
		#endif
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
			#ifdef SYS_FREQ_10MHZ
			    #pragma config FPLLODIV = DIV_8         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_20MHZ
			    #pragma config FPLLODIV = DIV_4         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_80MHZ
			    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
			#endif
            #pragma config UPLLEN = ON
            #pragma config POSCMOD = XT             // Primary Oscillator Configuration (External clock mode)
        #endif
        #ifdef XTAL_10MHZ
            #pragma config FPLLIDIV = DIV_2         // System PLL Input Divider (3x Divider) Crystall 10MHz
            #pragma config UPLLIDIV = DIV_5         // Crystall 10MHz
            #pragma config FPLLMUL = MUL_16        // System PLL Multiplier (PLL Multiply by xx)
			#ifdef SYS_FREQ_10MHZ
			    #pragma config FPLLODIV = DIV_8         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_20MHZ
			    #pragma config FPLLODIV = DIV_4         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_80MHZ
			    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
			#endif
            #pragma config UPLLEN = ON
            #pragma config POSCMOD = HS             // Primary Oscillator Configuration (External clock mode)
        #endif
        #ifdef XTAL_20MHZ
            #pragma config FPLLIDIV = DIV_5         // System PLL Input Divider (3x Divider) Crystall 20MHz
            #pragma config UPLLIDIV = DIV_5         // Crystall 20MHz
            #pragma config FPLLMUL = MUL_20        // System PLL Multiplier (PLL Multiply by xx)
			#ifdef SYS_FREQ_10MHZ
			    #pragma config FPLLODIV = DIV_8         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_20MHZ
			    #pragma config FPLLODIV = DIV_4         // PLL Output Divider
			#endif
			#ifdef SYS_FREQ_80MHZ
			    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
			#endif
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

ResetReason mal_resetReason_temp = ResetReason_Unknown;
volatile unsigned int mal_RCON_temp = 0;

volatile unsigned int gieTemp = 0;
volatile unsigned int isrLockCnt = 0;

void __attribute__((noreturn)) SoftReset(void);
ResetReason mal_reset_reason(void);

extern unsigned int bootloader_get_bootloader_was_reset_called(void);
__attribute__(( weak )) unsigned int bootloader_get_bootloader_was_reset_called(void) {return 0;} 

void init_mal(void) {
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

	mal_RCON_temp = RCON;
	mal_resetReason_temp = mal_reset_reason();
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

}

ResetReason mal_get_reset_reason(unsigned int * rcon) {
	ResetReason result = 0;
	result = mal_resetReason_temp;
	if (rcon != NULL) {
		*rcon = mal_RCON_temp;
	}
	return result;
}

void mal_reset(void) {
	SoftReset();
}

ResetReason mal_reset_reason(void) {
	ResetReason resetReason = ResetReason_Unknown;
	if (
				RCONbits.POR &&
				RCONbits.BOR
	) {
		resetReason = ResetReason_Power_on_Reset;
	} else if (
				RCONbits.POR &&
				!RCONbits.BOR
	) {
		resetReason = ResetReason_Brown_out_Reset;
	} else if (
				RCONbits.EXTR &&
				!RCONbits.SLEEP &&
				!RCONbits.IDLE
	) {
		resetReason = ResetReason_MCLR_reset_during_normal_operation;
	} else if (
				RCONbits.SWR
	) {
		if (bootloader_get_bootloader_was_reset_called() == 0xcafecafe) {
			resetReason = ResetReason_Software_reset_during_normal_operation_bootloader;
		} else {
			resetReason = ResetReason_Software_reset_during_normal_operation;
		}
	} else if (
				RCONbits.EXTR &&
				RCONbits.SLEEP &&
				!RCONbits.IDLE
	) {
		resetReason = ResetReason_MCLR_reset_during_sleep;
	} else if (
				RCONbits.EXTR &&
				!RCONbits.SLEEP &&
				RCONbits.IDLE
	) {
		resetReason = ResetReason_MCLR_reset_during_idle;
	} else if (
				RCONbits.WDTO
				//!RCONbits.SLEEP &&
				//!RCONbits.IDLE
	) {
		resetReason = ResetReason_WDT_Time_out_reset;
	} else if (
				RCONbits.CMR
	) {
		resetReason = ResetReason_Configuration_Word_Mismatch_Reset;
	} else {
		resetReason = ResetReason_Unknown;
	}
	RCON = 0;
	return resetReason;
}
