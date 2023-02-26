/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "c_bootloader.h"
#include "definitions.h"                // SYS function prototypes
#include "configuration.h"
#include "device.h"

#ifndef BOOTLOADER_IS_AN_UPDATER_APPLICATION
   	#ifdef __32MZ2048EFM100__
        #ifdef XOSC_24MHZ
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    EC
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_POSC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_24MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif  
        #ifdef XTAL_24MHZ
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    HS
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_POSC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_24MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef INT_OSC_8MHZ
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    OFF
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_FRC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_24MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef INT_OSC_8MHZ_EC_12
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    EC
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_FRC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_12MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef INT_OSC_8MHZ_HS_12
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    HS
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_FRC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_12MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef INT_OSC_8MHZ_EC_24
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    EC
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_FRC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_24MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef INT_OSC_8MHZ_HS_24
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    HS
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_FRC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_24MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #pragma config POSCGAIN =   GAIN_LEVEL_3
        #pragma config IESO =       ON
        #pragma config FCKSM =      CSECME

        #pragma config DEBUG =      OFF
        #pragma config JTAGEN =     OFF
        #pragma config ICESEL =     ICS_PGx2
        #pragma config TRCEN =      OFF
        #pragma config BOOTISA =    MIPS32
        #pragma config FECCCON =    OFF_UNLOCKED
        #pragma config FSLEEP =     OFF
        #pragma config DBGPER =     PG_ALL
        #pragma config SMCLR =      MCLR_NORM
        #pragma config SOSCGAIN =   GAIN_LEVEL_3
        #pragma config SOSCBOOST =  ON
        #pragma config POSCBOOST =  ON
        #pragma config EJTAGBEN =   NORMAL
        #pragma config CP =         OFF

        #pragma config DMTINTV =    WIN_127_128
        #pragma config FSOSCEN =    OFF
        #pragma config OSCIOFNC =   OFF
        #pragma config WDTPS =      PS1048576
        #pragma config WDTSPGM =    STOP
        #pragma config FWDTEN =     OFF
        #pragma config WINDIS =     NORMAL
        #pragma config FWDTWINSZ =  WINSZ_25
        #pragma config DMTCNT =     DMT31
        #pragma config FDMTEN =     OFF

        #pragma config USERID =     0xffff
        #pragma config FMIIEN =     OFF
        #pragma config FETHIO =     OFF
        #pragma config PGL1WAY =    OFF
        #pragma config PMDL1WAY =   OFF
        #pragma config IOL1WAY =    OFF
        #pragma config FUSBIDIO =   OFF

        #pragma config TSEQ =       0xffff
        #pragma config CSEQ =       0x0
    #endif
   	#ifdef __32MZ2048ECG144__
        #ifdef XOSC_24MHZ
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    EC
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_POSC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_24MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif  
        #ifdef XTAL_24MHZ
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    HS
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_POSC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_24MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef XTAL_12MHZ
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    HS
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_POSC
            #pragma config FPLLMULT =   MUL_100
            #pragma config UPLLFSEL =   FREQ_12MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef INT_OSC_8MHZ
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    OFF
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_FRC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_24MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef INT_OSC_8MHZ_EC_12
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    EC
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_FRC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_12MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef INT_OSC_8MHZ_HS_12
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    HS
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_FRC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_12MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef INT_OSC_8MHZ_EC_24
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    EC
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_FRC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_24MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #ifdef INT_OSC_8MHZ_HS_24
            #pragma config FNOSC =      SPLL
            #pragma config POSCMOD =    HS
            #pragma config FPLLIDIV =   DIV_3
            #pragma config FPLLRNG =    RANGE_5_10_MHZ
            #pragma config FPLLICLK =   PLL_FRC
            #pragma config FPLLMULT =   MUL_50
            #pragma config UPLLFSEL =   FREQ_24MHZ
            #ifdef SYS_FREQ_100MHZ
                #pragma config FPLLODIV =   DIV_4
            #endif
            #ifdef SYS_FREQ_200MHZ
                #pragma config FPLLODIV =   DIV_2
            #endif
        #endif
        #pragma config IESO =       ON
        #pragma config FCKSM =      CSECME

        #pragma config DEBUG =      OFF
        #pragma config JTAGEN =     OFF
        #pragma config ICESEL =     ICS_PGx2
        #pragma config TRCEN =      OFF
        #pragma config BOOTISA =    MIPS32
        #pragma config FECCCON =    OFF_UNLOCKED
        #pragma config FSLEEP =     OFF
        #pragma config DBGPER =     PG_ALL
        #pragma config EJTAGBEN =   NORMAL
        #pragma config CP =         OFF

        #pragma config DMTINTV =    WIN_127_128
        #pragma config FSOSCEN =    OFF
        #pragma config OSCIOFNC =   OFF
        #pragma config WDTPS =      PS1048576
        #pragma config WDTSPGM =    STOP
        #pragma config FWDTEN =     OFF
        #pragma config WINDIS =     NORMAL
        #pragma config FWDTWINSZ =  WINSZ_25
        #pragma config DMTCNT =     DMT31
        #pragma config FDMTEN =     OFF

        #pragma config USERID =     0xffff
        #pragma config FMIIEN =     OFF
        #pragma config FETHIO =     OFF
        #pragma config PGL1WAY =    OFF
        #pragma config PMDL1WAY =   OFF
        #pragma config IOL1WAY =    OFF
        #pragma config FUSBIDIO =   OFF

        #pragma config TSEQ =       0xffff
        #pragma config CSEQ =       0x0
    #endif
