#include <string.h>
#include "c_bootloader.h"
#include "GenericTypeDefs.h"
#include "Framework.h"
#include "BootLoader.h"

#ifdef TRANSPORT_LAYER_ETH
	#include "Ethernet_Tasks.h"
#endif
#ifdef TRANSPORT_LAYER_USB
	#include "Usb_Tasks.h"
#endif


#ifndef BOOTLOADER_DELAY_TIME
	#define BOOTLOADER_DELAY_TIME (0x7FFFF)
#endif

#ifdef BOOTLOADER_DECRYPT
unsigned char framework_usb_ascii_buffer[1024] = {0};
unsigned int aes_offset_cnt = 0;
unsigned char framework_usb_buffer[128] = {0};
unsigned int block_len = 0;
#endif

const UINT8 BootInfo[2] = { MAJOR_VERSION, MINOR_VERSION };
BOOL RunApplication = FALSE;
unsigned int delayRunApplicationEnabled = 1;
unsigned long delayRunApplication = BOOTLOADER_DELAY_TIME;
BOOL RxFrameValid = FALSE;
T_FRAME RxBuff;
T_FRAME TxBuff;
unsigned int writeErrorCounter = 0;
unsigned int eraseErrorCounter = 0;

void FrameWorkInit(UINT val) {
	#ifdef TRANSPORT_LAYER_ETH
		ethernetInit(val);
	#endif
	#ifdef TRANSPORT_LAYER_USB
		UsbInit(val);
	#endif
}

void FrameWorkTask(void) {
	
	HandleCommand_crypto();

	#ifdef TRANSPORT_LAYER_ETH
		HandleCommand_Ethernet();
	#endif

	#ifdef TRANSPORT_LAYER_SD_CARD
		HandleCommand_SDCARD();
	#endif
	
	#ifdef TRANSPORT_LAYER_USB
		UsbTasks();
		HandleCommand_USB();
	#endif

	#ifdef BOOTLOADER_DELAYED_AUTOAPPLICATION
		if (delayRunApplicationEnabled) {
			if (!ValidAppPresent()) {
				delayRunApplicationEnabled = 0;
			} else {
				if (delayRunApplication != 0) {
					delayRunApplication--;
				} else {
					delayRunApplicationEnabled = 0;
					RunApplication = TRUE;
				}
			}
		}
	#endif
}

void FrameWorkClose(void) {
	#ifdef TRANSPORT_LAYER_ETH
		ethernetClose();
	#endif
	#ifdef TRANSPORT_LAYER_USB
		UsbClose();
	#endif
}

void DisableAutoRunDelay(void) {
	delayRunApplicationEnabled = 0;
}

void RunApplicationSet(void) {
	RunApplication = TRUE;
}

BOOL ExitFirmwareUpgradeMode(void) {
	return RunApplication;
}

void BuildRxFrame(UINT8 *RxData, INT16 RxLen) {
	static BOOL Escape = FALSE;
	WORD_VAL crc;

	while((RxLen > 0) && (!RxFrameValid)) { // Loop till len = 0 or till frame is valid
		RxLen--;

		if (RxBuff.Len >= sizeof(RxBuff.Data)) {
			RxBuff.Len = 0;
		}

		switch(*RxData) {
			case SOH : {//Start of header
				if (Escape) {
					// Received byte is not SOH, but data.
					RxBuff.Data[RxBuff.Len++] = *RxData;
					// Reset Escape Flag.
					Escape = FALSE;
				} else {
					// Received byte is indeed a SOH which indicates start of new frame.
					RxBuff.Len = 0;
				}
				break;
			}
			case EOT : {// End of transmission
				if (Escape) {
					// Received byte is not EOT, but data.
					RxBuff.Data[RxBuff.Len++] = *RxData;
					// Reset Escape Flag.
					Escape = FALSE;
				} else {
					// Received byte is indeed a EOT which indicates end of frame.
					// Calculate CRC to check the validity of the frame.
					if (RxBuff.Len > 1) {
						crc.byte.LB = RxBuff.Data[RxBuff.Len-2];
						crc.byte.HB = RxBuff.Data[RxBuff.Len-1];
						if ((CalculateCrc(RxBuff.Data, (UINT32)(RxBuff.Len-2)) == crc.Val) && (RxBuff.Len > 2)) {
							// CRC matches and frame received is valid.
							RxFrameValid = TRUE;
						}
					}
				}
				break;
			}
			case DLE : {// Escape character received.
				if (Escape) {
					// Received byte is not ESC but data.
					RxBuff.Data[RxBuff.Len++] = *RxData;
					// Reset Escape Flag.
					Escape = FALSE;
				} else {
					// Received byte is an escape character. Set Escape flag to escape next byte.
					Escape = TRUE;
				}
				break;
			}
			default: {// Data field.
				RxBuff.Data[RxBuff.Len++] = *RxData;
				// Reset Escape Flag.
				Escape = FALSE;
				break;
			}
		}
		//Increment the pointer.
		RxData++;
	}
}

