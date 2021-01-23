#include <stdio.h>
#include <string.h>
#include "eep_manager.h"
#include "c_eep_manager.h"

#include "eeprom.h"
#include "k_stdtype.h"
#include "mal.h"

#define EEPMANAGER_ADDR_PERSISTENT 0x04
#define EEPMANAGER_ADDR_OFFSET	0x10

#define EEPMANAGER_ADDR_CRC_SIZE 1

#define EEPMANAGER_ADDR_VERSION_OFFSET 2
#define EEPMANAGER_ADDR_VERSION_ADDR (EEPROM_START_ADDR + EEPMANAGER_ADDR_VERSION_OFFSET)

#define EEPMANAGER_ADDR_DATA_START	(EEPROM_START_ADDR + EEPMANAGER_ADDR_OFFSET)
#define EEPMANAGER_ADDR_DATA_END	(EEPROM_START_ADDR + EEPROM_SIZE)

typedef enum _EEP_MANAGER_WRITE_ALL_TRIGGER_ENUM {
	EEP_MANAGER_NO_TRIGGER = 0,
	EEP_MANAGER_TRIGGERED,
} EEP_MANAGER_WRITE_ALL_TRIGGER_ENUM;

//	extern void write_eeprom_char(unsigned char adr,unsigned char data);
//	extern unsigned char read_eeprom_char(unsigned char adr);
uint32 eepManagerFindAddr(EepManager_Item item);
uint8 eepManagerReadVar(EepManager_Item item, uint32 size, void * data);
uint8 eepManagerWriteVar_Asynch(EepManager_Item item, uint32 size, const void * data);
void eepManagerValid(void);
void eepManagerEmpty(void);
void eepManagerUpdate(void);
uint8 eepManagerReadVarBackup(EepManager_Item item, uint32 size, void * data);
uint32 eepManagerConfigPersistentValue(void);

uint32 eep_manager_persistent_config = 0;
uint32 eep_manager_persistent_eeprom = 0;
uint32 eep_manager_persistent_eeprom_2 = 0;
EEP_MANAGER_WRITE_ALL_TRIGGER_ENUM eep_manager_write_all_trigger[EepManager_Items_LastItem];
volatile uint8 do_eep_manager_1ms = 0;
uint32 eep_manager_debug_address[EepManager_Items_LastItem];

void eep_manager_ReadAll(void);
	
void init_eep_manager(void) {
	eep_manager_ReadAll();
	{
		uint32 x = 0;
		for (x = 0; x < (sizeof(eep_manager_write_all_trigger) / sizeof(*eep_manager_write_all_trigger)); x++) {
			eep_manager_write_all_trigger[x] = EEP_MANAGER_NO_TRIGGER;
			eep_manager_debug_address[x] = eepManagerFindAddr(x);
		}
	}
}

void deinit_eep_manager(void) {
}

void do_eep_manager(void) {
	if (do_eep_manager_1ms) {
		do_eep_manager_1ms = 0;
		{
			static sint32 item = -1;
			if (item == -1) {
				uint32 x = 0;
				for (x = 0; x < (sizeof(eep_manager_write_all_trigger) / sizeof(*eep_manager_write_all_trigger)); x++) {
					if (eep_manager_write_all_trigger[x] != EEP_MANAGER_NO_TRIGGER) {
						item = x;
						break;
					}
				}
			}
			if (item != -1) {
				if ((item < EepManager_Items_LastItem) && (eepManager_ItemTable[item].variablePtr != NULL)) {
					if (eepManagerWriteVar_Asynch(item, eepManager_ItemTable[item].size, eepManager_ItemTable[item].variablePtr) != 0) {
						eep_manager_write_all_trigger[item] = EEP_MANAGER_NO_TRIGGER;
						item = -1;
					}
				} else {
					eep_manager_write_all_trigger[item] = EEP_MANAGER_NO_TRIGGER;
					item = -1;
				}
			}
		}
	}
}

void isr_eep_manager_1ms(void) {
	do_eep_manager_1ms = 1;
}

