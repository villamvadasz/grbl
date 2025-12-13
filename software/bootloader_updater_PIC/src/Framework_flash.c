#include <string.h>
#include "c_bootloader.h"
#include "Framework.h"
#include "NVMem.h"
#include "my_plib.h"
#include "BootLoader.h"
#include "HardwareProfile.h"

#if defined(__PIC32MX1XX_2XX__)
	// PIC32MX1xx and PIC32MX2xx devices
	#define FLASH_PAGE_SIZE		 		1024
	#define DEV_CONFIG_REG_BASE_ADDRESS 0x9FC00BF0
	#define DEV_CONFIG_REG_END_ADDRESS   0x9FC00BFF
#elif defined(__PIC32MX3XX_7XX__)
	// PIC32MX3xx to PIC32MX7xx devices
	#define FLASH_PAGE_SIZE		 		4096
	#define DEV_CONFIG_REG_BASE_ADDRESS 0x9FC02FF0
	#define DEV_CONFIG_REG_END_ADDRESS   0x9FC02FFF
#endif

#define DATA_RECORD 		0
#define END_OF_FILE_RECORD 	1
#define EXT_SEG_ADRS_RECORD 2
#define EXT_LIN_ADRS_RECORD 4

const UINT16 crc_table[16] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};
T_HEX_RECORD HexRecordSt;

volatile int debug_andras = 0;