UINT GetTransmitFrame(UINT8* Buff) {
	INT BuffLen = 0;
	WORD_VAL crc;
	UINT8 i;

	if (TxBuff.Len) {
		//There is something to transmit.
		// Calculate CRC of the frame.
		crc.Val = CalculateCrc(TxBuff.Data, (UINT32)TxBuff.Len);
		TxBuff.Data[TxBuff.Len++] = crc.byte.LB;
		TxBuff.Data[TxBuff.Len++] = crc.byte.HB; 

		// Insert SOH (Indicates beginning of the frame)
		Buff[BuffLen++] = SOH;

		// Insert Data Link Escape Character.
		for(i = 0; i < TxBuff.Len; i++) {
			if (
				(TxBuff.Data[i] == EOT) || 
				(TxBuff.Data[i] == SOH) || 
				(TxBuff.Data[i] == DLE)
			) {
				// EOT/SOH/DLE repeated in the data field, insert DLE.
				Buff[BuffLen++] = DLE;
			}
			Buff[BuffLen++] = TxBuff.Data[i];
		} 

		// Mark end of frame with EOT.
		Buff[BuffLen++] = EOT;

		TxBuff.Len = 0; // Purge this buffer, no more required.
	}

	return(BuffLen); // Return buffer length.
}

void HandleCommand_general(void) {
	UINT8 Cmd;
	DWORD_VAL Address;
	UINT8 i;
	DWORD_VAL Length;
	UINT8 *DataPtr;
	UINT Result;
	WORD_VAL crc;
	void* pFlash;

	if (RxFrameValid == FALSE) {
		return;
	}

	RxFrameValid = FALSE;

	// First byte of the data field is command.
	Cmd = RxBuff.Data[0];
	// Partially build response frame. First byte in the data field carries command.
	TxBuff.Data[0] = RxBuff.Data[0];

	// Reset the response length to 0.
	TxBuff.Len = 0;

	DisableAutoRunDelay();

	// Process the command.
	switch(Cmd) {
		case READ_BOOT_INFO: { // Read boot loader version info.
			memcpy(&TxBuff.Data[1], BootInfo, 2);
			if (writeErrorCounter != 0) {
				TxBuff.Data[2] += 1;
			}
			if (writeErrorCounter != 0) {
				TxBuff.Data[1] += 1;
			}
 
			//Set the transmit frame length.
			TxBuff.Len = 2 + 1; // Boot Info Fields	+ command
			break;
		}
		case ERASE_FLASH: {
			#ifdef BOOTLOADER_DECRYPT
				CryptoDecrypt_reset();
			#endif
			if (Framework_EraseFlash() != 0) {
				eraseErrorCounter++;
			}
			//Set the transmit frame length.
			TxBuff.Len = 1; // Command
			break;
		}
		case PROGRAM_FLASH: {
			#ifdef BOOTLOADER_DECRYPT
			{
				unsigned int x = 0;
				for (x = 0; x < (RxBuff.Len - 3); x++) {
					if (CryptoDecrypt_add(0, RxBuff.Data[1 + x], &block_len)) {
						unsigned int y = 0;
						for (y = 0; y < block_len; y++) {
							unsigned char ch = decrypt_buffer[y];
							if (ch == ':') {
								aes_offset_cnt = 0;
							}
							framework_usb_ascii_buffer[aes_offset_cnt++] = ch;
							if (aes_offset_cnt >= (sizeof(framework_usb_ascii_buffer) / sizeof(*framework_usb_ascii_buffer)) ) {
								aes_offset_cnt = 0;
							}
							if ((ch == '\r') || (ch == '\n') || (ch == 0xFF)) {
								if (aes_offset_cnt != 1) {
									unsigned int hexRec = 0;
									framework_usb_ascii_buffer[aes_offset_cnt] = 0;
									hexRec = ConvertAsciiToHex(&framework_usb_ascii_buffer[1], framework_usb_buffer);
									if (WriteHexRecord2Flash(framework_usb_buffer, hexRec) != 0) {
										writeErrorCounter++;
									}
								}
								aes_offset_cnt = 0;
							}
						}
					}
				}
			}
			#else
				if (WriteHexRecord2Flash(&RxBuff.Data[1], RxBuff.Len - 3) != 0) {	//Negate length of command and CRC RxBuff.Len.
					writeErrorCounter++;
				}
			#endif
			//Set the transmit frame length.
			TxBuff.Len = 1; // Command
		   	break;
		}
		case READ_CRC: {
			#ifdef BOOTLOADER_DECRYPT
				extern T_HEX_RECORD HexRecordSt;
				HexRecordSt.MinAddress -= HexRecordSt.MinAddress % 4;
				HexRecordSt.MaxAddress += HexRecordSt.MaxAddress % 4;

				Length.Val = HexRecordSt.MaxAddress - HexRecordSt.MinAddress;
				Address.Val = (HexRecordSt.MinAddress - 0);
			#else
				// Get address from the packet.
				memcpy(&Address.v[0], &RxBuff.Data[1], sizeof(Address.Val));
				memcpy(&Length.v[0], &RxBuff.Data[5], sizeof(Length.Val));
			#endif

			#ifdef BOOTLOADER_WITH_DEE
				crc.Val = CalculateCrc_WithoutEeprom((UINT8 *)Address.Val, Length.Val);
			#else
				crc.Val = CalculateCrc((UINT8 *)Address.Val, Length.Val);
			#endif
			memcpy(&TxBuff.Data[1], &crc.v[0], 2);


			if (CryptoSignatureCheck_check() != 0) {
				#ifdef BOOTLOADER_DECRYPT
					TxBuff.Data[1] = 0xC0;
					TxBuff.Data[2] = 0x00;
				#endif
				//Signature OK
			} else {
				//Wrong signature
				TxBuff.Data[1] = 0xC0;
				TxBuff.Data[2] = 0xEE;
				if (Framework_EraseFlash() != 0) {
					eraseErrorCounter++;
				}
			}

			//Set the transmit frame length.
			TxBuff.Len = 1 + 2;	// Command + 2 bytes of CRC.

			break;
		}
		case JMP_TO_APP: {
			// Exit firmware upgrade mode.
			RunApplicationSet();
			break;
		}
		case WRITE_CRYPTO_SIGNATURE : {
			 // Get address from the packet.
			CryptoSignatureSet(&RxBuff.Data[1]);
			TxBuff.Len = 1; // Command
			break;
		}
		case READ_SERIAL_NUMBER : {
			memcpy(&TxBuff.Data[1], (void *)ADDRESS_SERIAL_NUMBER, 4);
 
			//Set the transmit frame length.
			TxBuff.Len = 1 + 4; // Boot Info Fields	+ command
			break;
		}
		case WRITE_SERIAL_NUMBER : {
			UINT WrData = 0;
			unsigned char current_serial_number[4];
			memcpy(current_serial_number, (void *)ADDRESS_SERIAL_NUMBER, 4);
			if (
					(current_serial_number[0] == 0xFF) &&
					(current_serial_number[1] == 0xFF) &&
					(current_serial_number[2] == 0xFF) &&
					(current_serial_number[3] == 0xFF)
			) {
				Framework_WriteSerialToFlash((void*)ADDRESS_SERIAL_NUMBER, &RxBuff.Data[1]);
			} else {
			}

			memcpy(&TxBuff.Data[1], (void *)ADDRESS_SERIAL_NUMBER, 4);
			TxBuff.Len = 1 + 4; // Boot Info Fields	+ command
			break;
		}
		case WRITE_CRYPTO_KEY_BOOT_CMAC : {
			UINT WrData = 0;
			unsigned char current_crypto_key[16];
			memcpy(current_crypto_key, (void *)ADDRESS_AES_KEY_BOOT_MAC, 16);
			if (
					(current_crypto_key[0] == 0xFF) &&
					(current_crypto_key[1] == 0xFF) &&
					(current_crypto_key[2] == 0xFF) &&
					(current_crypto_key[3] == 0xFF) &&
					(current_crypto_key[4] == 0xFF) &&
					(current_crypto_key[5] == 0xFF) &&
					(current_crypto_key[6] == 0xFF) &&
					(current_crypto_key[7] == 0xFF) &&
					(current_crypto_key[8] == 0xFF) &&
					(current_crypto_key[9] == 0xFF) &&
					(current_crypto_key[10] == 0xFF) &&
					(current_crypto_key[11] == 0xFF) &&
					(current_crypto_key[12] == 0xFF) &&
					(current_crypto_key[13] == 0xFF) &&
					(current_crypto_key[14] == 0xFF) &&
					(current_crypto_key[15] == 0xFF)
			) {
				Framework_WriteKeyToFlash((void*)ADDRESS_AES_KEY_BOOT_MAC, &RxBuff.Data[1]);
				CryptoSignatureKeySet((void *) ADDRESS_AES_KEY_BOOT_MAC);
				#ifdef BOOTLOADER_DECRYPT
					#warning TODO Bootloader uses same key for signature and encryption. Must be changed.
					CryptoDecryptKeySet((void *) ADDRESS_AES_KEY_BOOT_MAC);
				#endif
			} else {
			}

			memcpy(&TxBuff.Data[1], (void *)ADDRESS_AES_KEY_BOOT_MAC, 2);
			memcpy(&TxBuff.Data[1+2], (void *)(ADDRESS_AES_KEY_BOOT_MAC + 14), 2);
			TxBuff.Len = 1 + 4; // Boot Info Fields	+ command
			break;
		}
		case WRITE_CRYPTO_KEY_BOOT : {
			UINT WrData = 0;
			unsigned char current_crypto_key[16];
			memcpy(current_crypto_key, (void *)ADDRESS_AES_KEY_BOOT, 16);
			if (
					(current_crypto_key[0] == 0xFF) &&
					(current_crypto_key[1] == 0xFF) &&
					(current_crypto_key[2] == 0xFF) &&
					(current_crypto_key[3] == 0xFF) &&
					(current_crypto_key[4] == 0xFF) &&
					(current_crypto_key[5] == 0xFF) &&
					(current_crypto_key[6] == 0xFF) &&
					(current_crypto_key[7] == 0xFF) &&
					(current_crypto_key[8] == 0xFF) &&
					(current_crypto_key[9] == 0xFF) &&
					(current_crypto_key[10] == 0xFF) &&
					(current_crypto_key[11] == 0xFF) &&
					(current_crypto_key[12] == 0xFF) &&
					(current_crypto_key[13] == 0xFF) &&
					(current_crypto_key[14] == 0xFF) &&
					(current_crypto_key[15] == 0xFF)
			) {
				Framework_WriteKeyToFlash((void*)ADDRESS_AES_KEY_BOOT, &RxBuff.Data[1]);
				#ifdef BOOTLOADER_DECRYPT
					CryptoDecryptKeySet((void *) ADDRESS_AES_KEY_BOOT);
				#endif
			} else {
			}

			memcpy(&TxBuff.Data[1], (void *)ADDRESS_AES_KEY_BOOT, 2);
			memcpy(&TxBuff.Data[1+2], (void *)(ADDRESS_AES_KEY_BOOT + 14), 2);
			TxBuff.Len = 1 + 4; // Boot Info Fields	+ command
			break;
		}
		default: {
			// Nothing to do.
			break;
		}
	}
}

