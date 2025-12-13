#include <string.h>
#include <stdio.h>
#include "aes_128.h"

#include "k_stdtype.h"

void aes_getChallenge(AES_KeySlot keyslot, unsigned char *challenge, unsigned char len) {
	if ((len != 0) && (keyslot < AES_MAXKEY)) {
		memset(challenge, 0x00, len);
		aes_KeyHolder[keyslot].challengeResponse ++;
		if (len > 0) {
			challenge[0] = aes_KeyHolder[keyslot].challengeResponse;
		}
		if (len > 1) {
			challenge[1] = aes_KeyHolder[keyslot].challengeResponse >> 8;
		}
		if (len > 2) {
			challenge[2] = aes_KeyHolder[keyslot].challengeResponse >> 16;
		}
		if (len > 3) {
			challenge[3] = aes_KeyHolder[keyslot].challengeResponse >> 24;
		}
	}
}

int aes_checkResponse(AES_KeySlot keyslot, unsigned char *response, unsigned char len) {
	int result = -1;
	if ((len != 0) && (len <= 16) && (keyslot < AES_MAXKEY)) {
		result = 0;
		unsigned char plaintext_en[KEYLENGTH(AES_KEYBITS_MAX)];
		unsigned char ciphertext_en[KEYLENGTH(AES_KEYBITS_MAX)];
		memcpy(plaintext_en, &aes_KeyHolder[keyslot].challengeResponse, len);

		unsigned char buffer[AES_BLOCKLEN];
		memcpy(buffer, plaintext_en, AES_BLOCKLEN);
		AES_ECB_encrypt(&aes_KeyHolder[keyslot].ctx, buffer);
		//rijndaelEncrypt_sync(aes_KeyHolder[keyslot].rk_en, aes_KeyHolder[keyslot].nrounds_en, plaintext_en, ciphertext_en);
		memcpy(ciphertext_en, buffer, AES_BLOCKLEN);

		{
			unsigned char x = 0;
			result = 1;
			for (x = 0; x < len; x++) {
				if ((ciphertext_en[x] ^ response[x]) != 0) {
					result = 0;
					break;
				}
			}
		}
	}
	return result;
}
