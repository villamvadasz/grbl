#ifndef _GRBL_EEPROM_H_
#define _GRBL_EEPROM_H_

	extern void grbl_eeprom_trigger(void);
	extern void grbl_eeprom_trigger_settings(void);
	extern void grbl_eeprom_storePositionToNoInit(void);
	extern void grbl_eeprom_restorePositionFromNoInit(void);


	extern void init_grbl_eeprom(void);
	extern void do_grbl_eeprom(void);
	extern void isr_grbl_eeprom_1ms(void);

#endif