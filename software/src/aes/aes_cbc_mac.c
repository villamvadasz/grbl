#include <string.h>
#include <stdio.h>
#include "aes_128.h"

#include "k_stdtype.h"

int aes_encrypt_cbc_mac(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut) {
	int result = -1;
	if ( (dataIn != NULL) && (dataOut != NULL) && (keyslot < AES_MAXKEY) && (sizeIn != 0) && ((sizeIn % KEYLENGTH(aes_KeyHolder[keyslot].keybits)) == 0) ) {
		unsigned int x = 0;
		result = 1;

		memset(dataOut, 0x00, KEYLENGTH(aes_KeyHolder[keyslot].keybits));
		for (x = 0; x < sizeIn; x += KEYLENGTH(aes_KeyHolder[keyslot].keybits)) {
			aes_block_xor(dataIn + x, dataOut, KEYLENGTH(aes_KeyHolder[keyslot].keybits));
			aes_block_encrypt(keyslot, dataIn + x, dataOut, KEYLENGTH(aes_KeyHolder[keyslot].keybits));
		}
	}
	return result;
}
