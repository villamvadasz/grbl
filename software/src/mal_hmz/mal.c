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
	#ifdef __32MZ2048ECG144__
		// Configuration Bit settings
		// SYSCLK = "200 MHz (24MHz EC * ???? )
		// PBCLK7 = 200 MHz
		// PBCLK3 = 8 MHz
		// EC Oscillator w. PLL
		// WDT OFF
		// Other options are don't care
		//
		// Key Settings:
		// OSCILLATOR: 12MHz EC Oscillator w. PLL
		// SYSCLK = 200 MHz (set via config bits)
		// PBCLK7 (CPU) = SYSCLK/1 = 200 MHz (default setting in PB7DIV SFR)
		// PBCLK3 (Timer3) = 8 MHz (set via initialization code below)
		// JTAG PORT: Disabled
		// WATCHDOG TIMER: Disabled
		// DEBUG/PGM PINS: PGEC2/PGED2

		// DEVCFG3
		// USERID = No Setting
		#pragma config FMIIEN =     OFF
		#pragma config FETHIO =     OFF
		#pragma config PGL1WAY =    OFF
		#pragma config PMDL1WAY =   OFF
		#pragma config IOL1WAY =    OFF
		#pragma config FUSBIDIO =   OFF

		// DEVCFG2
		#pragma config FPLLIDIV = DIV_3         // System PLL Input Divider (3x Divider)
		#pragma config FPLLRNG =  RANGE_5_10_MHZ
		#pragma config FPLLICLK = PLL_POSC      // System PLL Input Clock Selection (POSC is input to the System PLL)
		#pragma config FPLLMULT = MUL_100
		#pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (2x Divider)
		#pragma config UPLLFSEL = FREQ_12MHZ    // USB PLL Input Frequency Selection (USB PLL input is 24 MHz)
		#pragma config UPLLEN = ON              // USB PLL Enable (USB PLL is enabled)

		// DEVCFG1
		#pragma config FNOSC = SPLL             // Oscillator Selection Bits (System PLL)
		#pragma config DMTINTV = WIN_127_128    // DMT Count Window Interval (Window/Interval value is 127/128 counter value)
		#pragma config FSOSCEN = ON            // Secondary Oscillator Enable (Disable SOSC)
		#pragma config IESO = ON               // Internal/External Switch Over (Enabled)
		#pragma config POSCMOD = HS             // Primary Oscillator Configuration (External clock mode)
		#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
		#pragma config FCKSM = CSECMD
		#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
		#pragma config WDTSPGM = STOP           // Watchdog Timer Stop During Flash Programming (WDT stops during Flash programming)
		#pragma config WINDIS = NORMAL          // Watchdog Timer Window Mode (Watchdog Timer is in non-Window mode)
		#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled)
		#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window size is 25%)
		#pragma config DMTCNT = DMT31           // Deadman Timer Count Selection (2^31 (2147483648))
		#pragma config FDMTEN = OFF             // Deadman Timer Enable (Deadman Timer is disabled)

		// DEVCFG0
		#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
		#pragma config JTAGEN = OFF              // JTAG Enable (JTAG Port Enabled)
		#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (Communicate on PGEC2/PGED2)
		#pragma config TRCEN = OFF               // Trace Enable (Trace features in the CPU are enabled)
		#pragma config BOOTISA = MIPS32         // Boot ISA Selection (Boot code and Exception code is MIPS32)
		#pragma config FECCCON = OFF_UNLOCKED   // Dynamic Flash ECC Configuration (ECC and Dynamic ECC are disabled (ECCCON bits are writable))
		#pragma config FSLEEP = OFF             // Flash Sleep Mode (Flash is powered down when the device is in Sleep mode)
		#pragma config DBGPER = PG_ALL          // Debug Mode CPU Access Permission (Allow CPU access to all permission regions)
		#pragma config EJTAGBEN = NORMAL        // EJTAG Boot (Normal EJTAG functionality)

		// DEVCP0
		#pragma config CP = OFF                 // Code Protect (Protection Disabled)

		// SEQ0

		// DEVADC1

		// DEVADC2

		// DEVADC3

		// DEVADC4

		// DEVADC5

		// #pragma config statements should precede project file includes.
		// Use project enums instead of #define for ON and OFF.

	#endif
#endif

ResetReason mal_resetReason_temp = ResetReason_Unknown;
volatile unsigned int mal_RCON_temp = 0;

volatile unsigned int gieTemp = 0;
volatile unsigned int isrLockCnt = 0;

volatile unsigned int mal_reset_counter = 0;

void __attribute__((noreturn)) SoftReset(void);
ResetReason mal_reset_reason(void);

extern unsigned int bootloader_get_bootloader_was_reset_called(void);
__attribute__(( weak )) unsigned int bootloader_get_bootloader_was_reset_called(void) {return 0;} 

void init_mal(void) {
	mal_RCON_temp = RCON;
	mal_resetReason_temp = mal_reset_reason();

	// PIC32MZ CPU Speed Optimizations (Cache/Wait States/Peripheral Bus Clks)
	// On reset, I+D cache is enabled for max performace setting (write-back with write allocation)
	// No wait setting required for main data RAM

	// Prefetch-cache: Enable prefetch for PFM (any PFM instructions or data)
	PRECONbits.PREFEN = 3;
	// Flash PM Wait States: MZ Flash runs at 2 wait states @ 200 MHz
	PRECONbits.PFMWS = 2;

	// PBCLKx settings: N/U in this project

	/* set PBCLK2 to deliver 40Mhz clock for PMP/I2C/UART/SPI. */
	#define hwUNLOCK_KEY_0					( 0xAA996655UL )
	#define hwUNLOCK_KEY_1					( 0x556699AAUL )

	SYSKEY = hwUNLOCK_KEY_0;
	SYSKEY = hwUNLOCK_KEY_1;

	PB1DIVbits.PBDIV = PB1_DIV - 1;
	PB2DIVbits.PBDIV = PB2_DIV - 1;
	PB3DIVbits.PBDIV = PB3_DIV - 1;
	PB4DIVbits.PBDIV = PB4_DIV - 1;
	PB5DIVbits.PBDIV = PB5_DIV - 1;
	//PB6DIVbits.PBDIV = PB6_DIV - 1;
	PB7DIVbits.PBDIV = PB7_DIV - 1;
	PB8DIVbits.PBDIV = PB8_DIV - 1;

	SYSKEY = 0;

	ANSELB = 0x0000;
	ANSELE = 0x0000;
	ANSELG = 0x0000;

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
	extern void __pic32_software_reset(void);
	__pic32_software_reset();
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

int DmaSuspend(void) {
	int suspSt = 0;
	if (!(suspSt = DMACONbits.SUSPEND)) {
		DMACONSET = _DMACON_SUSPEND_MASK; // suspend
		while(!(DMACONbits.SUSPEND)); // wait to be actually suspended
	}
	return suspSt;
}

void DmaResume(int susp) {
	if(susp)
	{
		DmaSuspend();
	}
	else
	{
		DMACONCLR = _DMACON_SUSPEND_MASK;		// resume DMA activity
	}
}

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

unsigned int mal_get_reset_counter(void) {
	return mal_reset_counter;
}