void eep_manager_ReadAll(void) {
	//Placed here so that value is easy to be debug
	eep_manager_persistent_config = eepManagerConfigPersistentValue();

	eep_manager_persistent_eeprom = read_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 3);
	eep_manager_persistent_eeprom <<= 8;
	eep_manager_persistent_eeprom += read_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 2);
	eep_manager_persistent_eeprom <<= 8;
	eep_manager_persistent_eeprom += read_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 1);
	eep_manager_persistent_eeprom <<= 8;
	eep_manager_persistent_eeprom += read_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 0);

	uint8 versionCheckResult = 0;
	uint8 updateEepromResult = 0;
	#ifndef EEPROM_VERSION
		#error Missing EEPROM_VERSION define in config file
	#endif
	#if (EEPROM_VERSION == 0xFF)
		versionCheckResult = 1;
	#else
		uint8 versionByte = read_eeprom_char(EEPMANAGER_ADDR_VERSION_ADDR);
		if (versionByte == EEPROM_VERSION) {
			if (eep_manager_persistent_config == eep_manager_persistent_eeprom) {
				versionCheckResult = 1;
			}
		} else if (versionByte < EEPROM_VERSION) {
			#ifndef EEPROM_INVALIDATE_OLD
				updateEepromResult = 1;
			#endif
		}
	#endif
	if (updateEepromResult) {
		eepManagerUpdate();
	} else {
		if (versionCheckResult) {
			eepManagerValid();
		} else {
			eepManagerEmpty();
		}
	}

	eep_manager_persistent_eeprom_2 = read_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 3);
	eep_manager_persistent_eeprom_2 <<= 8;
	eep_manager_persistent_eeprom_2 += read_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 2);
	eep_manager_persistent_eeprom_2 <<= 8;
	eep_manager_persistent_eeprom_2 += read_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 1);
	eep_manager_persistent_eeprom_2 <<= 8;
	eep_manager_persistent_eeprom_2 += read_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 0);
}

void eep_manager_WriteAll_Trigger(void) {
	uint8 x = 0;
	for (x = 0; x < sizeof(eep_manager_write_all_trigger) / sizeof(*eep_manager_write_all_trigger); x++) {
		eep_manager_write_all_trigger[x] = EEP_MANAGER_TRIGGERED;
	}
}

void eep_manager_WriteItem_Trigger(int item) {
	if (
		(item < EepManager_Items_LastItem) &&
		(item < sizeof(eep_manager_write_all_trigger) / sizeof(*eep_manager_write_all_trigger))
	) {
		eep_manager_write_all_trigger[item] = EEP_MANAGER_TRIGGERED;
	}
}

void eep_manager_WriteAllDefault(void) {
	eepManagerEmpty();
}

uint8 eep_manager_IsBusy(void) {
	uint8 result = 0;
	uint32 x = 0;
	for (x = 0; x < (sizeof(eep_manager_write_all_trigger) / sizeof(*eep_manager_write_all_trigger)); x++) {
		if (eep_manager_write_all_trigger[x] != EEP_MANAGER_NO_TRIGGER) {
			result = 1;
			break;
		}
	}
	return result;
}

void eepManagerValid(void) {
	//eeprom is valid
	uint32 i = 0;
	for (i = 0; i < EepManager_Items_LastItem; i++) {
		uint32 size = eepManager_ItemTable[i].size;
		void *variablePtr = eepManager_ItemTable[i].variablePtr;
		if (variablePtr != NULL) {
			uint8 result = 0;
			result = eepManagerReadVar(i, size, variablePtr);
			if (result != EEP_MANAGER_LOAD_SUCCESS) {
				eepManager_NotifyUserFailedRead(i, result);
			}
		} else {
			//do nothing
		}
	}
}

void eepManagerEmpty(void) {
	uint32 i = 0;
	for (i = 0; i < EepManager_Items_LastItem; i++) {
		uint32 size = eepManager_ItemTable[i].size;
		void *variablePtr = eepManager_ItemTable[i].variablePtr;
		const void *defaultPtr = eepManager_ItemTable[i].defaultPtr;
		if ((defaultPtr != NULL) && (variablePtr != NULL)) {
			while (eepManagerWriteVar_Asynch(i, size, defaultPtr) == 0);
			memcpy(variablePtr, defaultPtr, size);
			eepManager_NotifyUserFailedRead(i, EEP_MANAGER_LOAD_DEFAULT);
		} else {
			eepManager_NotifyUserFailedRead(i, EEP_MANAGER_LOAD_FAILED);
		}
	}
	write_eeprom_char(EEPMANAGER_ADDR_VERSION_ADDR, EEPROM_VERSION);
	
	write_eeprom_char(EEPMANAGER_ADDR_PERSISTENT,     eep_manager_persistent_config & 0xFF);
	write_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 1, (eep_manager_persistent_config >> 8) & 0xFF);
	write_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 2, (eep_manager_persistent_config >> 16) & 0xFF);
	write_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 3, (eep_manager_persistent_config >> 24) & 0xFF);
}

void eepManagerUpdate(void) {
	eepManagerValid();
	eep_manager_WriteAll_Trigger();

	write_eeprom_char(EEPMANAGER_ADDR_VERSION_ADDR, EEPROM_VERSION);
	
	write_eeprom_char(EEPMANAGER_ADDR_PERSISTENT,     eep_manager_persistent_config & 0xFF);
	write_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 1, (eep_manager_persistent_config >> 8) & 0xFF);
	write_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 2, (eep_manager_persistent_config >> 16) & 0xFF);
	write_eeprom_char(EEPMANAGER_ADDR_PERSISTENT + 3, (eep_manager_persistent_config >> 24) & 0xFF);

}

