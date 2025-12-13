#include <string.h>
#include "bootloader_interface.h"
#include "mal.h"
#include "fixedmemoryaddress.h"

#define BOOTLOADER_TRIGGER 0xDEAD1234

#pragma GCC diagnostic error "-W"
#ifdef __32MZ2048ECG144__
	volatile unsigned char bootloader_MAC[8] __attribute__ ((persistent, address(ADDRESS_bootloader_MAC)));
	volatile unsigned int bootloader_request_A __attribute__ ((persistent, address(ADDRESS_bootloader_request_A)));
	volatile unsigned int bootloader_request_B __attribute__ ((persistent, address(ADDRESS_bootloader_request_B)));
	volatile unsigned int bootloader_was_reset_called __attribute__ ((persistent, address(ADDRESS_bootloader_was_reset_called)));
#else
	volatile unsigned char bootloader_MAC[8] __attribute__ ((persistent, address(ADDRESS_bootloader_MAC)));
	volatile unsigned int bootloader_request_A __attribute__ ((persistent, address(ADDRESS_bootloader_request_A)));
	volatile unsigned int bootloader_request_B __attribute__ ((persistent, address(ADDRESS_bootloader_request_B)));
	volatile unsigned int bootloader_was_reset_called __attribute__ ((persistent, address(ADDRESS_bootloader_was_reset_called)));
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

void bootloader_write_mac(unsigned char *data) {
	if (data != NULL) {
		bootloader_MAC[6] = 0x00;
		bootloader_MAC[7] = 0x00;
		memcpy((unsigned char *)bootloader_MAC, data, 6);
		bootloader_MAC[6] = 0xCA;
		bootloader_MAC[7] = 0xFE;
	}
}
