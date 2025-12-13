#include <string.h>
#include "c_bootloader.h"
#include "Framework.h"
#include "aes_tiny.h"
#include "fixedmemoryaddress.h"

unsigned char CryptoSignature[16] = {0};
AES_ctx signature_ctx;
uint8_t signature_key[AES_BLOCKLEN] = {0};
uint8_t signature_iv[AES_BLOCKLEN] = {0};
unsigned char signature_buffer[AES_BLOCKLEN] = {0};
unsigned int signature_buffer_cnt = 0;
unsigned int signature_byte_cnt = 0;

#ifdef BOOTLOADER_DECRYPT
	AES_ctx decrypt_ctx;
	uint8_t decrypt_key[AES_BLOCKLEN] = {0};
	uint8_t decrypt_iv[AES_BLOCKLEN] = {0};
	unsigned char decrypt_buffer[AES_BLOCKLEN] = {0};
	unsigned int decrypt_buffer_cnt = 0;
	unsigned int decrypt_byte_cnt = 0;
	unsigned int decrypt_first_block = 0;
	unsigned int decrypt_fileSize = 0;
#endif

void HandleCommand_crypto(void) {
	static unsigned int singleShoot = 1;
	if (singleShoot) {
		singleShoot = 0;
		CryptoSignatureKeySet((void *) ADDRESS_AES_KEY_BOOT_MAC);
		#ifdef BOOTLOADER_DECRYPT
			#warning TODO Bootloader uses same key for signature and encryption. Must be changed.
			//CryptoDecryptKeySet((void *) ADDRESS_AES_KEY_BOOT);
			CryptoDecryptKeySet((void *) ADDRESS_AES_KEY_BOOT_MAC);
		#endif
	}
}

void CryptoSignatureKeySet(unsigned char *ptr) {
	memcpy(signature_key, ptr, 16);
}

unsigned int CryptoSignatureKeyIsWritten(void) {
	unsigned int result = 0;
	unsigned int x = 0;
	for (x = 0; x < 16; x++) {
		if (signature_key[x] != 0xFF) {
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
	AES_init_ctx_iv(&signature_ctx, signature_key, signature_iv);
	memset(signature_iv, 0x00, AES_BLOCKLEN);
	memset(signature_buffer, 0x00, AES_BLOCKLEN);
	signature_byte_cnt = 0;
}

void CryptoSignatureCheck_add(void * add, unsigned char data) {
	signature_buffer[signature_buffer_cnt] = ((data >> 0) & 0xFF);
	signature_buffer_cnt++;
	if (signature_buffer_cnt >= AES_BLOCKLEN) {
		signature_buffer_cnt = 0;
		AES_CBC_encrypt_buffer(&signature_ctx, signature_buffer, AES_BLOCKLEN);
	}
	signature_byte_cnt++;
}

unsigned int CryptoSignatureCheck_check(void) {
	unsigned int result = 0;

	while (signature_buffer_cnt != 0) {
		CryptoSignatureCheck_add(0, 0);
	}

	if (memcmp(CryptoSignature, signature_buffer, AES_BLOCKLEN) == 0) {
		result = 1;
	}

	return result;
}





#ifdef BOOTLOADER_DECRYPT
	void CryptoDecryptKeySet(unsigned char *ptr) {
		memcpy(decrypt_key, ptr, 16);
	}
	
	unsigned int CryptoDecryptKeyIsWritten(void) {
		unsigned int result = 0;
		unsigned int x = 0;
		for (x = 0; x < 16; x++) {
			if (decrypt_key[x] != 0xFF) {
				result = 1;
				break;
			}
		}
		return result;
	}
	
	void CryptoDecrypt_reset(void) {
		AES_init_ctx_iv(&decrypt_ctx, decrypt_key, decrypt_iv);
		memset(decrypt_iv, 0x00, AES_BLOCKLEN);
		memset(decrypt_buffer, 0x00, AES_BLOCKLEN);
		decrypt_byte_cnt = 0;
		decrypt_first_block = 1;
	}
	
	unsigned int CryptoDecrypt_add(void * add, unsigned char data, unsigned int *len) {
		unsigned int result = 0;
		decrypt_buffer[decrypt_buffer_cnt] = ((data >> 0) & 0xFF);
		decrypt_buffer_cnt++;
		if (decrypt_buffer_cnt >= AES_BLOCKLEN) {
			decrypt_buffer_cnt = 0;
			AES_CBC_decrypt_buffer(&decrypt_ctx, decrypt_buffer, AES_BLOCKLEN);
			if (decrypt_first_block) {
				decrypt_first_block = 0;

				decrypt_fileSize += decrypt_buffer[7];
				decrypt_fileSize <<= 8;
				decrypt_fileSize += decrypt_buffer[6];
				decrypt_fileSize <<= 8;
				decrypt_fileSize += decrypt_buffer[5];
				decrypt_fileSize <<= 8;
				decrypt_fileSize += decrypt_buffer[4];
				decrypt_fileSize <<= 8;
				decrypt_fileSize += decrypt_buffer[3];
				decrypt_fileSize <<= 8;
				decrypt_fileSize += decrypt_buffer[2];
				decrypt_fileSize <<= 8;
				decrypt_fileSize += decrypt_buffer[1];
				decrypt_fileSize <<= 8;
				decrypt_fileSize += decrypt_buffer[0];
			} else {
				if (len != NULL) {
					if (decrypt_fileSize >= AES_BLOCKLEN) {
						*len = AES_BLOCKLEN;
						decrypt_fileSize -= AES_BLOCKLEN;
					} else {
						*len = decrypt_fileSize;
						decrypt_fileSize -= decrypt_fileSize;
					}
				}
				result = 1;
			}
		}
		decrypt_byte_cnt++;
		return result;
	}
	
	unsigned int CryptoDecrypt_check(void) {
		unsigned int result = 0;
		unsigned int len;
	
		while (decrypt_buffer_cnt != 0) {
			CryptoDecrypt_add(0, 0, &len);
		}
	
		result = 1;
	
		return result;
	}
#endif