uint8 eepManagerReadVar(EepManager_Item item, uint32 size, void * data) {
	uint8 result = EEP_MANAGER_LOAD_FAILED;
	if (data != NULL) {
		uint32 addr = eepManagerFindAddr(item);
		if (size == eepManager_ItemTable[item].size) {
			uint32 i = 0;
			uint8 crc = 0;
			uint8 crcRead = read_eeprom_char(addr + size);
			for (i = 0; i < size; i++) {
				((unsigned char * )data)[i] = read_eeprom_char(addr + i);
			}
			switch (eepManager_ItemTable[item].protection) {
				case EEP_NO_CRC : {
					break;
				}
				case EEP_USE_CRC : {
					crc = calculate_eep_manager_crc((unsigned char *)data, size);
					break;
				}
				case EEP_USE_XOR : {
					crc = calculate_eep_manager_xor((unsigned char *)data, size);
					break;
				}
				case EEP_USE_SUM :
				default : {
					crc = calculate_eep_manager_sum((unsigned char *)data, size);
					break;
				}
			}

			if (eepManager_ItemTable[item].protection != EEP_NO_CRC) {
				if (crc == crcRead) {
					result = EEP_MANAGER_LOAD_SUCCESS;
				}
			} else {
				result = EEP_MANAGER_LOAD_SUCCESS;
			}
			if (result == EEP_MANAGER_LOAD_FAILED) {
				if (eepManager_ItemTable[item].backup == EEP_HAVE_BACKUP) {
					result = eepManagerReadVarBackup(item, size, data);
				}
			}
			if (result == EEP_MANAGER_LOAD_FAILED) {
				if (eepManager_ItemTable[item].defaultPtr != NULL) {
					memcpy(data, eepManager_ItemTable[item].defaultPtr , size);
					result = EEP_MANAGER_LOAD_DEFAULT;
				}
			}
		}
	}
	return result;
}

uint8 eepManagerReadVarBackup(EepManager_Item item, uint32 size, void * data) {
	uint8 result = EEP_MANAGER_LOAD_FAILED;
	if (data != NULL) {
		uint32 addr = eepManagerFindAddr(item);
		if (size == eepManager_ItemTable[item].size) {
			addr += size + EEPMANAGER_ADDR_CRC_SIZE;

			uint32 i = 0;
			uint8 crc = 0;
			uint8 crcRead = read_eeprom_char(addr + size);
			for (i = 0; i < size; i++) {
				((unsigned char * )data)[i] = read_eeprom_char(addr + i);
			}
			switch (eepManager_ItemTable[item].protection) {
				case EEP_NO_CRC : {
					break;
				}
				case EEP_USE_CRC : {
					crc = calculate_eep_manager_crc((unsigned char *)data, size);
					break;
				}
				case EEP_USE_XOR : {
					crc = calculate_eep_manager_xor((unsigned char *)data, size);
					break;
				}
				case EEP_USE_SUM :
				default : {
					crc = calculate_eep_manager_sum((unsigned char *)data, size);
					break;
				}
			}

			if (eepManager_ItemTable[item].protection != EEP_NO_CRC) {
				if (crc == crcRead) {
					result = EEP_MANAGER_LOAD_SUCCESS;
				}
			} else {
				result = EEP_MANAGER_LOAD_SUCCESS;
			}
			if (result == EEP_MANAGER_LOAD_FAILED) {
				if (eepManager_ItemTable[item].defaultPtr != NULL) {
					memcpy(data, eepManager_ItemTable[item].defaultPtr , size);
					result = EEP_MANAGER_LOAD_DEFAULT;
				}
			}
		}
	}
	return result;
}

