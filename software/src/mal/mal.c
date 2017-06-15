#include "mal.h"

#include "c_main.h"
#include "noinitram.h"
#include "exceptions.h"

#ifdef __DEBUG
	#ifndef _STANDALONE_APPLICATION
		#define _STANDALONE_APPLICATION
	#endif
#endif

#ifdef _STANDALONE_APPLICATION
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
		#ifndef APP_SLEEP_MODE
			#pragma config WDTPS	= PS1				// Watchdog Timer Postscale
		#else
			//1:1024 1.024 s
			//1:2048 2.048 s
			//1:4096 4.096 s
			//1:8192 8.192 s
			//1:16384 16.384 s
			//#pragma config WDTPS	= PS16384				// Watchdog Timer Postscale
			//1:32768 32.768 s
			//1:65536 65.536 s
			//1:131072 131.072 s		2.2min
			//1:262144 262.144 s		4.4min
			//1:524288 524.288 s		8.7min
			//1:1045876 1048.576 s		17.5min
			#pragma config WDTPS	= PS1048576				// Watchdog Timer Postscale
		#endif
		#pragma config FCKSM	= CSECMD			// Clock Switching & Fail Safe Clock Monitor
		#pragma config OSCIOFNC = OFF				// CLKO Enable
		#pragma config IESO	 = OFF				// Internal/External Switch-over
		#pragma config FSOSCEN  = OFF				// Secondary Oscillator Enable (KLO was off)
		#pragma config CP	   = OFF				// Code Protect
		#pragma config BWP	  = OFF				// Boot Flash Write Protect
		#pragma config PWP	  = OFF				// Program Flash Write Protect
	#endif
	#ifdef __32MX470F512H__
		// DEVCFG3
		// USERID = No Setting
		#pragma config FSRSSEL = PRIORITY_7			// Peripheral Module Disable Configuration (Allow only one reconfiguration)
		#pragma config PMDL1WAY = OFF				// Peripheral Module Disable Configuration (Allow only one reconfiguration)
		#pragma config IOL1WAY = OFF				// Peripheral Pin Select Configuration (Allow multiple reconfigurations)
		#pragma config FUSBIDIO = ON				// USB USBID Selection (Controlled by the USB Module)
		#pragma config FVBUSONIO = ON

		// DEVCFG2
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

		// DEVCFG1
		#pragma config FSOSCEN = OFF				// Secondary Oscillator Enable (Disable SOSC)
		#pragma config IESO = OFF					// Internal/External Switch Over (Enabled)
		#pragma config OSCIOFNC = OFF				// CLKO Output Signal Active on the OSCO Pin (Disabled)
		#pragma config FCKSM = CSECMD				// Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
		#pragma config WDTPS = PS1048576			// Watchdog Timer Postscaler (1:1048576)
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
		#pragma config IESO	 = OFF				// Internal/External Switch-over
		#pragma config FSOSCEN  = OFF				// Secondary Oscillator Enable (KLO was off)
		#pragma config CP	   = OFF				// Code Protect
		#pragma config BWP	  = OFF				// Boot Flash Write Protect
		#pragma config PWP	  = OFF				// Program Flash Write Protect
	#endif
	#ifdef __32MX470F512L__
		// DEVCFG3
		// USERID = No Setting
		#pragma config FSRSSEL = PRIORITY_7			// Peripheral Module Disable Configuration (Allow only one reconfiguration)
		#pragma config PMDL1WAY = OFF				// Peripheral Module Disable Configuration (Allow only one reconfiguration)
		#pragma config IOL1WAY = OFF				// Peripheral Pin Select Configuration (Allow multiple reconfigurations)
		#pragma config FUSBIDIO = ON				// USB USBID Selection (Controlled by the USB Module)
		#pragma config FVBUSONIO = ON

		// DEVCFG2
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

		// DEVCFG1
		#pragma config FSOSCEN = OFF				// Secondary Oscillator Enable (Disable SOSC)
		#pragma config IESO = OFF					// Internal/External Switch Over (Enabled)
		#pragma config OSCIOFNC = OFF				// CLKO Output Signal Active on the OSCO Pin (Disabled)
		#pragma config FCKSM = CSECMD				// Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
		#pragma config WDTPS = PS1048576			// Watchdog Timer Postscaler (1:1048576)
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
	#ifdef __32MX795F512H__
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
		#pragma config IESO	 = OFF				// Internal/External Switch-over
		#pragma config FSOSCEN  = OFF				// Secondary Oscillator Enable (KLO was off)
		#pragma config CP	   = OFF				// Code Protect
		#pragma config BWP	  = OFF				// Boot Flash Write Protect
		#pragma config PWP	  = OFF				// Program Flash Write Protect
	#endif
