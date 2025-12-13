#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "Bootloader.h"
#include "Framework.h"
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include "my_plib.h"
#include "mal\fixedmemoryaddress.h"

#define BOOTLOADER_TRIGGER 0xDEAD1234

#define PB_BUS_MAX_FREQ_HZ      80000000 //Max Peripheral bus speed
#define OSC_PB_DIV_8    (3 << _OSCCON_PBDIV_POSITION)
#define OSC_PB_DIV_4    (2 << _OSCCON_PBDIV_POSITION)
#define OSC_PB_DIV_2    (1 << _OSCCON_PBDIV_POSITION)
#define OSC_PB_DIV_1    (0 << _OSCCON_PBDIV_POSITION)

#define FLASH_SPEED_HZ          30000000 //Max Flash speed
#define mCheConfigure(val) (CHECON = (val))

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
#define SWITCH_PRESSED 0

#ifdef USE_GRBL_STUFFS
	#define SPINDLE_ENABLE_DDR TRISDbits.TRISD4
	#define SPINDLE_ENABLE_PORT LATDbits.LATD4
	#define SPINDLE_PWM_DDR TRISDbits.TRISD1
	#define SPINDLE_PWM_PORT LATDbits.LATD1
#endif

#ifdef USE_GRBL_STUFFS
	#define N_AXIS 3
#endif
#define NOINITRAM_SIZE 128

#define NOINITRAM_BASE_ADDRESS_1 ADDRESS_NOINITRAM_BASE_ADDRESS_1
#define NOINITRAM_CRC_SIZE 4
#define NOINITRAM_BASE_ADDRESS_2 (NOINITRAM_BASE_ADDRESS_1 + NOINITRAM_CRC_SIZE + NOINITRAM_CRC_SIZE + NOINITRAM_SIZE + 8)

typedef struct _ExceptionLog {
	unsigned int _excep_code;
	unsigned int _excep_addr;
	unsigned int magicWord;
} ExceptionLog;

#pragma GCC diagnostic error "-W"
volatile unsigned int noinitram_crc_1[2] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_1)));
volatile unsigned char noinitram_1[NOINITRAM_SIZE] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_1 + NOINITRAM_CRC_SIZE + NOINITRAM_CRC_SIZE + 8)));

volatile unsigned int noinitram_crc_2[2] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_2)));
volatile unsigned char noinitram_2[NOINITRAM_SIZE] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_2 + NOINITRAM_CRC_SIZE + NOINITRAM_CRC_SIZE + 8)));

#ifdef USE_GRBL_STUFFS
	volatile int32_t grbl_eeprom_sys_position_1[N_AXIS + 1] __attribute__ ((persistent, address(ADDRESS_GRBL_EEPROM_SYS_POSITION_ADDRESS_1)));
	volatile int32_t grbl_eeprom_sys_position_2[N_AXIS + 1] __attribute__ ((persistent, address(ADDRESS_GRBL_EEPROM_SYS_POSITION_ADDRESS_2)));
	volatile int32_t grbl_eeprom_sys_position_3[N_AXIS + 1] __attribute__ ((persistent, address(ADDRESS_GRBL_EEPROM_SYS_POSITION_ADDRESS_3)));
#endif

volatile unsigned char bootloader_MAC[8] __attribute__ ((persistent, address(ADDRESS_bootloader_MAC)));
volatile unsigned int bootloader_request_A __attribute__ ((persistent, address(ADDRESS_bootloader_request_A)));
volatile unsigned int bootloader_request_B __attribute__ ((persistent, address(ADDRESS_bootloader_request_B)));
volatile unsigned int bootloader_was_reset_called __attribute__ ((persistent, address(ADDRESS_bootloader_was_reset_called)));
volatile ExceptionLog exceptionLog[16] __attribute__ ((persistent, address(ADDRESS_exceptionLog)));
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
		#ifdef BOOTLOADER_DECRYPT
			#pragma config CP	    = OFF				// Code Protect
		#else
			#pragma config CP	    = OFF				// Code Protect
		#endif
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
		#ifdef BOOTLOADER_DECRYPT
			#pragma config CP	    = OFF				// Code Protect
		#else
			#pragma config CP	    = OFF				// Code Protect
		#endif
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
		#ifdef BOOTLOADER_DECRYPT
			#pragma config CP	    = OFF				// Code Protect
		#else
			#pragma config CP	    = OFF				// Code Protect
		#endif
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
		#ifdef BOOTLOADER_DECRYPT
			#pragma config CP	    = OFF				// Code Protect
		#else
			#pragma config CP	    = OFF				// Code Protect
		#endif
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
		#pragma config IESO	    = OFF				// Internal/External Switch-over
		#pragma config FSOSCEN  = OFF				// Secondary Oscillator Enable (KLO was off)
		#ifdef BOOTLOADER_DECRYPT
			#pragma config CP	    = OFF				// Code Protect
		#else
			#pragma config CP	    = OFF				// Code Protect
		#endif
		#pragma config BWP	    = OFF				// Boot Flash Write Protect
		#pragma config PWP	    = OFF				// Program Flash Write Protect
	#endif
