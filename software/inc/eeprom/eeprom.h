#ifndef	_EEPROM_H_
#define	_EEPROM_H_

	extern void init_eeprom(void);
	extern void do_eeprom(void);
	extern void isr_eeprom_1ms(void);
	extern void deinit_eeprom(void);

	extern unsigned char is_eeprom_write_protected(void);

	extern void write_eeprom_char(unsigned int adr, unsigned char data);
	extern unsigned char read_eeprom_char(unsigned int adr);

	extern void write_eeprom_float(unsigned int adr, float data);
	extern float read_eeprom_float(unsigned int adr);

	extern void write_eeprom_int(unsigned int adr, unsigned int data);
	extern unsigned int read_eeprom_int(unsigned int adr);

#endif