#endif

ResetReason mal_resetReason_temp = ResetReason_Unknown;
volatile unsigned int mal_RCON_temp = 0;

volatile unsigned int gieTemp = 0;
volatile unsigned int isrLockCnt = 0;

volatile unsigned int mal_reset_counter = 0;

void __attribute__((noreturn)) SoftReset(void);
ResetReason mal_reset_reason(void);
void mal_clockswitch_slowdown(void);
void mal_clockswitch(void);

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
	#ifdef __32MX795F512H__
		CM1CON = 0;
		CM2CON = 0;
		DDPCONbits.JTAGEN = 0;
	#else
	#ifdef __32MX795F512H__
		DDPCON = 0; //Turn off JTAG and TRACE
		#error TODO Implement
	#endif
	#endif
	#endif
	#endif
	#endif
	#endif

	mal_RCON_temp = RCON;
	mal_resetReason_temp = mal_reset_reason();
	
	mal_clockswitch_slowdown();
	{
		#ifdef _PCACHE
			unsigned int cache_status = 0;
			unsigned int sys_clock = SYS_FREQ;
			unsigned int wait_states = 0;
		#endif

		//lock_isr(); no need since here interrupts are not enabled at all

		BMXCONCLR = _BMXCON_BMXWSDRM_MASK;

		#ifdef _PCACHE
			#define FLASH_SPEED_HZ		  30000000 //Max Flash speed
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
	
	mal_clockswitch();

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
	#ifdef __32MX795F512H__
		AD1PCFG = 0xFFFF;
	#else
		#error TODO Implement
	#endif
	#endif
	#endif
	#endif
	#endif

	init_exception();

	mal_reset_counter = 0;
	noinitram_getByteArray(0, (unsigned char *)&mal_reset_counter, sizeof(mal_reset_counter) / sizeof(unsigned char));
	mal_reset_counter++;
	noinitram_setByteArray(0, (unsigned char *)&mal_reset_counter, sizeof(mal_reset_counter) / sizeof(unsigned char));
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

int mal_DmaSuspend(void) {
	int suspSt;
	if(!(suspSt=DMACONbits.SUSPEND)) {
		DMACONSET=_DMACON_SUSPEND_MASK;		// suspend
		while(!(DMACONbits.SUSPEND));	// wait to be actually suspended
	}
	return suspSt;
}

void mal_DmaResume(int susp) {
	if(susp) {
		mal_DmaSuspend();
	} else {
		DMACONCLR=_DMACON_SUSPEND_MASK;		// resume DMA activity
	}
}

#pragma GCC diagnostic ignored "-Wunused-variable"
void mal_clockswitch_slowdown(void) {
	#ifndef _STANDALONE_APPLICATION
		int	int_status = 0;
		int	susp = 0;

		// Disable DMA & Disable Interrupts
		#ifdef _DMAC
			lock_isr();
			susp = mal_DmaSuspend();
		#else
			lock_isr(); 
		#endif
		
		
		SYSKEY = 0; // ensure OSCCON is locked
		SYSKEY = 0xAA996655; // Write Key1 to SYSKEY
		SYSKEY = 0x556699AA; // Write Key2 to SYSKEY
		// OSCCON is now unlocked
		
		//First change to FRC 8MHz
		OSCCONbits.NOSC = 0x0;
		OSCCONbits.FRCDIV = 0x0;
		OSCCONbits.OSWEN = 1;
		while (OSCCONbits.OSWEN);

		SYSKEY = 0; // ensure OSCCON is locked
		
		// Enable DMA & Enable Interrupts
		#ifdef _DMAC
			mal_DmaResume(susp);
			unlock_isr();
		#else
			unlock_isr();
		#endif
	#endif
}

void mal_clockswitch(void) {
	#ifndef _STANDALONE_APPLICATION
		int	int_status = 0;
		int	susp = 0;

		// Disable DMA & Disable Interrupts
		#ifdef _DMAC
			lock_isr();
			susp = mal_DmaSuspend();
		#else
			lock_isr(); 
		#endif
		
		
		SYSKEY = 0; // ensure OSCCON is locked
		SYSKEY = 0xAA996655; // Write Key1 to SYSKEY
		SYSKEY = 0x556699AA; // Write Key2 to SYSKEY
		// OSCCON is now unlocked
		
		// Change to external Qtal 8MHz and PLL it to 80MHz
		#ifdef XTAL_8MHZ
			#ifdef SYS_FREQ_10MHZ
				//Change to PLL
				OSCCONbits.PLLODIV = 0x4;	//DIV_8
				OSCCONbits.PLLMULT = 0x5;	//MUL_20
				OSCCONbits.NOSC = 0x3;
				OSCCONbits.OSWEN = 1;
				while (OSCCONbits.OSWEN);
			#endif
			#ifdef SYS_FREQ_20MHZ
				//Change to PLL
				OSCCONbits.PLLODIV = 0x3;	//DIV_4
				OSCCONbits.PLLMULT = 0x5;	//MUL_20
				OSCCONbits.NOSC = 0x3;
				OSCCONbits.OSWEN = 1;
				while (OSCCONbits.OSWEN);
			#endif
			#ifdef SYS_FREQ_40MHZ
				//Change to PLL
				OSCCONbits.PLLODIV = 0x1;	//DIV_2
				OSCCONbits.PLLMULT = 0x5;	//MUL_20
				OSCCONbits.NOSC = 0x3;
				OSCCONbits.OSWEN = 1;
				while (OSCCONbits.OSWEN);
			#endif
			#ifdef SYS_FREQ_80MHZ
				//Change to PLL
				OSCCONbits.PLLODIV = 0x0;	//DIV_1
				OSCCONbits.PLLMULT = 0x5;	//MUL_20
				OSCCONbits.NOSC = 0x3;
				OSCCONbits.OSWEN = 1;
				while (OSCCONbits.OSWEN);
			#endif
		#endif

		SYSKEY = 0; // ensure OSCCON is locked
		
		// Enable DMA & Enable Interrupts
		#ifdef _DMAC
			mal_DmaResume(susp);
			unlock_isr();
		#else
			unlock_isr();
		#endif
	#endif
}
#pragma GCC diagnostic pop

unsigned char hexStringToChar(unsigned char *str) {
	unsigned char result = 0;
	if (str != NULL) {
		unsigned int x = 0;
		for (x = 0; x < 2; x++) {
			result <<= 4;
			if ((str[x] >= '0') && (str[x] <= '9')) {
				result += str[x] - '0' + 0x00;
			} else if ((str[x] >= 'A') && (str[x] <= 'F')) {
				result += str[x] - 'A' + 0x0A;
			} else if ((str[x] >= 'a') && (str[x] <= 'f')) {
				result += str[x] - 'a' + 0x0A;
			}
		}
	}
	return result;
}

unsigned int hexStringToInt(unsigned char *str) {
	unsigned int result = 0;
	if (str != NULL) {
		unsigned int x = 0;
		for (x = 0; x < 8; x++) {
			result <<= 4;
			if ((str[x] >= '0') && (str[x] <= '9')) {
				result += str[x] - '0' + 0x00;
			} else if ((str[x] >= 'A') && (str[x] <= 'F')) {
				result += str[x] - 'A' + 0x0A;
			} else if ((str[x] >= 'a') && (str[x] <= 'f')) {
				result += str[x] - 'a' + 0x0A;
			}
		}
	}
	return result;
}

void __attribute__((noreturn)) SoftReset(void)
{
	volatile unsigned int* p = &RSWRST;

	lock_isr(); 
	mal_DmaSuspend();
	SYSKEY = 0, SYSKEY = 0xAA996655, SYSKEY = 0x556699AA;

	RSWRSTSET = _RSWRST_SWRST_MASK;
	*p;

	while(1);

}

void __attribute__ ((nomips16)) CheKseg0CacheOff()
{
	register unsigned long tmp;

	asm("mfc0 %0,$16,0" :  "=r"(tmp));
	tmp = (tmp & ~7) | 2;
	asm("mtc0 %0,$16,0" :: "r" (tmp));
}

void __attribute__ ((nomips16)) CheKseg0CacheOn()
{
	register unsigned long tmp;
	asm("mfc0 %0,$16,0" :  "=r"(tmp));
	tmp = (tmp & ~7) | 3;
	asm("mtc0 %0,$16,0" :: "r" (tmp));
}

unsigned int __attribute__((nomips16)) INTDisableInterrupts(void)
{
    unsigned int status = 0;

    asm volatile("di    %0" : "=r"(status));

    return status;
}

void __attribute__((nomips16))  INTRestoreInterrupts(unsigned int status)
{
    if(status & 0x00000001)
        asm volatile("ei");
    else
        asm volatile("di");
}

unsigned int mal_get_reset_counter(void) {
	return mal_reset_counter;
}
