#include <string.h>
#include "c_bootloader.h"
#include "Framework.h"
#include "fixedmemoryaddress.h"

T_FRAME RxBuff;
T_FRAME TxBuff;
unsigned int writeErrorCounter = 0;
unsigned int eraseErrorCounter = 0;
BOOL RxFrameValid = FALSE;

void BuildRxFrame(UINT8 *RxData, INT16 RxLen);

void HandleCommand_USB(void) {
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
			if (Framework_EraseFlash() != 0) {
				eraseErrorCounter++;
			}
			//Set the transmit frame length.
			TxBuff.Len = 1; // Command
			break;
		}
		case PROGRAM_FLASH: {
			if (WriteHexRecord2Flash(&RxBuff.Data[1], RxBuff.Len - 3) != 0) {	//Negate length of command and CRC RxBuff.Len.
				writeErrorCounter++;
			}
			//Set the transmit frame length.
			TxBuff.Len = 1; // Command
		   	break;
		}
		case READ_CRC: {
			 // Get address from the packet.
			memcpy(&Address.v[0], &RxBuff.Data[1], sizeof(Address.Val));
			memcpy(&Length.v[0], &RxBuff.Data[5], sizeof(Length.Val));
			#ifdef BOOTLOADER_WITH_DEE
				crc.Val = CalculateCrc_WithoutEeprom((UINT8 *)Address.Val, Length.Val);
			#else
				crc.Val = CalculateCrc((UINT8 *)Address.Val, Length.Val);
			#endif
			memcpy(&TxBuff.Data[1], &crc.v[0], 2);


			if (CryptoSignatureCheck_check() != 0) {
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
		case WRITE_CRYPTO_KEY : {
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
			} else {
			}

			memcpy(&TxBuff.Data[1], (void *)ADDRESS_AES_KEY_BOOT_MAC, 2);
			memcpy(&TxBuff.Data[1+2], (void *)(ADDRESS_AES_KEY_BOOT_MAC + 14), 2);
			TxBuff.Len = 1 + 4; // Boot Info Fields	+ command
			break;
		}
		default: {
			// Nothing to do.
			break;
		}
	}
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

