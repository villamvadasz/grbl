#ifndef _RIJNDAEL_H_
#define _RIJNDAEL_H_

	#include "k_stdtype.h"
	#include "c_aes_128.h"

	#define AES_KEYBITS_MAX 256
	#define AES_BLOCK_SIZE 128

	typedef uint8 u8;
	typedef uint32 u32;

	typedef enum _AES_WORK_STATE {
		AES_WORK_STATE_EMPTY = 0,
		AES_WORK_STATE_BUSY,
		AES_WORK_STATE_FINISHED,
	} AES_WORK_STATE;

	typedef struct _Aes_Working_Variables {
		u32 s0;
		u32 s1; 
		u32 s2; 
		u32 s3; 
		u32 t0; 
		u32 t1; 
		u32 t2; 
		u32 t3;
		int r;
		u32 state;
		u32 rk_add_value;
	} Aes_Working_Variables;
	
	extern int rijndaelSetupEncrypt(u32 *rk, const u8 *key, int keybits);
	extern int rijndaelSetupDecrypt(u32 *rk, const u8 *key, int keybits);

	extern void rijndaelEncrypt_sync(const u32 *rk, int nrounds, const u8 *plaintext, u8 *ciphertext);
	extern void rijndaelDecrypt_sync(const u32 *rk, int nrounds, const u8 *ciphertext, u8 *plaintext);
	extern AES_WORK_STATE rijndaelEncrypt_async(const u32 *rk, int nrounds, const u8 *plaintext, u8 *ciphertext, Aes_Working_Variables *working_variables);
	extern AES_WORK_STATE rijndaelDecrypt_async(const u32 *rk, int nrounds, const u8 *ciphertext, u8 *plaintext, Aes_Working_Variables *working_variables);

#endif
