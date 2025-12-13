#include <string.h>
#include "c_bootloader.h"
#include "Framework.h"
#include "aes_tiny.h"
#include "fixedmemoryaddress.h"

unsigned char CryptoSignature[16] = {0};
AES_ctx ctx;
uint8_t key[AES_BLOCKLEN] = {0};
uint8_t iv[AES_BLOCKLEN] = {0};
unsigned char crypto_buffer[AES_BLOCKLEN] = {0};
unsigned int crypto_buffer_cnt = 0;
unsigned int crtpto_byte_cnt = 0;

void HandleCommand_crypto(void) {
	static unsigned int singleShoot = 1;
	if (singleShoot) {
		singleShoot = 0;
		CryptoKeySet((void *) ADDRESS_AES_KEY_BOOT_MAC);
	}
}

void CryptoKeySet(unsigned char *ptr) {
	memcpy(key, ptr, 16);
}

unsigned int CryptoKeyIsWritten(void) {
	unsigned int result = 0;
	unsigned int x = 0;
	for (x = 0; x < 16; x++) {
		if (key[x] != 0xFF) {
			result = 1;
			break;
		}
	}
	return result;
}

void CryptoSignatureSet(unsigned char *ptr) {
	memcpy(CryptoSignature, ptr, 16);
}

void CryptoSignatureCheck_reset(void) {
	AES_init_ctx_iv(&ctx, key, iv);
	memset(iv, 0x00, AES_BLOCKLEN);
	memset(crypto_buffer, 0x00, AES_BLOCKLEN);
	crtpto_byte_cnt = 0;
}

void CryptoSignatureCheck_add(void * add, unsigned char data) {
	crypto_buffer[crypto_buffer_cnt] = ((data >> 0) & 0xFF);
	crypto_buffer_cnt++;
	if (crypto_buffer_cnt >= AES_BLOCKLEN) {
		crypto_buffer_cnt = 0;
		AES_CBC_encrypt_buffer(&ctx, crypto_buffer, AES_BLOCKLEN);
	}
	crtpto_byte_cnt++;
}

unsigned int CryptoSignatureCheck_check(void) {
	unsigned int result = 0;

	while (crypto_buffer_cnt != 0) {
		CryptoSignatureCheck_add(0, 0);
	}

	if (memcmp(CryptoSignature, crypto_buffer, AES_BLOCKLEN) == 0) {
		result = 1;
	}

	return result;
}

