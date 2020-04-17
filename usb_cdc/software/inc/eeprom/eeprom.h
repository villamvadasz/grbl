#ifndef	_EEPROM_H_
#define	_EEPROM_H_

	extern void write_eeprom_float(unsigned int adr, float data);
	extern float read_eeprom_float(unsigned int adr);

	extern void write_eeprom(unsigned int adr,unsigned char data);
	extern unsigned char read_eeprom(unsigned int adr);

	extern void write_eeprom_word(unsigned int adr, unsigned int data);
	extern unsigned int read_eeprom_word(unsigned int adr);

	extern void WriteWord(unsigned int *adr, unsigned int data);
	extern unsigned int ReadWord(unsigned int *adr);


#endif