unsigned int ConvertAsciiToHex(UINT8* asciiRec_in, UINT8* hexByte_out) {
	unsigned int result = 0;
	UINT8 i = 0;
	UINT8 k = 0;
	UINT8 hex = 0;

	while(
		((asciiRec_in[i] >= '0') && (asciiRec_in[i] <= '9')) ||
		((asciiRec_in[i] >= 'a') && (asciiRec_in[i] <= 'f')) ||
		((asciiRec_in[i] >= 'A') && (asciiRec_in[i] <= 'F'))
	) {
		// Check if the ascii values are in alpha numeric range.
		if ((asciiRec_in[i] >= 'A') && (asciiRec_in[i] <= 'F')) {
			asciiRec_in[i] = asciiRec_in[i] - 'A' + 'a';
		}

		if ((asciiRec_in[i] >= '0') && (asciiRec_in[i] <= '9')) {
			// Numerical reperesentation in ASCII found.
			hex = (asciiRec_in[i] - '0');
		} else if ((asciiRec_in[i] >= 'a') && (asciiRec_in[i] <= 'f')) {
			// Alphabetical value.
			hex = (asciiRec_in[i] - 'a') + 0xA;
		} else {
		}
	
		// Following logic converts 2 bytes of ASCII to 1 byte of hex.
		k = i % 2;
	
		if (k) {
			hexByte_out[i / 2] |= hex;
		} else {
			hexByte_out[i / 2] = (hex << 4) & 0xF0;
		}
		i++;
		result++;
	}
	result /= 2;
	return result;
}
