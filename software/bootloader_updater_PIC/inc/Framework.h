 // Copyright (c) 2002-2010,  Microchip Technology Inc.
//
// Microchip licenses this software to you solely for use with Microchip
// products.  The software is owned by Microchip and its licensors, and
// is protected under applicable copyright laws.  All rights reserved.
//
// SOFTWARE IS PROVIDED "AS IS."  MICROCHIP EXPRESSLY DISCLAIMS ANY
// WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  IN NO EVENT SHALL
// MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, HARM TO YOUR
// EQUIPMENT, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY
// OR SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED
// TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION,
// OR OTHER SIMILAR COSTS.
//
// To the fullest extent allowed by law, Microchip and its licensors
// liability shall not exceed the amount of fees, if any, that you
// have paid directly to Microchip to use this software.
//
// MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE
// OF THESE TERMS.
#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

	#include "GenericTypeDefs.h"
	
	#define REC_FLASHED 0
	#define REC_NOT_FOUND 1
	#define REC_FOUND_BUT_NOT_FLASHED 2

	#define SOH 01
	#define EOT 04
	#define DLE 16

	#define FRAMEWORK_BUFF_SIZE					1000

	typedef enum _T_COMMANDS {
		READ_BOOT_INFO = 1,
		ERASE_FLASH, 
		PROGRAM_FLASH,
		READ_CRC,
		JMP_TO_APP,
		WRITE_CRYPTO_SIGNATURE,
		READ_SERIAL_NUMBER,
		WRITE_SERIAL_NUMBER,
		WRITE_CRYPTO_KEY
	} T_COMMANDS;

	typedef struct _T_FRAME {
		UINT Len;
		UINT8 Data[FRAMEWORK_BUFF_SIZE];

	} T_FRAME;

	typedef struct _T_HEX_RECORD {
		UINT8 RecDataLen;
		DWORD_VAL Address;
		UINT8 RecType;
		UINT8* Data;
		UINT8 CheckSum;
		DWORD_VAL ExtSegAddress;
		DWORD_VAL ExtLinAddress;
	} T_HEX_RECORD;


	typedef struct _T_REC {
		UINT8 *start;
		UINT8 len;
		UINT8 status;
	} T_REC;


	extern void FrameWorkInit(UINT val);
	extern void FrameWorkTask(void);
	extern void FrameWorkClose(void);
	extern BOOL ExitFirmwareUpgradeMode(void);
	

	extern void HandleCommand_crypto(void);
	extern void HandleCommand_SDCARD(void);
	extern void HandleCommand_USB(void);
	
	extern UINT GetTransmitFrame(UINT8* Buff);
	extern void BuildRxFrame(UINT8 *RxData, INT16 RxLen);


	extern const UINT8 BootInfo[2];
	extern BOOL ValidAppPresent(void);
	extern unsigned int Framework_EraseFlash(void);
	extern void Framework_WriteKeyToFlash(void* address, unsigned char *key);
	extern void Framework_WriteSerialToFlash(void* address, unsigned char *serial);
	extern unsigned int WriteHexRecord2Flash(UINT8* HexRecord, UINT totalRecLen);
	extern UINT16 CalculateCrc(UINT8 *data, UINT32 len);
	#ifdef BOOTLOADER_WITH_DEE
		extern UINT16 CalculateCrc_WithoutEeprom(UINT8 *data, UINT32 len);
	#endif
	
	extern void CryptoSignatureSet(unsigned char *ptr);
	extern void CryptoKeySet(unsigned char *ptr);
	extern unsigned int CryptoKeyIsWritten(void);
	extern void CryptoSignatureCheck_reset(void);
	extern void CryptoSignatureCheck_add(void * add, unsigned char data);
	

	extern unsigned int CryptoSignatureCheck_check(void);
	extern void RunApplicationSet(void);
	extern void DisableAutoRunDelay(void);

#endif
