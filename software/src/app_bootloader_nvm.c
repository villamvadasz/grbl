#include <string.h>
#include "app.h"
#include "k_stdtype.h"

#ifdef APP_BOOTLOADER_FOTA

	#include "FSIO.h"

	BOOL do_HandleCommand_SDCARD = FALSE;
	FSFILE * myFile_hex = NULL;
	FSFILE * myFile_sig = NULL;
	const char sigFileName[] =			"IMAGE___.SIG";
	const char hexFileName[] =       	"IMAGE___.HEX";
	const char defaulthexFileName[] =	"DEFAULT_.HEX";
	const char defaultsigFileName[] =	"DEFAULT_.SIG";
	const char burnhexFileName[] =   	"BURN____.HEX";
	const char finishedhexFileName[] =	"FINISHED.HEX";
	
	void app_bootloader_nvm_init(void) {
	}

	void do_app_bootloader_nvm(void) {
		static unsigned char do_app_bootloader_nvm_Singleton = 1;

		extern BYTE MDD_SDSPI_MediaDetect(void);
		if (MDD_SDSPI_MediaDetect() != 0) {
			if (do_app_bootloader_nvm_Singleton) {
				do_app_bootloader_nvm_Singleton = 0;
				if (!FSInit()) {
					do_HandleCommand_SDCARD = FALSE;
				} else {
					do_HandleCommand_SDCARD = TRUE;
				}
			}
		}
	}

	sint32 app_bootloader_nvm_check_erased(void) {
		sint32 result = -1;
		
		if (do_HandleCommand_SDCARD != FALSE) {
			result = 0;
	
			myFile_sig = FSfopen(sigFileName, FS_READ);
			if (myFile_sig != NULL) {
				result++;
				if (FSfclose(myFile_sig) != 0) {
					result = -1;
				}
			}

			myFile_hex = FSfopen(hexFileName, FS_READ);
			if (myFile_hex != NULL) {
				result++;
				if (FSfclose(myFile_hex) != 0) {
					result = -1;
				}
			}

		}
		return result;
	}

	sint32 app_bootloader_nvm_erase_asynch(void) {
		sint32 result = -1;

		if (do_HandleCommand_SDCARD != FALSE) {
			result = 0;
			if (FSremove(sigFileName) != 0) {
				if (FSerror() != CE_FILE_NOT_FOUND) {
					result++;
				}
			}

			if (FSremove(hexFileName) != 0) {
				if (FSerror() != CE_FILE_NOT_FOUND) {
					result++;
				}
			}
		} else {
			result = 1;
		}
		return result;
	}
	
	sint32 app_bootloader_nvm_set_signature(char *signature_str) {
		sint32 result = -1;
		if (do_HandleCommand_SDCARD != FALSE) {
			if (signature_str != NULL) {

				myFile_sig = FSfopen(sigFileName, FS_WRITE);
				if (myFile_sig == NULL) {
					result++;
				} else {
					size_t  nWrite = 0;
					size_t  len = strlen(signature_str);
					result = 0;
					
					nWrite = FSfwrite(&signature_str[0], 1, len, myFile_sig);
					if (nWrite != len) {
						result ++;
					}
					if (FSfclose(myFile_sig) != 0) {
						result++;
					}
				}
			}
		} else {
			result = 1;
		}
		return result;
	}

#endif