#endif

const DRV_USBHS_INIT drvUSBInit =
{
    /* Interrupt Source for USB module */
    .interruptSource = INT_SOURCE_USB,

	/* Interrupt Source for USB module */
    .interruptSourceUSBDma = INT_SOURCE_USB_DMA,
	
    /* System module initialization */
    .moduleInit = {0},

    /* USB Controller to operate as USB Device */
    .operationMode = DRV_USBHS_OPMODE_DEVICE,

	/* Enable High Speed Operation */
    .operationSpeed = USB_SPEED_HIGH,
    
    /* Stop in idle */
    .stopInIdle = true,

    /* Suspend in sleep */
    .suspendInSleep = false,

    /* Identifies peripheral (PLIB-level) ID */
    .usbID = USBHS_ID_0,
	
};

SYSTEM_OBJECTS sysObj;

void clock_switch(void);

int main ( void )
{
    /* Start out with interrupts disabled before configuring any modules */
    __builtin_disable_interrupts();
    
    clock_switch();

    /* unlock system for clock configuration */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    /* Peripheral Module Disable Configuration */
    PMD1 = 0x1001;
    PMD2 = 0x3;
    PMD3 = 0x1ff01ff;
    PMD4 = 0x1ff;
    PMD5 = 0x301f3f3f;
    PMD6 = 0x10830001;
    PMD7 = 0x500000;

    /* Lock system since done with clock configuration */
    SYSKEY = 0x33333333;
    
    /* Configure Prefetch, Wait States and ECC */
    PRECONbits.PREFEN = 3;
    PRECONbits.PFMWS = 3;
    CFGCONbits.ECCCON = 3;

    if (bootloader_Trigger()) {
        NVM_Initialize();
        sysObj.drvUSBHSObject = DRV_USBHS_Initialize(DRV_USBHS_INDEX_0, (SYS_MODULE_INIT *) &drvUSBInit);	
        sysObj.usbDevObject0 = USB_DEVICE_Initialize (USB_DEVICE_INDEX_0 , ( SYS_MODULE_INIT* ) & usbDevInitData);
        EVIC_Initialize();
        __builtin_enable_interrupts();

        //{
        //    TRISEbits.TRISE3 = 0;
        //    LATEbits.LATE3 = 1;
        //}

        while ( !ExitFirmwareUpgradeMode() )
        {
            bootloader_Tasks();
            DRV_USBHS_Tasks(sysObj.drvUSBHSObject);
            USB_DEVICE_Tasks(sysObj.usbDevObject0);
        }


        DATASTREAM_Close();
        __builtin_disable_interrupts();
        EVIC_Deinitialize();
        USB_DEVICE_Deinitialize (sysObj.usbDevObject0);
        DRV_USBHS_Deinitialize(DRV_USBHS_INDEX_0);	
        NVM_Deinitialize();

        //{
        //    TRISEbits.TRISE3 = 0;
        //    LATEbits.LATE3 = 0;
        //}        
    }

    bootloader_run_Application();
    bootloader_TriggerReset();
    
    while (1);
    
    return ( EXIT_FAILURE );
}

void clock_switch(void) {
}