#endif

BOOL CheckTrigger(void);
void JumpToApp(void);
BOOL ValidAppPresent(void);
extern inline unsigned int __attribute__((always_inline)) SYSTEMConfig(unsigned int sys_clock, unsigned int flags);
extern inline unsigned int __attribute__((always_inline)) SYSTEMConfigPB(unsigned int sys_clock);

#ifdef USE_GRBL_STUFFS
	#define SPINDLE_ENABLE_DDR TRISDbits.TRISD4
	#define SPINDLE_ENABLE_PORT LATDbits.LATD4
	#define SPINDLE_PWM_DDR TRISDbits.TRISD1
	#define SPINDLE_PWM_PORT LATDbits.LATD1
#endif
INT main(void) {
	UINT pbClk;

	// Setup configuration
	pbClk = SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
	
	#ifdef USE_GRBL_STUFFS
		SPINDLE_ENABLE_PORT = 0;
		SPINDLE_ENABLE_DDR = 0; //Spindle must be disabled.
	
		SPINDLE_PWM_PORT = 1;
		SPINDLE_PWM_DDR = 0; //Spindle PWM be 10V
	#endif

	#ifdef __32MX795F512H__
		CM1CON = 0;
		CM2CON = 0;
		DDPCONbits.JTAGEN = 0;
		DDPCON = 0; //Turn off JTAG and TRACE
	#endif

	
	// Enter firmware upgrade mode if there is a trigger or if the application is not valid
	if (CheckTrigger() || !ValidAppPresent()) {
		// Initialize the transport layer - UART/USB/Ethernet
		FrameWorkInit(pbClk);
		
		while(!ExitFirmwareUpgradeMode()) {// Be in loop till framework recieves "run application" command from PC
			FrameWorkTask(); // Run frame work related tasks (Handling Rx frame, process frame and so on)
		}
		// Close trasnport layer.
		FrameWorkClose();
	}

	// No trigger + valid application = run application.
	JumpToApp();
	
	return 0;
}			

