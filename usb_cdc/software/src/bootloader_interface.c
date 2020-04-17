#include "bootloader_interface.h"

#define BOOTLOADER_TRIGGER 0xDEAD1234

volatile unsigned int bootloader_request_A __attribute__ ((persistent, address(0x80007F00)));
volatile unsigned int bootloader_request_B __attribute__ ((persistent, address(0x80007F10)));

void bootloader_interface_clearRequest(void) {
	bootloader_request_A = 0;
	bootloader_request_B = ~0;
}

void bootloader_interface_setRequest(void) {
	bootloader_request_A = BOOTLOADER_TRIGGER;
	bootloader_request_B = ~BOOTLOADER_TRIGGER;
}
