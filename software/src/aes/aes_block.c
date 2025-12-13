#include <string.h>
#include <stdio.h>
#include "aes_128.h"

#include "k_stdtype.h"

int aes_block_encrypt(AES_KeySlot keyslot, unsigned char *dataIn, unsigned char *dataOut, unsigned char size) {
	int result = -1;
	if ((dataIn != NULL) && (dataOut != NULL) && (keyslot < AES_MAXKEY)) {
		if (size == KEYLENGTH(aes_KeyHolder[keyslot].keybits)) {
			rijndaelEncrypt_sync(aes_KeyHolder[keyslot].rk_en, aes_KeyHolder[keyslot].nrounds_en, dataIn, dataOut);
			result = 1;
		}
	}
	return result;
}

int aes_block_decrypt(AES_KeySlot keyslot, unsigned char *dataIn, unsigned char *dataOut, unsigned char size) {
	int result = -1;
	if ((dataIn != NULL) && (dataOut != NULL && (keyslot < AES_MAXKEY))) {
		if (size == KEYLENGTH(aes_KeyHolder[keyslot].keybits)) {
			rijndaelDecrypt_sync(aes_KeyHolder[keyslot].rk_de, aes_KeyHolder[keyslot].nrounds_de, dataIn, dataOut);
			result = 1;
		}
	}
	return result;
}