BOOL CheckTrigger(void) {
	BOOL result = FALSE;
	#ifdef BOOTLOADER_SWITCH_TRIGGER
		UINT SwitchStatus;
		SwitchStatus = ReadSwitchStatus();
		if (SwitchStatus == SWITCH_PRESSED) {
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

void JumpToApp(void) {
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

BOOL ValidAppPresent(void) {
	DWORD *AppPtr;
	AppPtr = (DWORD *)USER_APP_RESET_ADDRESS;
	if (*AppPtr == 0xFFFFFFFF) {
		return FALSE;
	} else {
		return TRUE;
	}
}			

void bootloader_get_shared_mac(unsigned char *data) {
	if (data != NULL) {
		if ((bootloader_MAC[6] == 0xCA) && (bootloader_MAC[7] == 0xFE)) {
			if (
				(bootloader_MAC[0] != 0x00) && (bootloader_MAC[0] != 0xFF) &&
				(bootloader_MAC[1] != 0x00) && (bootloader_MAC[1] != 0xFF) &&
				(bootloader_MAC[2] != 0x00) && (bootloader_MAC[2] != 0xFF) &&
				(bootloader_MAC[3] != 0x00) && (bootloader_MAC[3] != 0xFF) &&
				(bootloader_MAC[4] != 0x00) && (bootloader_MAC[4] != 0xFF) &&
				(bootloader_MAC[5] != 0x00) && (bootloader_MAC[5] != 0xFF)
			) {
				data[0] = bootloader_MAC[0];
				data[1] = bootloader_MAC[1];
				data[2] = bootloader_MAC[2];
				data[3] = bootloader_MAC[3];
				data[4] = bootloader_MAC[4];
				data[5] = bootloader_MAC[5];
			}
		} else {
			/**/
		}
	} else {
		/**/
	}
}

void __attribute__((nomips16)) WriteCoreTimer(unsigned int timer) {
    /* get the count reg */
    asm volatile("mtc0   %0, $9": "+r"(timer));
}

unsigned int __attribute__((nomips16)) ReadCoreTimer(void) {
    unsigned int timer;

    // get the count reg
    asm volatile("mfc0   %0, $9" : "=r"(timer));

    return timer;
}

unsigned int __attribute__((nomips16))  INTEnableInterrupts(void) {
    unsigned int status = 0;

    asm volatile("ei    %0" : "=r"(status));

    return status;
}

void __attribute__ ((nomips16)) INTEnableSystemMultiVectoredInt(void) {
    unsigned int val = 0;

    // set the CP0 cause IV bit high
    asm volatile("mfc0   %0,$13" : "=r"(val));
    val |= 0x00800000;
    asm volatile("mtc0   %0,$13" : "+r"(val));

    INTCONSET = _INTCON_MVEC_MASK;

    // set the CP0 status IE bit high to turn on interrupts
    INTEnableInterrupts();
}

void __attribute__((nomips16))  INTRestoreInterrupts(unsigned int status) {
    if (status & 0x00000001) {
        asm volatile("ei");
    } else {
        asm volatile("di");
	}
}

unsigned int __attribute__((nomips16)) INTDisableInterrupts(void) {
    unsigned int status = 0;

    asm volatile("di    %0" : "=r"(status));

    return status;
}

void __attribute__((noreturn)) SoftReset(void) {
	int	intStat = 0;
	int	dmaSusp = 0;
	volatile int* p = &RSWRST;

	mSYSTEMUnlock(intStat, dmaSusp);
	RSWRSTSET=_RSWRST_SWRST_MASK;
	*p;

	while(1);

}

extern inline void __attribute__((always_inline)) OSCSetPBDIV(unsigned int oscPbDiv) {
    unsigned int dma_status = 0;
    unsigned int int_status = 0;
    __OSCCONbits_t oscBits;

    mSYSTEMUnlock(int_status, dma_status);

    oscBits.w=OSCCON;       // read to be in sync. flush any pending write
    oscBits.PBDIV=0;
    oscBits.w|=oscPbDiv;
    OSCCON=oscBits.w;       // write back
    oscBits.w=OSCCON;       // make sure the write occurred before returning from this function

    mSYSTEMLock(int_status, dma_status);
}

extern inline unsigned int __attribute__((always_inline)) SYSTEMConfigPB(unsigned int sys_clock) {
    unsigned int pb_div = 0;
    unsigned int pb_clock = 0;

    pb_clock = sys_clock;

    if (sys_clock > PB_BUS_MAX_FREQ_HZ) {
        pb_div = OSC_PB_DIV_2;
        pb_clock >>= 1;
    } else {
        pb_div = OSC_PB_DIV_1;
    }

    OSCSetPBDIV(pb_div);

    return pb_clock;
}

extern inline void __attribute__((always_inline)) SYSTEMConfigWaitStates(unsigned int sys_clock) {
#ifdef _PCACHE
    unsigned int wait_states = 0;
    unsigned int int_status = 0;
#endif

#ifdef _PCACHE
    wait_states = 0;

    while(sys_clock > FLASH_SPEED_HZ) {
        wait_states++;
        sys_clock -= FLASH_SPEED_HZ;
    }

    int_status = INTDisableInterrupts();
    mCheConfigure(wait_states);
    INTRestoreInterrupts(int_status);

#endif
}

void __attribute__ ((nomips16)) CheKseg0CacheOff() {
	register unsigned long tmp = 0;

	asm("mfc0 %0,$16,0" :  "=r"(tmp));
	tmp = (tmp & ~7) | 2;
	asm("mtc0 %0,$16,0" :: "r" (tmp));
}

void __attribute__ ((nomips16)) CheKseg0CacheOn() {
	register unsigned long tmp = 0;
	asm("mfc0 %0,$16,0" :  "=r"(tmp));
	tmp = (tmp & ~7) | 3;
	asm("mtc0 %0,$16,0" :: "r" (tmp));
}

extern inline unsigned int __attribute__((always_inline)) SYSTEMConfig(unsigned int sys_clock, unsigned int flags) {
    unsigned int pb_clk = 0;
    unsigned int int_status = 0;
#ifdef _PCACHE
    unsigned int cache_status = 0;
#endif

    int_status = INTDisableInterrupts();

    mBMXDisableDRMWaitState();

    if (flags & SYS_CFG_WAIT_STATES) {
        SYSTEMConfigWaitStates(sys_clock);
    }

    if (flags & SYS_CFG_PB_BUS) {
        SYSTEMConfigPB(sys_clock);
    }


#ifdef _PCACHE
    if (flags & SYS_CFG_PCACHE) {
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
