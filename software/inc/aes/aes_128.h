#ifndef _AES_128_H_
#define _AES_128_H_

	#include "k_stdtype.h"
	#include "c_aes_128.h"
	#include "rijndael.h"

	#define KEYLENGTH(keybits) ((keybits)/8)
	#define RKLENGTH(keybits)  ((keybits)/8+28)
	#define NROUNDS(keybits)   ((keybits)/32+6)

	typedef enum _AES_KeySlot {
		AES_ROM = 0,
		AES_MEK,
		AES_BOOT,//Bootloader encrytion
		AES_BOOTMAC,//Bootloader signature
		AES_KEY1,
		AES_KEY2,
		AES_KEY3,
		AES_KEY4,//SIO
		AES_KEY5,//SDCARD
		AES_KEY6,//NRF
		AES_KEY7,
		AES_KEY8,
		AES_ZERO_KEY,
		AES_MAXKEY,
	} AES_KeySlot;

	typedef struct _AES_KeyHolderNv {
		u8 keyNv[KEYLENGTH(AES_KEYBITS_MAX)];
		int keybits;
		uint32 challengeResponse;
		uint32 pattern;
	} AES_KeyHolderNv;

	typedef struct _AES_KeyHolder {
		u32 rk_en[RKLENGTH(AES_KEYBITS_MAX)];
		u32 rk_de[RKLENGTH(AES_KEYBITS_MAX)];
		int nrounds_en;
		int nrounds_de;
		int keybits;
		uint32 challengeResponse;
		u8 key[KEYLENGTH(AES_KEYBITS_MAX)];
	} AES_KeyHolder;
	
	extern void aes_LoadROMKey(unsigned char * rom_key);

	extern int aes_injectKeySecured(AES_KeySlot keyslot, unsigned char *encryptedNewKeyWithSign);
	extern int aes_injectKey(AES_KeySlot keyslot, unsigned char *newkey, int keybits);

	extern void aes_block_xor(unsigned char *a, unsigned char *b, unsigned int len);
	extern void aes_block_buffer_xor(unsigned char *a, unsigned char *b, unsigned char *out, unsigned int len);

	extern void aes_getChallenge(AES_KeySlot keyslot, unsigned char *challenge, unsigned char len);
	extern int aes_checkResponse(AES_KeySlot keyslot, unsigned char *response, unsigned char len);

	extern int aes_block_encrypt(AES_KeySlot keyslot, unsigned char *dataIn, unsigned char *dataOut, unsigned char size);
	extern int aes_block_decrypt(AES_KeySlot keyslot, unsigned char *dataIn, unsigned char *dataOut, unsigned char size);

	extern int aes_encrypt_ecb(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut);
	extern int aes_decrypt_ecb(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut);

	extern int aes_encrypt_cbc(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut);
	extern int aes_decrypt_cbc(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut);

	extern int aes_encrypt_cbc_mac(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut);
	extern int aes_encrypt_cmac(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut);

	extern unsigned char aes_key_loading_finished(void);
	extern void aes_key_loading_completed_callout(void);//CALLOUT

	//Asynchronous interface
	#define AES_INVALID_HANDLE (-1)
	typedef int AES_HANDLE;
	
	typedef enum _AES_CHUNK_STATE {
		AES_CHUNK_STATE_WAIT = 0,
		AES_CHUNK_STATE_NOT_LAST_BLOCK,
		AES_CHUNK_STATE_LAST_BLOCK,
	} AES_CHUNK_STATE;

	typedef AES_CHUNK_STATE (*fp_aes_get_chunk)(unsigned char *, unsigned char *);

	extern AES_HANDLE aes_encrypt_cmac_start(AES_KeySlot keyslot, fp_aes_get_chunk fp, unsigned int sizeIn, unsigned char *dataOut);
	extern AES_WORK_STATE aes_encrypt_cmac_update(AES_HANDLE handle);
	extern void aes_encrypt_cmac_finish(AES_HANDLE *handle);


	extern AES_KeyHolder aes_KeyHolder[AES_MAXKEY];
	extern AES_KeyHolderNv aes_KeyHolderNv[AES_MAXKEY];

	extern void aes_cmac_init(void);
	extern void do_aes_key_1ms(void);
	extern void init_aes_key(void);

	extern void init_aes_128(void);
	extern void deinit_aes_128(void);
	extern void do_aes_128(void);
	extern void isr_aes_128_1ms(void);
	
	//Helper functions


#endif