unsigned int WriteHexRecord2Flash(UINT8* HexRecord, UINT totalHexRecLen) {
	unsigned int result = 0;
	UINT8 Checksum = 0;
	UINT i = 0;
	UINT WrData = 0;
	UINT RdData = 0;
	void* ProgAddress = NULL;
	UINT nextRecStartPt = 0;
	UINT8 temp[4] = {0};

	while(totalHexRecLen>=5) { // A hex record must be atleast 5 bytes. (1 Data Len byte + 1 rec type byte+ 2 address bytes + 1 crc)
		HexRecord = &HexRecord[nextRecStartPt];
		HexRecordSt.RecDataLen = HexRecord[0];
		HexRecordSt.RecType = HexRecord[3];
		HexRecordSt.Data = &HexRecord[4];

		//Determine next record starting point.
		nextRecStartPt = HexRecordSt.RecDataLen + 5;

		// Decrement total hex record length by length of current record.
		totalHexRecLen = totalHexRecLen - nextRecStartPt;

		// Hex Record checksum check.
		Checksum = 0;
		for(i = 0; i < HexRecordSt.RecDataLen + 5; i++) {
			Checksum += HexRecord[i];
		}

		if (Checksum != 0) {
			//Error. Hex record Checksum mismatch.
		} else {
			// Hex record checksum OK.
			switch(HexRecordSt.RecType) {
				case DATA_RECORD: { //Record Type 00, data record.
					HexRecordSt.Address.byte.MB = 0;
					HexRecordSt.Address.byte.UB = 0;
					HexRecordSt.Address.byte.HB = HexRecord[1];
					HexRecordSt.Address.byte.LB = HexRecord[2];

					// Derive the address.
					HexRecordSt.Address.Val = HexRecordSt.Address.Val + HexRecordSt.ExtLinAddress.Val + HexRecordSt.ExtSegAddress.Val;

					while(HexRecordSt.RecDataLen) {// Loop till all bytes are done.
						// Convert the Physical address to Virtual address. 
						ProgAddress = (void *)PA_TO_KVA0(HexRecordSt.Address.Val);
						
						if (ProgAddress == (void *)0x9D012180uL) {
							debug_andras++;
							debug_andras++;
							debug_andras++;
							debug_andras++;
						}

						#ifdef BOOT_FLASH_BASE_ADDRESS
							// Enable write everywhere
							if (
								((ProgAddress >= (void *)APP_FLASH_BASE_ADDRESS) && (ProgAddress <= (void *)APP_FLASH_END_ADDRESS)) || 
								((ProgAddress >= (void *)BOOT_FLASH_BASE_ADDRESS) && (ProgAddress <= (void *)BOOT_FLASH_END_ADDRESS))
								//((ProgAddress >= (void*)DEV_CONFIG_REG_BASE_ADDRESS) && (ProgAddress <= (void*)DEV_CONFIG_REG_END_ADDRESS))
							)
						#else
						// Make sure we are not writing boot area and device configuration bits.
							if (
								((ProgAddress >= (void *)APP_FLASH_BASE_ADDRESS) && (ProgAddress <= (void *)APP_FLASH_END_ADDRESS)) &&
								((ProgAddress < (void*)DEV_CONFIG_REG_BASE_ADDRESS) || (ProgAddress > (void*)DEV_CONFIG_REG_END_ADDRESS))
							)
						#endif
						{
							#ifdef BOOTLOADER_WITH_DEE
								if ((ProgAddress >= (void *)EEPROM_FLASH_BASE_ADDRESS) && (ProgAddress <= (void *)EEPROM_FLASH_END_ADDRESS)) {
								}
								else
							#endif
							{ // skip eeprom address
								if (HexRecordSt.RecDataLen < 4) {
									// Sometimes record data length will not be in multiples of 4. Appending 0xFF will make sure that..
									// we don't write junk data in such cases.
									WrData = 0xFFFFFFFF;
									memcpy(&WrData, HexRecordSt.Data, HexRecordSt.RecDataLen);
								} else {
									memcpy(&WrData, HexRecordSt.Data, 4);
								}

								// Write the data into flash.
								// Assert on error. This must be caught during debug phase.
								if (NVMemWriteWord(ProgAddress, WrData) != 0) {
									result = 1;
								}
							}
						}

						// Increment the address.
						HexRecordSt.Address.Val += 4;
						// Increment the data pointer.
						HexRecordSt.Data += 4;
						// Decrement data len.
						if (HexRecordSt.RecDataLen > 3) {
							HexRecordSt.RecDataLen -= 4;
						} else {
							HexRecordSt.RecDataLen = 0;
						}
					}
					break;
				}
				case EXT_SEG_ADRS_RECORD : {  // Record Type 02, defines 4th to 19th bits of the data address.
					HexRecordSt.ExtSegAddress.byte.MB = 0;
					HexRecordSt.ExtSegAddress.byte.UB = HexRecordSt.Data[0];
					HexRecordSt.ExtSegAddress.byte.HB = HexRecordSt.Data[1];
					HexRecordSt.ExtSegAddress.byte.LB = 0;
					// Reset linear address.
					HexRecordSt.ExtLinAddress.Val = 0;
					break;
				}
				case EXT_LIN_ADRS_RECORD : {  // Record Type 04, defines 16th to 31st bits of the data address. 
					HexRecordSt.ExtLinAddress.byte.MB = HexRecordSt.Data[0];
					HexRecordSt.ExtLinAddress.byte.UB = HexRecordSt.Data[1];
					HexRecordSt.ExtLinAddress.byte.HB = 0;
					HexRecordSt.ExtLinAddress.byte.LB = 0;
					// Reset segment address.
					HexRecordSt.ExtSegAddress.Val = 0;
					break;
				}
				case END_OF_FILE_RECORD:  //Record Type 01, defines the end of file record.
				default: {
					HexRecordSt.ExtSegAddress.Val = 0;
					HexRecordSt.ExtLinAddress.Val = 0;
					break;
				}
			}
		}
	}//while(1)
	return result;
}

UINT16 CalculateCrc(UINT8 *data, UINT32 len) {
	UINT i;
	UINT16 crc = 0;

	while(len--) {
		i = (crc >> 12) ^ (*data >> 4);
		crc = crc_table[i & 0x0F] ^ (crc << 4);
		i = (crc >> 12) ^ (*data >> 0);
		crc = crc_table[i & 0x0F] ^ (crc << 4);
		data++;
	} 

	return (crc & 0xFFFF);
}

