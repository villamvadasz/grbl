#include <string.h>
#include <stdio.h>
#include "aes_128.h"

#include "k_stdtype.h"

static int aes_deencrypt_cbc(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut, unsigned char enDeCrypt);

int aes_encrypt_cbc(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut) {
	return aes_deencrypt_cbc(keyslot, dataIn, sizeIn, dataOut, 0x01);
}

int aes_decrypt_cbc(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut) {
	return aes_deencrypt_cbc(keyslot, dataIn, sizeIn, dataOut, 0x00);
}

static int aes_deencrypt_cbc(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut, unsigned char enDeCrypt) {
	int result = -1;
	if ( (dataIn != NULL) && (dataOut != NULL) && (keyslot < AES_MAXKEY) && (sizeIn != 0) && (aes_KeyHolder[keyslot].keybits != 0) && ((sizeIn % KEYLENGTH(aes_KeyHolder[keyslot].keybits)) == 0) ) {
		unsigned int x = 0;
		result = 1;
		for (x = 0; x < sizeIn; x += KEYLENGTH(aes_KeyHolder[keyslot].keybits)) {
			if (enDeCrypt) {
				if (x >= KEYLENGTH(aes_KeyHolder[keyslot].keybits)) {
					aes_block_xor(dataIn + x, dataOut + x - KEYLENGTH(aes_KeyHolder[keyslot].keybits), KEYLENGTH(aes_KeyHolder[keyslot].keybits));
				}
				aes_block_encrypt(keyslot, dataIn + x, dataOut + x, KEYLENGTH(aes_KeyHolder[keyslot].keybits));
			} else {
				aes_block_decrypt(keyslot, dataIn + x, dataOut + x, KEYLENGTH(aes_KeyHolder[keyslot].keybits));
				if (x >= KEYLENGTH(aes_KeyHolder[keyslot].keybits)) {
					aes_block_xor(dataOut + x, dataIn + x - KEYLENGTH(aes_KeyHolder[keyslot].keybits), KEYLENGTH(aes_KeyHolder[keyslot].keybits));
				}
			}
		}
	}
	return result;
}
