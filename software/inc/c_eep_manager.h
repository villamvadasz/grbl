#ifndef _C_EEP_MANAGER_H_
#define _C_EEP_MANAGER_H_

	#define EEPROM_START_ADDR	0x0000
	#define EEPROM_SIZE			0x0800

	//Use 0xFF to disable version check
	//#define EEPROM_VERSION 0xFF

	#define EEPROM_VERSION 0x0B
	//#define EEPROM_INVALIDATE_OLD

	typedef enum _EepManager_Item {
		EepManager_Items_GRBL_0 = 0,
		EepManager_Items_GRBL_Settings,
		EepManager_Items_GRBL_Settings_Backup,
		EepManager_Items_GRBL_4,
		EepManager_Items_GRBL_5,
		EepManager_Items_GRBL_6,
		EepManager_Items_GRBL_7,
		EepManager_Items_RunningMinutes,
		EepManager_Items_RunningHours,
		EepManager_Items_StartUpCnt,
		EepManager_Items_SpindleRunningMinutes,
		EepManager_Items_MAC_Address,
		EepManager_Items_Tool_Length_1,
		EepManager_Items_Tool_Length_2,
		EepManager_Items_Coordinates_Teached,
		EepManager_Items_SerialNumber,
		EepManager_Items_RFE,
		EepManager_Items_LastItem,
	} EepManager_Item;

#endif