#ifdef BOOTLOADER_WITH_DEE
UINT16 CalculateCrc_WithoutEeprom(UINT8 *data, UINT32 len) {
	UINT i;
	UINT16 crc = 0;
	UINT8 data2 = 0;

	while(len--) {
		if (
			(((unsigned int)data >= EEPROM_FLASH_BASE_ADDRESS) && ((unsigned int)data <= EEPROM_FLASH_END_ADDRESS))
		) {
			i = (crc >> 12) ^ (0xFF >> 4);
			crc = crc_table[i & 0x0F] ^ (crc << 4);
			i = (crc >> 12) ^ (0xFF >> 0);
			crc = crc_table[i & 0x0F] ^ (crc << 4);
			data2 = 0xFF;
		} else {
			i = (crc >> 12) ^ (*data >> 4);
			crc = crc_table[i & 0x0F] ^ (crc << 4);
			i = (crc >> 12) ^ (*data >> 0);
			crc = crc_table[i & 0x0F] ^ (crc << 4);
			data2 = *data;
		}
		CryptoSignatureCheck_add(NULL, data2);
		data++;
	} 

	return (crc & 0xFFFF);
}
#endif

unsigned int Framework_EraseFlash(void) {
	unsigned int result = 0;
	UINT32 i = 0;
	void* pFlash = NULL;
	unsigned int currentAddress = 0;
	
	pFlash = (void*)APP_FLASH_BASE_ADDRESS;
	for( i = 0; i < ((APP_FLASH_END_ADDRESS - APP_FLASH_BASE_ADDRESS + 1)/FLASH_PAGE_SIZE); i++ ) {
		#ifdef BOOTLOADER_WITH_DEE
			currentAddress = (unsigned int)pFlash + (i*FLASH_PAGE_SIZE);
			if ((currentAddress >= EEPROM_FLASH_BASE_ADDRESS) && (currentAddress <= EEPROM_FLASH_END_ADDRESS)) {
			} else
		#endif
		{
			// Assert on NV error. This must be caught during debug phase.
			if (NVMemErasePage( pFlash + (i*FLASH_PAGE_SIZE) ) != 0) {
				result = 1;
			}
		}
	}				   
	#ifdef BOOT_FLASH_BASE_ADDRESS
		pFlash = (void*)BOOT_FLASH_BASE_ADDRESS;
		for( i = 0; i < ((BOOT_FLASH_END_ADDRESS - BOOT_FLASH_BASE_ADDRESS + 1)/FLASH_PAGE_SIZE); i++ ) {
			// Assert on NV error. This must be caught during debug phase.
			if (NVMemErasePage( pFlash + (i*FLASH_PAGE_SIZE) ) != 0) {
				result = 1;
			}
		}
	#endif
	CryptoSignatureCheck_reset();
	
	return result;
}

void Framework_WriteKeyToFlash(void* address, unsigned char *key_in) {
	UINT WrData = 0;
	unsigned int x = 0;
	for (x = 0; x < 4; x++) {
		void* ProgAddress = (void*)(address + (x * 4));

		unsigned char temp_buffer[4];
		memcpy(temp_buffer, &key_in[(x * 4)], 4);
		
		WrData = temp_buffer[3];
		WrData <<= 8;
		WrData += temp_buffer[2];
		WrData <<= 8;
		WrData += temp_buffer[1];
		WrData <<= 8;
		WrData += temp_buffer[0];
		
		NVMemWriteWord(ProgAddress, WrData);
	}
	CryptoKeySet((void *) ADDRESS_AES_KEY_BOOT_MAC);
}

void Framework_WriteSerialToFlash(void* address, unsigned char *serial) {
	UINT WrData = 0;
	void* ProgAddress = (void*)address;

	unsigned char temp_buffer[4];
	memcpy(temp_buffer, &serial[0], 4);
	
	WrData = temp_buffer[0];
	WrData <<= 8;
	WrData += temp_buffer[1];
	WrData <<= 8;
	WrData += temp_buffer[2];
	WrData <<= 8;
	WrData += temp_buffer[3];
	
	NVMemWriteWord(ProgAddress, WrData);
}
