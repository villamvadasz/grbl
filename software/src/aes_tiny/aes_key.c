#include <string.h>
#include <stdio.h>
#include "c_aes_128.h"
#include "aes_128.h"

#include "k_stdtype.h"

#ifdef AES_EEPROM_MANAGER_PRESENT
	#include "eep_manager.h"
#endif

AES_KeyHolder aes_KeyHolder[AES_MAXKEY];
AES_KeyHolderNv aes_KeyHolderNv[AES_MAXKEY];
uint8 aes_key_triggerLoadKeyFromEeprom = 0;

__attribute__(( weak )) void aes_key_loading_completed_callout(void) {
/*           */	unsigned char key_AES_MEK[KEYLENGTH(AES_KEYBITS_MAX)] =     {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/*           */	unsigned char key_AES_BOOT[KEYLENGTH(AES_KEYBITS_MAX)] =    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/*           */	unsigned char key_AES_BOOTMAC[KEYLENGTH(AES_KEYBITS_MAX)] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/*           */	unsigned char key_AES_KEY1[KEYLENGTH(AES_KEYBITS_MAX)] =    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/*           */	unsigned char key_AES_KEY2[KEYLENGTH(AES_KEYBITS_MAX)] =    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/*           */	unsigned char key_AES_KEY3[KEYLENGTH(AES_KEYBITS_MAX)] =    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/*SIO        */	unsigned char key_AES_KEY4[KEYLENGTH(AES_KEYBITS_MAX)] =    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/*SDCARD     */	unsigned char key_AES_KEY5[KEYLENGTH(AES_KEYBITS_MAX)] =    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/*NRF        */	unsigned char key_AES_KEY6[KEYLENGTH(AES_KEYBITS_MAX)] =    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/*           */	unsigned char key_AES_KEY7[KEYLENGTH(AES_KEYBITS_MAX)] =    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/*           */	unsigned char key_AES_KEY8[KEYLENGTH(AES_KEYBITS_MAX)] =    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

	aes_injectKey(AES_MEK, key_AES_MEK, AES_KEYBITS);
	aes_injectKey(AES_BOOT, key_AES_BOOT, AES_KEYBITS);
	aes_injectKey(AES_BOOTMAC, key_AES_BOOTMAC, AES_KEYBITS);
	aes_injectKey(AES_KEY1, key_AES_KEY1, AES_KEYBITS);
	aes_injectKey(AES_KEY2, key_AES_KEY2, AES_KEYBITS);
	aes_injectKey(AES_KEY3, key_AES_KEY3, AES_KEYBITS);
	aes_injectKey(AES_KEY4, key_AES_KEY4, AES_KEYBITS);
	aes_injectKey(AES_KEY5, key_AES_KEY5, AES_KEYBITS);
	aes_injectKey(AES_KEY6, key_AES_KEY6, AES_KEYBITS);
	aes_injectKey(AES_KEY7, key_AES_KEY7, AES_KEYBITS);
	aes_injectKey(AES_KEY8, key_AES_KEY8, AES_KEYBITS);
}

void init_aes_key(void) {
	#warning TODO This key should be made unique across each Microcontroller
	sprintf((char *)aes_KeyHolder[AES_ROM].key, "1234567890abcdef");
	aes_KeyHolder[AES_ROM].challengeResponse = 0;
	aes_KeyHolder[AES_ROM].keybits = AES_KEYBITS;

	AES_init_ctx(&aes_KeyHolder[AES_ROM].ctx, aes_KeyHolder[AES_ROM].key);
	{
		uint8_t iv[16]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		AES_ctx_set_iv(&aes_KeyHolder[AES_ROM].ctx, iv);
	}
	//aes_KeyHolder[AES_ROM].nrounds_en = rijndaelSetupEncrypt(aes_KeyHolder[AES_ROM].rk_en, aes_KeyHolder[AES_ROM].key, aes_KeyHolder[AES_ROM].keybits);
	//aes_KeyHolder[AES_ROM].nrounds_de = rijndaelSetupDecrypt(aes_KeyHolder[AES_ROM].rk_de, aes_KeyHolder[AES_ROM].key, aes_KeyHolder[AES_ROM].keybits);

	aes_key_triggerLoadKeyFromEeprom = 1;
	
}

void do_aes_key_1ms(void) {
	if (aes_key_triggerLoadKeyFromEeprom) {
		static unsigned int x = 0;
		if (x < AES_MAXKEY) {
			if (x != 0) {
				aes_KeyHolder[x].keybits = 0;
				aes_KeyHolder[x].challengeResponse = 0;
				memset((char *)aes_KeyHolder[x].key, 0xCC, KEYLENGTH(aes_KeyHolder[x].keybits));
			}
			if (aes_KeyHolderNv[x].pattern == 0xcafedead) {//valid key, must be decoded
				aes_KeyHolder[x].keybits = aes_KeyHolderNv[x].keybits;
				aes_KeyHolder[x].challengeResponse = aes_KeyHolderNv[x].challengeResponse;
				int resultTemp = 0;
				resultTemp = aes_decrypt_cbc(AES_ROM, aes_KeyHolderNv[x].keyNv, KEYLENGTH(aes_KeyHolder[x].keybits), aes_KeyHolder[x].key);
				if (resultTemp == 1) {//key decrypted
					//nothing to do here
				} else {//failed to decrypt
					memset((char *)aes_KeyHolder[x].key, 0xCC, KEYLENGTH(aes_KeyHolder[x].keybits));
				}
				AES_init_ctx(&aes_KeyHolder[x].ctx, aes_KeyHolder[x].key);
				{
					uint8_t iv[16]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
					AES_ctx_set_iv(&aes_KeyHolder[AES_ROM].ctx, iv);
				}
				//aes_KeyHolder[x].nrounds_en = rijndaelSetupEncrypt(aes_KeyHolder[x].rk_en, aes_KeyHolder[x].key, aes_KeyHolder[x].keybits);
				//aes_KeyHolder[x].nrounds_de = rijndaelSetupDecrypt(aes_KeyHolder[x].rk_de, aes_KeyHolder[x].key, aes_KeyHolder[x].keybits);
			}
			x++;
		} else {
			x = 0;
			aes_key_triggerLoadKeyFromEeprom = 0;
			aes_key_loading_completed_callout();
		}
	}
}

int aes_injectKeySecured(AES_KeySlot keyslot, unsigned char *encryptedNewKeyWithSign) {
	int result = -1;
	if ((encryptedNewKeyWithSign != NULL) && (keyslot < AES_MAXKEY)) {
		int resultTemp = 0;
		unsigned char decryptedNewKeyWithSign[KEYLENGTH(aes_KeyHolder[keyslot].keybits) * 2];
		unsigned char decryptedNewKey[KEYLENGTH(aes_KeyHolder[keyslot].keybits)];
		unsigned char decryptedSign[KEYLENGTH(aes_KeyHolder[keyslot].keybits)];
		resultTemp = aes_decrypt_cbc(keyslot, encryptedNewKeyWithSign, KEYLENGTH(aes_KeyHolder[keyslot].keybits) * 2, decryptedNewKeyWithSign);
		if (resultTemp == 1) {
			memcpy(decryptedNewKey, &decryptedNewKeyWithSign[0], KEYLENGTH(aes_KeyHolder[keyslot].keybits));
			memcpy(decryptedSign, &decryptedNewKeyWithSign[KEYLENGTH(aes_KeyHolder[keyslot].keybits)], KEYLENGTH(aes_KeyHolder[keyslot].keybits));
			if ((decryptedSign[0] == 0xde) && (decryptedSign[1] == 0xad) && (decryptedSign[2] == 0x55) && (decryptedSign[3] == 0xAA)) {
				//Valid signature present
				if (memcmp(decryptedNewKey, aes_KeyHolder[keyslot].key, KEYLENGTH(aes_KeyHolder[keyslot].keybits)) == 0) {
					result = aes_injectKey(keyslot, decryptedNewKey, aes_KeyHolder[keyslot].keybits);
				} else {
					result = 0;
				}
			}
		} else {
			result = 0;
		}
	}
	return result;
}

int aes_injectKey(AES_KeySlot keyslot, unsigned char *newkey, int keybits) {
	int result = -1;
	if ((newkey != NULL) && (keyslot < AES_MAXKEY)) {
		int resultTemp = 0;
		aes_KeyHolderNv[keyslot].pattern = 0xCCCCCCCC;
		aes_KeyHolder[keyslot].keybits = keybits;
		memcpy((char *)aes_KeyHolder[keyslot].key, newkey, KEYLENGTH(aes_KeyHolder[keyslot].keybits));

		AES_init_ctx(&aes_KeyHolder[keyslot].ctx, aes_KeyHolder[keyslot].key);
		{
			uint8_t iv[16]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			AES_ctx_set_iv(&aes_KeyHolder[AES_ROM].ctx, iv);
		}
		//aes_KeyHolder[keyslot].nrounds_en = rijndaelSetupEncrypt(aes_KeyHolder[keyslot].rk_en, aes_KeyHolder[keyslot].key, aes_KeyHolder[keyslot].keybits);
		//aes_KeyHolder[keyslot].nrounds_de = rijndaelSetupDecrypt(aes_KeyHolder[keyslot].rk_de, aes_KeyHolder[keyslot].key, aes_KeyHolder[keyslot].keybits);

		resultTemp = aes_encrypt_cbc(AES_ROM, aes_KeyHolder[keyslot].key, KEYLENGTH(aes_KeyHolder[keyslot].keybits), aes_KeyHolderNv[keyslot].keyNv);
		aes_KeyHolderNv[keyslot].keybits = aes_KeyHolder[keyslot].keybits;
		aes_KeyHolderNv[keyslot].challengeResponse = aes_KeyHolder[keyslot].challengeResponse;
		if (resultTemp == 1) {
			aes_KeyHolderNv[keyslot].pattern = 0xcafedead;
		}
		#ifdef AES_EEPROM_MANAGER_PRESENT
			eep_manager_WriteAll_Trigger();
		#endif
		
		result = 1;
	} else {
		result = 0;
	}
	return result;
}

unsigned char aes_key_loading_finished(void) {
	unsigned char result = 0;
	if (aes_key_triggerLoadKeyFromEeprom == 0) {
		result = 1;
	}
	return result;
}
