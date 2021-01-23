#include <string.h>
#include "noinitram.h"

#include "eep_manager.h"//CRC

#define NOINITRAM_SIZE 128

#define NOINITRAM_BASE_ADDRESS_1 0xA0000300 //use this address, because this seams to retain the values. Lower addresses still got initialized
#define NOINITRAM_CRC_SIZE 4
#define NOINITRAM_BASE_ADDRESS_2 (NOINITRAM_BASE_ADDRESS_1 + NOINITRAM_CRC_SIZE + NOINITRAM_CRC_SIZE + NOINITRAM_SIZE + 8)

#pragma GCC diagnostic error "-w"
volatile unsigned int noinitram_crc_1[2] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_1)));
volatile unsigned char noinitram_1[NOINITRAM_SIZE] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_1 + NOINITRAM_CRC_SIZE + NOINITRAM_CRC_SIZE + 8)));

volatile unsigned int noinitram_crc_2[2] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_2)));
volatile unsigned char noinitram_2[NOINITRAM_SIZE] __attribute__ ((persistent, address(NOINITRAM_BASE_ADDRESS_2 + NOINITRAM_CRC_SIZE + NOINITRAM_CRC_SIZE + 8)));
#pragma GCC diagnostic pop

unsigned int noinitram_valid_block = 0;

static void noinitram_updateCRC(void);

unsigned char noinitram_IsValid(void) {
	unsigned char result = 0;

	if (noinitram_valid_block == 0) {
		if (noinitram_crc_1[0] == ~noinitram_crc_1[1]) {
			unsigned int actual = calculate_eep_manager_crc32((unsigned char *)noinitram_crc_1, NOINITRAM_SIZE);
			unsigned int expected = noinitram_crc_1[0];
			if (actual == expected) {
				result = 1;
			}
		}
		if (result == 0) {
			if (noinitram_crc_2[0] == ~noinitram_crc_2[1]) {
				unsigned int actual = calculate_eep_manager_crc32((unsigned char *)noinitram_crc_2, NOINITRAM_SIZE);
				unsigned int expected = noinitram_crc_2[0];
				if (actual == expected) {
					result = 2;
				}
			}
		}
		noinitram_valid_block = result;
	} else {
		result = noinitram_valid_block;
	}
	
	return result;
}

int noinitram_getByte(unsigned int address) {
	int result = -1;
	if (address <= NOINITRAM_SIZE) {
		switch (noinitram_IsValid()) {
			case 1: {
				result = noinitram_1[address];
				break;
			}
			case 2: {
				result = noinitram_2[address];
				break;
			}
			default : {
				break;
			}
		}
	}
	return result;
}

void noinitram_getByteArray(unsigned int address, unsigned char *data, unsigned int length) {
	if (address <= NOINITRAM_SIZE) {
		if ((address + length) <= NOINITRAM_SIZE) {
			switch (noinitram_IsValid()) {
				case 1: {
					memcpy((void *)data, (const void *)&noinitram_1[address], length);
					break;
				}
				case 2: {
					memcpy((void *)data, (const void *)&noinitram_2[address], length);
					break;
				}
				default : {
					break;
				}
			}
		}
	}
}

void noinitram_setByte(unsigned int address, unsigned char data) {
	if (address <= NOINITRAM_SIZE) {
		noinitram_1[address] = data;
		noinitram_updateCRC();
	}
}

void noinitram_setByteArray(unsigned int address, unsigned char *data, unsigned int length) {
	if (address <= NOINITRAM_SIZE) {
		if ((address + length) <= NOINITRAM_SIZE) {
			memcpy((void *)&noinitram_1[address], (const void *)data, length);
			noinitram_updateCRC();
		}
	}
}

static void noinitram_updateCRC(void) {
	unsigned int actual = calculate_eep_manager_crc32((unsigned char *)noinitram_1, NOINITRAM_SIZE);
	noinitram_crc_1[0] = actual;
	noinitram_crc_1[1] = ~actual;
	noinitram_valid_block = 1;
	
	memcpy((void *)noinitram_2, 	(const void *)noinitram_1, 		sizeof(noinitram_2) / sizeof (unsigned char));
	memcpy((void *)noinitram_crc_2,	(const void *)noinitram_crc_1, 	sizeof(noinitram_crc_2) / sizeof (unsigned char));

}
