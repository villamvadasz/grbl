#include "bootloader_interface.h"
#include "mal.h"

#define BOOTLOADER_TRIGGER 0xDEAD1234

#pragma GCC diagnostic error "-w"
#ifdef __32MZ2048ECG144__
	volatile unsigned int bootloader_request_A __attribute__ ((persistent, address(0x80000000)));
	volatile unsigned int bootloader_request_B __attribute__ ((persistent, address(0x80000010)));
	volatile unsigned int bootloader_was_reset_called __attribute__ ((persistent, address(0x80000020)));
#else
	volatile unsigned int bootloader_request_A __attribute__ ((persistent, address(0xA0000000)));
	volatile unsigned int bootloader_request_B __attribute__ ((persistent, address(0xA0000010)));
	volatile unsigned int bootloader_was_reset_called __attribute__ ((persistent, address(0xA0000020)));
#endif
#pragma GCC diagnostic pop

void bootloader_interface_clearRequest(void) {
	bootloader_request_A = 0;
	bootloader_request_B = ~0;
}

void bootloader_interface_setRequest(void) {
	bootloader_request_A = BOOTLOADER_TRIGGER;
	bootloader_request_B = ~BOOTLOADER_TRIGGER;
}

unsigned int bootloader_get_bootloader_was_reset_called(void) {
	unsigned int result = bootloader_was_reset_called;
	bootloader_was_reset_called = 0;
	return result;
}

void bootloader_reset(void) {
	bootloader_was_reset_called = 0xcafecafe;
	mal_reset();
}
