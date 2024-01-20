#include "eeprom.h"

#include "dee.h"
#include "k_stdtype.h"

void write_eeprom(unsigned int addr, unsigned char data) {
	uint32 dataTemp = 0;
	uint32 dataTempNew = data;
	uint32 mask = 0xFF;
	dataTemp = dee_read_eeprom(addr / 4);
	mask = ~(mask << ( (addr % 4) * 8 ) );
	dataTemp &= mask;
	dataTempNew = (dataTempNew << ( (addr % 4) * 8 ) );
	dataTemp |= dataTempNew;
	dee_write_eeprom((uint32)addr / 4, (uint32)dataTemp);
}

unsigned char read_eeprom(unsigned int addr) {
	unsigned char result = 0;
	uint32 dataTemp = 0;
	dataTemp = dee_read_eeprom(addr / 4);
	dataTemp = (dataTemp >> ( (addr % 4) * 8 )) & 0xFF;
	result = dataTemp;
	return result;
}

void write_eeprom_float(unsigned int addr, float data) {
	unsigned char x = 0;

	for (x = 0; x < sizeof(float) / sizeof(unsigned char); x++) {
		write_eeprom (addr + x, ((unsigned char *)&data)[x]);
	}
}

float read_eeprom_float (unsigned int addr) {
	float result = 0;
	unsigned char x = 0;
	for (x = 0; x < sizeof(float) / sizeof(unsigned char); x++) {
		((unsigned char *)&result)[x] = read_eeprom (addr + x);
	}
	return result;
}

void write_eeprom_word(unsigned int addr, unsigned int data) {
	unsigned char x = 0;

	for (x = 0; x < sizeof(unsigned int) / sizeof(unsigned char); x++) {
		write_eeprom (addr + x, ((unsigned char *)&data)[x]);
	}
}

unsigned int read_eeprom_word(unsigned int addr) {
	unsigned int result = 0;
	unsigned char x = 0;
	for (x = 0; x < sizeof(unsigned int) / sizeof(unsigned char); x++) {
		((unsigned char *)&result)[x] = read_eeprom (addr + x);
	}
	return result;
}