uint8 eepManagerWriteVar_Asynch(EepManager_Item item, uint32 size, const void * data) {
	uint8 finished = 0;
	
	static uint8 eepManagerWriteVar_AsynchSm = 0;
	static uint32 addr = 0;
	static uint32 eepManagerWriteVar_AsynchSm_i = 0;
	static uint32 second_item = 0;
	static uint8 crc = 0;
	switch (eepManagerWriteVar_AsynchSm) {
		case 0 : {
			if (data != NULL) {
				addr = eepManagerFindAddr(item);
				if (size == eepManager_ItemTable[item].size) {
					eepManagerWriteVar_AsynchSm_i = 0;
					crc = 0;
					switch (eepManager_ItemTable[item].protection) {
						case EEP_NO_CRC : {
							break;
						}
						case EEP_USE_CRC : {
							crc = calculate_eep_manager_crc((unsigned char *)data, size);
							break;
						}
						case EEP_USE_XOR : {
							crc = calculate_eep_manager_xor((unsigned char *)data, size);
							break;
						}
						case EEP_USE_SUM :
						default : {
							crc = calculate_eep_manager_sum((unsigned char *)data, size);
							break;
						}
					}
					eepManagerWriteVar_AsynchSm = 3;
				} else {
					eepManagerWriteVar_AsynchSm = 0;
					finished = 2;
				}
			} else {
				finished = 2;
			}
			break;
		}
		case 3 : {
			uint8 dataTemp = read_eeprom_char(addr + eepManagerWriteVar_AsynchSm_i);
			uint8 defaultDataTemp = ((unsigned char * )data)[eepManagerWriteVar_AsynchSm_i];
			if (dataTemp != defaultDataTemp) {
				write_eeprom_char(addr + eepManagerWriteVar_AsynchSm_i, defaultDataTemp);
			}
			eepManagerWriteVar_AsynchSm_i++;
			if (eepManagerWriteVar_AsynchSm_i >= size) {
				eepManagerWriteVar_AsynchSm = 4;
			}
			break;
		}
		case 4 : {
			if (eepManager_ItemTable[item].protection != EEP_NO_CRC) {
				write_eeprom_char(addr + size, crc);
			}
			if (eepManager_ItemTable[item].backup == EEP_HAVE_BACKUP) {
				second_item = eepManagerWriteVar_AsynchSm_i + EEPMANAGER_ADDR_CRC_SIZE;
				eepManagerWriteVar_AsynchSm_i = 0;
				eepManagerWriteVar_AsynchSm = 45;
			} else {
				eepManagerWriteVar_AsynchSm = 0;
				finished = 1;
			}
			break;
		}
		case 45 : {
			uint8 dataTemp = read_eeprom_char(addr + second_item + eepManagerWriteVar_AsynchSm_i);
			uint8 defaultDataTemp = ((unsigned char * )data)[eepManagerWriteVar_AsynchSm_i];
			if (dataTemp != defaultDataTemp) {
				write_eeprom_char(addr + second_item + eepManagerWriteVar_AsynchSm_i, defaultDataTemp);
			}
			eepManagerWriteVar_AsynchSm_i++;
			if (eepManagerWriteVar_AsynchSm_i >= size) {
				eepManagerWriteVar_AsynchSm = 46;
			}
			break;
		}
		case 46 : {
			if (eepManager_ItemTable[item].protection != EEP_NO_CRC) {
				write_eeprom_char(addr + second_item + size, crc);
			}
			eepManagerWriteVar_AsynchSm = 0;
			finished = 1;
			break;
		}
		default : {
			eepManagerWriteVar_AsynchSm = 0;
			break;
		}
	}
	return finished;
}

uint32 eepManagerFindAddr(EepManager_Item item) {
	uint32 i = 0;
	uint32 result = EEPMANAGER_ADDR_DATA_START;
	for (i = 0; i < item; i++) {
		uint32 item_result = 0;
		item_result = eepManager_ItemTable[i].size;
		item_result += EEPMANAGER_ADDR_CRC_SIZE;
		if (eepManager_ItemTable[i].backup == EEP_HAVE_BACKUP) {
			item_result *= 2;
		}
		result += item_result;
	}
	return result;
}

uint32 eepManagerConfigPersistentValue(void) {
	uint32 x = 0;
	uint32 persistentValue = 0xFFFFFFFF;
	for (x = 0; x < EepManager_Items_LastItem; x++) {
		persistentValue = calculate_eep_manager_crc32_special( (uint8 *)&eepManager_ItemTable[x].item, 			sizeof(eepManager_ItemTable[x].item), persistentValue);
		persistentValue = calculate_eep_manager_crc32_special( (uint8 *)&eepManager_ItemTable[x].size, 			sizeof(eepManager_ItemTable[x].size), persistentValue);
		//persistentValue = calculate_eep_manager_crc32_special( (uint8 *)&eepManager_ItemTable[x].variablePtr, 	sizeof(eepManager_ItemTable[x].variablePtr), persistentValue);
		//persistentValue = calculate_eep_manager_crc32_special( (uint8 *)&eepManager_ItemTable[x].defaultPtr, 	sizeof(eepManager_ItemTable[x].defaultPtr), persistentValue);
		persistentValue = calculate_eep_manager_crc32_special( (uint8 *)&eepManager_ItemTable[x].protection, 	sizeof(eepManager_ItemTable[x].protection), persistentValue);
		persistentValue = calculate_eep_manager_crc32_special( (uint8 *)&eepManager_ItemTable[x].backup, 		sizeof(eepManager_ItemTable[x].backup), persistentValue);
	}
	return persistentValue;
}
