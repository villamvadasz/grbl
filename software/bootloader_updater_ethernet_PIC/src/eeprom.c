#include "eeprom.h"

#include "dee.h"
#include "k_stdtype.h"

#define EEPROM_ENABLE_WRITE_BUFFER

#ifdef EEPROM_ENABLE_WRITE_BUFFER
	#define EEPROM_WRITE_BUFFER 128
	typedef struct _Eeprom_write_buffer {
		unsigned int addr;
		unsigned char data;
	} Eeprom_write_buffer;

	Eeprom_write_buffer eeprom_write_buffer[EEPROM_WRITE_BUFFER];
	uint32 eeprom_write_buffer_write_cnt = 0;
	uint32 eeprom_write_buffer_read_cnt = 0;
#endif

volatile uint32 do_eeprom_1ms = 0;

void init_eeprom(void) {
}

void do_eeprom(void) {
	if (do_eeprom_1ms) {
		do_eeprom_1ms = 0;
		{
			#ifdef EEPROM_ENABLE_WRITE_BUFFER
				if (is_eeprom_write_protected() == 0) {
					if (eeprom_write_buffer_write_cnt != eeprom_write_buffer_read_cnt) {
						write_eeprom_char(eeprom_write_buffer[eeprom_write_buffer_read_cnt].addr, eeprom_write_buffer[eeprom_write_buffer_read_cnt].data);
						eeprom_write_buffer_read_cnt++;
						if (eeprom_write_buffer_write_cnt == eeprom_write_buffer_read_cnt) {
							eeprom_write_buffer_write_cnt = 0;
							eeprom_write_buffer_read_cnt = 0;
						}
					}
				}
			#endif
		}
	}
}

void isr_eeprom_1ms(void) {
	do_eeprom_1ms = 1;
}

void deinit_eeprom(void) {
}

unsigned char is_eeprom_write_protected(void) {
	unsigned char result = 1;
	
	if (dee_is_pack_running() == 0) {
		result = 0; //No reorganization is running. No write protection
	}

	return result;
}

void write_eeprom_char(unsigned int addr, unsigned char data) {
	if (is_eeprom_write_protected() == 0) {
		uint32 dataTemp = 0;
		uint32 dataTempNew = data;
		uint32 mask = 0xFF;
		dataTemp = dee_read_eeprom(addr / 4);
		mask = ~(mask << ( (addr % 4) * 8 ) );
		dataTemp &= mask;
		dataTempNew = (dataTempNew << ( (addr % 4) * 8 ) );
		dataTemp |= dataTempNew;
		dee_write_eeprom((uint32)addr / 4, (uint32)dataTemp);
	} else {
		#ifdef EEPROM_ENABLE_WRITE_BUFFER
			if (eeprom_write_buffer_write_cnt < EEPROM_WRITE_BUFFER) {
				eeprom_write_buffer[eeprom_write_buffer_write_cnt].addr = addr;
				eeprom_write_buffer[eeprom_write_buffer_write_cnt].data = data;
				eeprom_write_buffer_write_cnt++;
			}
		#endif
	}
}

unsigned char read_eeprom_char(unsigned int addr) {
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
		write_eeprom_char(addr + x, ((unsigned char *)&data)[x]);
	}
}

float read_eeprom_float (unsigned int addr) {
	float result = 0;
	unsigned char x = 0;
	for (x = 0; x < sizeof(float) / sizeof(unsigned char); x++) {
		((unsigned char *)&result)[x] = read_eeprom_char(addr + x);
	}
	return result;
}

void write_eeprom_int(unsigned int addr, unsigned int data) {
	unsigned char x = 0;

	for (x = 0; x < sizeof(unsigned int) / sizeof(unsigned char); x++) {
		write_eeprom_char(addr + x, ((unsigned char *)&data)[x]);
	}
}

unsigned int read_eeprom_int(unsigned int addr) {
	unsigned int result = 0;
	unsigned char x = 0;
	for (x = 0; x < sizeof(unsigned int) / sizeof(unsigned char); x++) {
		((unsigned char *)&result)[x] = read_eeprom_char(addr + x);
	}
	return result;
}
