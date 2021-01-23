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
#include "ethernet.h"

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

volatile unsigned char do_loopCnt = 0;
unsigned long loopCntTemp = 0;
unsigned long loopCnt = 0;
unsigned int tick_count = 0;

unsigned long loopCntHistory[60];
unsigned int loopCntHistoryCnt = 0;

__attribute__(( weak )) void init_ethernet(void) {} 
__attribute__(( weak )) void do_ethernet(void) {} 
__attribute__(( weak )) void isr_ethernet_1ms(void) {} 
__attribute__(( weak )) BYTE SerializedMACAddress[6];
__attribute__(( weak )) void ethernet_set_mac_address(unsigned char *str) {} 


int main (void) {

	init_mal();

	init_stackmeasure();
	init_ad();

	init_pwm();
	init_dee();
	init_eep_manager();
	init_task();

	init_tmr(); //this should be the first beceaus it clears registered timers.

	init_usb();
	init_ethernet();
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
		do_ethernet();
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
	isr_ethernet_1ms();
	isr_ad_1ms();
	isr_app_1ms();
	isr_grbl_1ms();
	isr_dee_1ms();
}

void isr_main_100us(void) {
	isr_app_100us();
}
