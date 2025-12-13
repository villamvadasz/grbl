#include <string.h>
#include "c_bootloader.h"
#include "Framework.h"
#include "FSIO.h"
#include "fixedmemoryaddress.h"

FSFILE * myFileHex = NULL;
FSFILE * myFileSig = NULL;
FSFILE * myFileHex_default = NULL;
FSFILE * myFileSig_default = NULL;
UINT8 asciiBuffer[1024] = {0};
UINT framework_pointer = 0;
const char sigFileName[] =			"IMAGE___.SIG";
const char hexFileName[] =       	"IMAGE___.HEX";
const char default_hexFileName[] =	"DEFAULT_.HEX";
const char default_sigFileName[] =	"DEFAULT_.SIG";
const char burn_hexFileName[] =   	"BURN____.HEX";
const char burn_sigFileName[] =   	"BURN____.HEX";
const char finished_hexFileName[] =	"FINISHED.HEX";
const char finished_sigFileName[] =	"FINISHED.HEX";
UINT8 hexRec_sdCard[100] = {0};
T_REC framework_record;
int FSInit_result = 0;
unsigned int readByteCount = 0;

unsigned int frameworkStateSdCard = 0;

void HandleCommand_LoadCryptoKey(FSFILE * sigFilePtr);

void HandleCommand_LoadCryptoKey(FSFILE * sigFilePtr) {
	//myFile_default = FSfopen(sigFileName, FS_READ);
	if (sigFilePtr != NULL) {
		UINT8 hexByte[100] = {0};
		UINT readBytes_n = 0;
		unsigned int len = 0;
		readBytes_n = FSfread((void *)&asciiBuffer[framework_pointer], 1, 512, sigFilePtr);
		if (readBytes_n == 32) {
			len = ConvertAsciiToHex(asciiBuffer, hexByte);
			if (len == 16) {
				CryptoSignatureSet(hexByte);
			}
		}
		//FSfclose(sigFilePtr);
	}
}
	#include <p32xxxx.h>

void HandleCommand_SDCARD(void) {
	switch (frameworkStateSdCard) {
		case 0 : {
			FSInit_result = FSInit();
			TRISDbits.TRISD1 = 0;
			LATDbits.LATD1 = 0;
			if (FSInit_result == 0) {
				frameworkStateSdCard = 98;
			} else {
				frameworkStateSdCard = 1;
			}
			break;
		}
		case 1 : {
			if (CryptoSignatureKeyIsWritten() == 1) {
				#ifdef BOOTLOADER_DECRYPT
					if (CryptoDecryptKeyIsWritten() == 1) {
						frameworkStateSdCard = 2;
					}
				#else
					frameworkStateSdCard = 2;
				#endif
			} else {
				//frameworkStateSdCard = 99;
			}
			break;
		}
		case 2 : {
			// Initialize the File System
			TRISDbits.TRISD1 = 0;
			LATDbits.LATD1 = 1;
			myFileHex = FSfopen(hexFileName, FS_READ);
			myFileSig = FSfopen(sigFileName, FS_READ);
	
			if ((myFileHex == NULL) || (myFileSig == NULL)) {
				if (myFileHex != NULL) {
					FSfclose(myFileHex);
				}
				if (myFileSig != NULL) {
					FSfclose(myFileSig);
				}
				if (ValidAppPresent() == 0) {
					myFileHex_default = FSfopen(default_hexFileName, FS_READ);
					myFileSig_default = FSfopen(default_sigFileName, FS_READ);
					if ((myFileHex_default != NULL) || (myFileSig_default != NULL)) {
						FSfclose(myFileHex_default);
						FSfclose(myFileSig_default);
						FScopy(default_hexFileName, hexFileName);
						FScopy(default_sigFileName, sigFileName);
					}
				} else {
					frameworkStateSdCard = 99;
				}
			} else {
				FSremove(burn_hexFileName);
				FSremove(burn_sigFileName);
				FSremove(finished_hexFileName);
				FSremove(finished_sigFileName);
				if (
						(FSrename (burn_hexFileName, myFileHex) != 0) || 
						(FSrename (burn_sigFileName, myFileSig) != 0)
				) {
					FSfclose(myFileHex);
					FSfclose(myFileSig);
					frameworkStateSdCard = 99;
				} else {
					HandleCommand_LoadCryptoKey(myFileSig);
					// Erase Flash (Block Erase the program Flash)
					Framework_EraseFlash();
					// Initialize the state-machine to read the records.
					framework_record.status = REC_NOT_FOUND;
					frameworkStateSdCard = 3;
				}	
			}
			break;
		}
		case 3 : {
			UINT readBytes = 0;
			// For a faster read, read 512 bytes at a time and buffer it.
			readBytes = FSfread((void *)&asciiBuffer[framework_pointer], 1, 512, myFileHex);
			readByteCount += readBytes;
			if (readBytes == 0) {
				// Nothing to read. Come out of this loop
				// break;
				FSfclose(myFileHex);
				FSfclose(myFileSig);
				// Something fishy. The hex file has ended abruptly, looks like there was no "end of hex record".
				//Indicate error and stay in while loop.
				frameworkStateSdCard = 4;
			}
			{
				unsigned int count_crypto = 0;
				for (count_crypto = 0; count_crypto < readBytes; count_crypto++) {
					CryptoSignatureCheck_add(NULL, asciiBuffer[framework_pointer + count_crypto]);
				}
			}
			
			volatile UINT i = 0;
			for(i = 0; i < (readBytes + framework_pointer); i ++) {
				// This state machine separates-out the valid hex records from the read 512 bytes.
				switch(framework_record.status) {
					case REC_FLASHED:
					case REC_NOT_FOUND: {
						if (asciiBuffer[i] == ':') {
							// We have a record found in the 512 bytes of data in the buffer.
							framework_record.start = &asciiBuffer[i];
							framework_record.len = 0;
							framework_record.status = REC_FOUND_BUT_NOT_FLASHED;
						}
						break;
					}
					case REC_FOUND_BUT_NOT_FLASHED: {
						if ((asciiBuffer[i] == 0x0A) || (asciiBuffer[i] == 0xFF)) {
							// We have got a complete record. (0x0A is new line feed and 0xFF is End of file)
							// Start the hex conversion from element
							// 1. This will discard the ':' which is
							// the start of the hex record.
							unsigned int hexRec_sdCardLen = 0;
							hexRec_sdCardLen = ConvertAsciiToHex(&framework_record.start[1], hexRec_sdCard);
							WriteHexRecord2Flash(hexRec_sdCard, hexRec_sdCardLen);
							framework_record.status = REC_FLASHED;
						}
						break;
					}
				}
				// Move to next byte in the buffer.
				framework_record.len ++;
			}
			
			if (framework_record.status == REC_FOUND_BUT_NOT_FLASHED) {
				// We still have a half read record in the buffer. The next half part of the record is read 
				// when we read 512 bytes of data from the next file read. 
				memcpy(asciiBuffer, framework_record.start, framework_record.len);
				framework_pointer = framework_record.len;
				framework_record.status = REC_NOT_FOUND;
			} else {
				framework_pointer = 0;
			}
				
			break;
		}
		case 4 : {
			TRISDbits.TRISD1 = 0;
			LATDbits.LATD1 = 0;
			if (CryptoSignatureCheck_check() != 0) {
				//RunApplicationSet();
				frameworkStateSdCard = 99;
			} else {
				Framework_EraseFlash();
			}
		}
		default : {
			break;
		}
	}
}
