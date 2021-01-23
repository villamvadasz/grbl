#ifndef _EEP_MANAGER_H_
#define _EEP_MANAGER_H_

	#include "k_stdtype.h"
	#include "c_eep_manager.h"

	#define EEP_NO_CRC 0
	#define EEP_USE_CRC 1
	#define EEP_USE_XOR 2
	#define EEP_USE_SUM 3

	#define EEP_NO_BACKUP   0
	#define EEP_HAVE_BACKUP 1

	#define EEP_MANAGER_LOAD_FAILED 0
	#define EEP_MANAGER_LOAD_SUCCESS 1
	#define EEP_MANAGER_LOAD_DEFAULT 2

	typedef struct _EepManager_ItemTable {
		int item;
		uint32 size;
		void *variablePtr;
		void *defaultPtr;
		uint8 protection;
		uint8 backup;
	} EepManager_ItemTable;

	extern EepManager_ItemTable eepManager_ItemTable[EepManager_Items_LastItem];

	extern void eep_manager_WriteAll_Trigger(void);
	extern void eep_manager_WriteItem_Trigger(int item);
	extern void eep_manager_WriteAllDefault(void);
	extern uint8 eep_manager_IsBusy(void);

	extern void eepManager_NotifyUserFailedRead(int item, uint8 type);

	extern uint8 calculate_eep_manager_crc(uint8 *data, uint16 size);
	extern uint32 calculate_eep_manager_crc32(uint8 *data, uint16 size);
	extern uint8 calculate_eep_manager_xor(uint8 *data, uint16 size);
	extern uint8 calculate_eep_manager_sum(uint8 *data, uint16 size);
	extern uint32 calculate_eep_manager_crc32_special(uint8 *data, uint16 size, uint32 crc);

	extern void init_eep_manager(void);
	extern void deinit_eep_manager(void);
	extern void do_eep_manager(void);
	extern void isr_eep_manager_1ms(void);


#endif
