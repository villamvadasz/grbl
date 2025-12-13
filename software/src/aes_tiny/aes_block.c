#include <string.h>
#include <stdio.h>
#include "aes_128.h"

#include "k_stdtype.h"

int aes_block_encrypt(AES_KeySlot keyslot, unsigned char *dataIn, unsigned char *dataOut, unsigned char size) {
	int result = -1;
	if ((dataIn != NULL) && (dataOut != NULL) && (keyslot < AES_MAXKEY)) {
		if (size == KEYLENGTH(aes_KeyHolder[keyslot].keybits)) {
			unsigned char buffer[AES_BLOCKLEN];
			memcpy(buffer, dataIn, AES_BLOCKLEN);
			AES_ECB_encrypt(&aes_KeyHolder[keyslot].ctx, buffer);
			//rijndaelEncrypt_sync(aes_KeyHolder[keyslot].rk_en, aes_KeyHolder[keyslot].nrounds_en, dataIn, dataOut);
			memcpy(dataOut, buffer, AES_BLOCKLEN);
			result = 1;
		}
	}
	return result;
}

int aes_block_decrypt(AES_KeySlot keyslot, unsigned char *dataIn, unsigned char *dataOut, unsigned char size) {
	int result = -1;
	if ((dataIn != NULL) && (dataOut != NULL && (keyslot < AES_MAXKEY))) {
		if (size == KEYLENGTH(aes_KeyHolder[keyslot].keybits)) {
			unsigned char buffer[AES_BLOCKLEN];
			memcpy(buffer, dataIn, AES_BLOCKLEN);
			AES_ECB_decrypt(&aes_KeyHolder[keyslot].ctx, buffer);
			//rijndaelDecrypt_sync(aes_KeyHolder[keyslot].rk_de, aes_KeyHolder[keyslot].nrounds_de, dataIn, dataOut);
			memcpy(dataOut, buffer, AES_BLOCKLEN);
			result = 1;
		}
	}
	return result;
}
