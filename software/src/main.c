#include "main.h"
#include "c_main.h"

#include "k_stdtype.h"
#include "mal.h"
#include "sleep.h"
#include "tmr.h"
#include "stackmeasure.h"
#include "tmr.h"
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
#include "ethernet.h"
#include "fixedmemoryaddress.h"
#include "aes_128.h"
#include "rtc.h"
#ifdef USE_P256_M
	#include "p256-m.h"
#endif
#include "random.h"
#include "sha256.h"
#include "spi2.h"
#include "SD-SPI.h"

#ifndef MAKEFILE_NAME
	#error MAKEFILE_NAME not defined.
#endif

#define SW_YEAR 0x2024
#define SW_MONTH 0x01
#define SW_DAY 0x08
#define SW_TYPE TYPE_GRBL
#define SW_VERSION 0x0104

SoftwareIdentification softwareIdentification = {SW_YEAR, SW_MONTH, SW_DAY, SW_VERSION, SW_TYPE};
unsigned char VERSION_ID[] = "cnc_1_0_0";
const char VERSION_DATE[]  __attribute__ ((address(ADDRESS_VERSION_DATE))) = __DATE__;//Example May 10 2024
const char VERSION_TIME[]  __attribute__ ((address(ADDRESS_VERSION_TIME))) = __TIME__;//Example 22:11:40
#if (defined(DEFAULTS_PIC32_MX_CNC_1_0_0))
	const char VERSION_ID_FIX[]  __attribute__ ((address(ADDRESS_VERSION_ID_FIX))) = "cnc_1_0_0";
#else
	const char VERSION_ID_FIX[]  __attribute__ ((address(ADDRESS_VERSION_ID_FIX))) = "cnc_1_x_x";
#endif
const char VERSION_MAKEFILE_NAME[]  __attribute__ ((address(ADDRESS_VERSION_MAKEFILE_NAME))) = MAKEFILE_NAME;
const char FILE_CRC[]  __attribute__ ((address(ADDRESS_FILE_CRC))) = "0000CRC0";//Only the first 4 bytes are CRC, the text after it is just to help find it in the file.

const unsigned char VERSION_APP_KEY_ROM[]  		__attribute__ ((address(ADDRESS_AES_KEY_ROM))) 		= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
const unsigned char VERSION_APP_KEY_BOOT[]  	__attribute__ ((address(ADDRESS_AES_KEY_BOOT))) 	= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
const unsigned char VERSION_APP_KEY_BOOT_MAC[]  __attribute__ ((address(ADDRESS_AES_KEY_BOOT_MAC))) = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
const unsigned int VERSION_APP_SERIAL_NUMBER 	__attribute__ ((address(ADDRESS_SERIAL_NUMBER))) 	= 0xFFFFFFFF;
const unsigned char VERSION_APP_RESERVER_0[]	__attribute__ ((address(ADDRESS_SERIAL_NUMBER + 4)))= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
const unsigned char VERSION_APP_PLACEHOLDER[0x1000 - 0x40]  		__attribute__ ((address(ADDRESS_BOOTLOADER_SKIP_ERASE_START + 0x40)));

volatile unsigned char do_loopCnt = 0;
unsigned long loopCntTemp = 0;
unsigned long loopCnt = 0;
unsigned int tick_count = 0;

unsigned long loopCntHistory[32];
unsigned long loopCntHistoryLast = 0;
unsigned int loopCntHistoryCnt = 0;

unsigned int SPI2_LOCK_COLLISION_CALLOUT_cnt = 0;

__attribute__(( weak )) void init_stackmeasure(void) {}
__attribute__(( weak )) void do_stackmeasure(void) {}

__attribute__(( weak )) void init_ethernet(void) {}
__attribute__(( weak )) void do_ethernet(void) {}
__attribute__(( weak )) void isr_ethernet_1ms(void) {}
__attribute__(( weak )) BYTE SerializedMACAddress[6];
__attribute__(( weak )) void ethernet_set_mac_address(unsigned char *str) {}

__attribute__(( weak )) void init_sd_spi(void) {}
__attribute__(( weak )) void do_sd_spi(void) {}
__attribute__(( weak )) void isr_sd_spi_1ms(void) {}

__attribute__(( weak )) void init_random(void) {}
__attribute__(( weak )) void do_random(void) {}
__attribute__(( weak )) void isr_random_1ms(void) {}

__attribute__(( weak )) BYTE MDD_SDSPI_MediaDetect(void) {return 1;}

#ifdef USE_P256_M
__attribute__(( weak )) int p256_generate_random(uint8_t * output, unsigned output_size) {unsigned int x = 0; for (x=0;x<output_size;x++){output[x]=x;}return 0;}
#endif

int main (void) {
	#ifdef EXCEPTION_TESTING_ENABLED
		test_exception();
	#endif

	init_mal();

	init_stackmeasure();

	init_ad();

	init_pwm();

	#ifdef DEE_TESTING_ENABLED
		init_tmr(); //this should be the first beceaus it clears registered timers.
		init_isr();
		test_dee();
	#endif
	init_dee();
	init_random();

	init_eep_manager();
	init_task();

	init_tmr(); //this should be the first beceaus it clears registered timers.

	init_spi2();
	init_sd_spi();

	init_usb();
	init_ethernet();
	init_app();
	init_isr();

	ClearWDT();
	EnableWDT();
	
	init_grbl();
	init_aes_128();
	init_rtc();
	do_loopCnt = 0;

	bootloader_interface_clearRequest();

	while (1) {
		if (do_loopCnt) {
			do_loopCnt = 0;
			loopCnt = loopCntTemp; //Risk is here that interrupt corrupts the value, but it is taken
			loopCntHistory[loopCntHistoryCnt] = loopCnt;
			loopCntHistoryLast = loopCnt;
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
		do_ethernet();
		do_grbl();
		do_dee();
		do_random();
		do_aes_128();
		do_rtc();
		do_spi2();
		do_sd_spi();

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
	isr_ethernet_1ms();
	isr_ad_1ms();
	isr_app_1ms();
	isr_grbl_1ms();
	isr_dee_1ms();
	isr_random_1ms();
	isr_aes_128_1ms();
	isr_rtc_1ms();
	isr_spi2_1ms();
	isr_sd_spi_1ms();
}

void isr_main_100us(void) {
	isr_app_100us();
}

void SPI2_LOCK_COLLISION_CALLOUT(void) {
	SPI2_LOCK_COLLISION_CALLOUT_cnt++;
}

unsigned int ethernet_callout_wait_for_other_device(void) {
	unsigned int result = 0;//dont wait, go ahead
	
	extern BYTE MDD_SDSPI_MediaDetect(void);
	if (MDD_SDSPI_MediaDetect() == 0) {
		result = 1;
	}
	
	return result;
}
