#include <string.h>
#include <stdio.h>
#include "aes_128.h"

#include "k_stdtype.h"

static void generate_subkey(AES_KeySlot keyslot, unsigned char *K1, unsigned char *K2);
static void padding ( unsigned char *lastb, unsigned char *pad, int length, int keylength);
static void leftshift_onebit(unsigned char *input,unsigned char *output);

typedef struct _AES_CMAC_HANDLE_STORE {
	AES_KeySlot keyslot;
	fp_aes_get_chunk fp;
	unsigned char dataInActual[KEYLENGTH(AES_KEYBITS_MAX)];
	unsigned int sizeIn;
	unsigned char *dataOut;
	
	unsigned char X[KEYLENGTH(AES_KEYBITS_MAX)];
	unsigned char Y[KEYLENGTH(AES_KEYBITS_MAX)];
	unsigned char K1[KEYLENGTH(AES_KEYBITS_MAX)];
	unsigned char K2[KEYLENGTH(AES_KEYBITS_MAX)];
	unsigned int n;
	unsigned int i; 
	unsigned int flag;
	
	unsigned int aes_statemachine;
	AES_HANDLE aes_handle;
} AES_CMAC_HANDLE_STORE;

AES_CMAC_HANDLE_STORE aes_cmac_handle_store[3];

//unsigned char test_key1[KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits)] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
//unsigned char test_plain1[KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits)] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
//unsigned char test_mac1[KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits)] = {0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44, 0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c};
//
//unsigned char test_key2[KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits)] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
//unsigned char test_plain2[KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits) * 2] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, 0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51, 0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11};
//unsigned char test_mac2[KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits)] = {0xdf, 0xa6, 0x67, 0x47, 0xde, 0x9a, 0xe6, 0x30, 0x30, 0xca, 0x32, 0x61, 0x14, 0x97, 0xc8, 0x27};

void aes_cmac_init(void) {
	unsigned int x = 0;
	for (x = 0; x < (sizeof(aes_cmac_handle_store) / sizeof(*aes_cmac_handle_store)); x++) {
		aes_cmac_handle_store[x].keyslot = AES_ROM;
		aes_cmac_handle_store[x].fp = NULL;
		aes_cmac_handle_store[x].sizeIn = 0;
		aes_cmac_handle_store[x].dataOut = NULL;
		aes_cmac_handle_store[x].aes_statemachine = 0;
		aes_cmac_handle_store[x].aes_handle = AES_INVALID_HANDLE;
	}
}

AES_HANDLE aes_encrypt_cmac_start(AES_KeySlot keyslot, fp_aes_get_chunk fp, unsigned int sizeIn, unsigned char *dataOut) {
	AES_HANDLE result = AES_INVALID_HANDLE;
	unsigned int x = 0;
	if ((keyslot < AES_MAXKEY) && (fp != NULL) && (sizeIn != 0) && (dataOut != NULL)) {
		for (x = 0; x < (sizeof(aes_cmac_handle_store) / sizeof(*aes_cmac_handle_store)); x++) {
			if (aes_cmac_handle_store[x].aes_handle == AES_INVALID_HANDLE) {
				aes_cmac_handle_store[x].keyslot = keyslot;
				aes_cmac_handle_store[x].fp = fp;
				aes_cmac_handle_store[x].sizeIn = sizeIn;
				aes_cmac_handle_store[x].dataOut = dataOut;
				aes_cmac_handle_store[x].aes_statemachine = 0;
				aes_cmac_handle_store[x].aes_handle = x;
				result = x;
				break;
			}
		}	
	}
	return result;
}

AES_WORK_STATE aes_encrypt_cmac_update(AES_HANDLE handle) {
	AES_WORK_STATE result = AES_WORK_STATE_EMPTY;
	if (aes_cmac_handle_store[handle].aes_handle == handle) {
		if (aes_cmac_handle_store[handle].sizeIn == 0) {
			result = AES_WORK_STATE_FINISHED;
		} else {
			switch (aes_cmac_handle_store[handle].aes_statemachine) {
				case 0 : {
					memset(aes_cmac_handle_store[handle].Y, 0x00, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits));
					memset(aes_cmac_handle_store[handle].K1, 0x00, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits));
					memset(aes_cmac_handle_store[handle].K2, 0x00, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits));
					aes_cmac_handle_store[handle].n = 0;
					aes_cmac_handle_store[handle].i = 0;
					aes_cmac_handle_store[handle].flag = 0;
					
					generate_subkey(aes_cmac_handle_store[handle].keyslot, aes_cmac_handle_store[handle].K1, aes_cmac_handle_store[handle].K2);

					if (aes_cmac_handle_store[handle].sizeIn == 0xFFFFFFFF) {
						aes_cmac_handle_store[handle].n = 0xFFFFFFFF;	   /* n is number of rounds */
					} else {
						aes_cmac_handle_store[handle].n = (aes_cmac_handle_store[handle].sizeIn + 15) / KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits);	   /* n is number of rounds */
					}

					if ( aes_cmac_handle_store[handle].n == 0 ) {
						aes_cmac_handle_store[handle].n = 1;
						aes_cmac_handle_store[handle].flag = 0;
					} else {
						if ( (aes_cmac_handle_store[handle].sizeIn % KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits)) == 0 ) { /* last block is a complete block */
							aes_cmac_handle_store[handle].flag = 1;
						} else { /* last block is not complete block */
							aes_cmac_handle_store[handle].flag = 0;
						}
					}

					memset(aes_cmac_handle_store[handle].X, 0x00, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits));
					aes_cmac_handle_store[handle].i = 0;
					aes_cmac_handle_store[handle].aes_statemachine = 1;
					break;
				}
				case 1 : {
					unsigned char sizeRead = 0;
					AES_CHUNK_STATE aes_chunk_state = aes_cmac_handle_store[handle].fp(aes_cmac_handle_store[handle].dataInActual, &sizeRead);
					if (aes_chunk_state != AES_CHUNK_STATE_WAIT) {
						if (aes_chunk_state == AES_CHUNK_STATE_NOT_LAST_BLOCK) {
							if (aes_cmac_handle_store[handle].i < (aes_cmac_handle_store[handle].n - 1)) {
								aes_cmac_handle_store[handle].aes_statemachine = 2;
							} else {
								aes_cmac_handle_store[handle].aes_statemachine = 3;
							}
						} else if (aes_chunk_state == AES_CHUNK_STATE_LAST_BLOCK) {
							if (sizeRead == KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits)) {
								aes_cmac_handle_store[handle].flag = 1;
							} else {
								aes_cmac_handle_store[handle].sizeIn = sizeRead;
								aes_cmac_handle_store[handle].flag = 0;
							}
							aes_cmac_handle_store[handle].aes_statemachine = 3;
						} else {
							aes_cmac_handle_store[handle].aes_statemachine = 3;
						}
					}
					break;
				}
				case 2 : {
					aes_block_buffer_xor(aes_cmac_handle_store[handle].X, &aes_cmac_handle_store[handle].dataInActual[0], aes_cmac_handle_store[handle].Y, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits)); /* Y := Mi (+) X  */
					//AES_128(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].key,Y,X);	  /* X := AES-128(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].key, Y); */
					//aes_set_key(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].key, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits), &ctx);
					//aesencrypt(Y, X, &ctx);
					aes_encrypt_ecb(aes_cmac_handle_store[handle].keyslot, aes_cmac_handle_store[handle].Y, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits), aes_cmac_handle_store[handle].X);
					aes_cmac_handle_store[handle].i++;
					aes_cmac_handle_store[handle].aes_statemachine = 1;
					break;
				}
				case 3 : {
					unsigned char M_last[KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits)];
					
					if ( aes_cmac_handle_store[handle].flag ) { /* last block is complete block */
						aes_block_buffer_xor(&aes_cmac_handle_store[handle].dataInActual[0], aes_cmac_handle_store[handle].K1, M_last, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits));
					} else {
						unsigned char padded[KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits)];
						padding(&aes_cmac_handle_store[handle].dataInActual[0], padded, aes_cmac_handle_store[handle].sizeIn % KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits), KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits));
						aes_block_buffer_xor(padded, aes_cmac_handle_store[handle].K2, M_last, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits));
					}

					aes_block_buffer_xor(aes_cmac_handle_store[handle].X, M_last, aes_cmac_handle_store[handle].Y, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits));
					//AES_128(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].key,Y,X);
					//aes_set_key(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].key, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits), &ctx);
					//aesencrypt(Y, X, &ctx);
					aes_encrypt_ecb(aes_cmac_handle_store[handle].keyslot, aes_cmac_handle_store[handle].Y, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits), aes_cmac_handle_store[handle].X);

					memcpy(aes_cmac_handle_store[handle].dataOut, aes_cmac_handle_store[handle].X, KEYLENGTH(aes_KeyHolder[aes_cmac_handle_store[handle].keyslot].keybits));
					aes_cmac_handle_store[handle].sizeIn = 0;
					aes_cmac_handle_store[handle].aes_statemachine = 0;
					break;
				}
				default : {
					aes_cmac_handle_store[handle].aes_statemachine = 0;
					break;
				}
			}
			result = AES_WORK_STATE_BUSY;
		}
	}
	return result;
}

void aes_encrypt_cmac_finish(AES_HANDLE *handle) {
	if ((handle != NULL) && (aes_cmac_handle_store[*handle].aes_handle == *handle)) {
		aes_cmac_handle_store[*handle].aes_statemachine = 0;
		aes_cmac_handle_store[*handle].aes_handle = AES_INVALID_HANDLE;
		*handle = AES_INVALID_HANDLE;
	}
}

int aes_encrypt_cmac(AES_KeySlot keyslot, unsigned char *dataIn, unsigned int sizeIn, unsigned char *dataOut) {
	int result = -1;
	if ((keyslot < AES_MAXKEY) && (dataIn != NULL) && (sizeIn != 0) && (dataOut != NULL)) {
		unsigned char X[KEYLENGTH(AES_BLOCK_SIZE)];
		unsigned char Y[KEYLENGTH(AES_BLOCK_SIZE)];
		unsigned char M_last[KEYLENGTH(AES_BLOCK_SIZE)];
		unsigned char K1[KEYLENGTH(AES_BLOCK_SIZE)];
		unsigned char K2[KEYLENGTH(AES_BLOCK_SIZE)];
		int n = 0;
		int i = 0; 
		int flag = 0;
		
		generate_subkey(keyslot, K1, K2);

		n = (sizeIn + 15) / KEYLENGTH(AES_BLOCK_SIZE);	   /* n is number of rounds */

		if ( n == 0 ) {
			n = 1;
			flag = 0;
		} else {
			if ( (sizeIn % KEYLENGTH(AES_BLOCK_SIZE)) == 0 ) { /* last block is a complete block */
				flag = 1;
			} else { /* last block is not complete block */
				flag = 0;
			}
		}

		if ( flag ) { /* last block is complete block */
			aes_block_buffer_xor(&dataIn[KEYLENGTH(AES_BLOCK_SIZE) * (n - 1)], K1, M_last, KEYLENGTH(AES_BLOCK_SIZE));
		} else {
			unsigned char padded[KEYLENGTH(AES_KEYBITS_MAX)];
			padding(&dataIn[KEYLENGTH(AES_BLOCK_SIZE) * (n - 1)], padded, sizeIn % KEYLENGTH(AES_BLOCK_SIZE), KEYLENGTH(AES_BLOCK_SIZE));
			aes_block_buffer_xor(padded, K2, M_last, KEYLENGTH(AES_BLOCK_SIZE));
		}

		memset(X, 0x00, KEYLENGTH(AES_BLOCK_SIZE));
		for ( i = 0; i < (n-1); i++ ) {
			aes_block_buffer_xor(X, &dataIn[KEYLENGTH(AES_BLOCK_SIZE)*i], Y, KEYLENGTH(AES_BLOCK_SIZE)); /* Y := Mi (+) X  */
			//AES_128(aes_KeyHolder[keyslot].key,Y,X);	  /* X := AES-128(aes_KeyHolder[keyslot].key, Y); */
			//aes_set_key(aes_KeyHolder[keyslot].key, KEYLENGTH(AES_BLOCK_SIZE), &ctx);
			//aesencrypt(Y, X, &ctx);
			aes_encrypt_ecb(keyslot, Y, KEYLENGTH(AES_BLOCK_SIZE), X);
		}

		aes_block_buffer_xor(X, M_last, Y, KEYLENGTH(AES_BLOCK_SIZE));
		//AES_128(aes_KeyHolder[keyslot].key,Y,X);
		//aes_set_key(aes_KeyHolder[keyslot].key, KEYLENGTH(AES_BLOCK_SIZE), &ctx);
		//aesencrypt(Y, X, &ctx);
		aes_encrypt_ecb(keyslot, Y, KEYLENGTH(AES_BLOCK_SIZE), X);

		memcpy(dataOut, X, KEYLENGTH(AES_BLOCK_SIZE));
		result = 1;
	}
	return result;
}

static void generate_subkey(AES_KeySlot keyslot, unsigned char *K1, unsigned char *K2) {
	unsigned char const_Rb[KEYLENGTH(AES_BLOCK_SIZE)] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87
	};
	unsigned char L[KEYLENGTH(AES_BLOCK_SIZE)];
	unsigned char Z[KEYLENGTH(AES_BLOCK_SIZE)];
	unsigned char tmp[KEYLENGTH(AES_BLOCK_SIZE)];

	memset(Z, 0x00, KEYLENGTH(AES_BLOCK_SIZE));

	//AES_128(key,Z,L);
	//aes_set_key(key, KEYLENGTH(aes_KeyHolder[keyslot].keybits), &ctx);
	//aesencrypt(Z, L, &ctx);
	aes_encrypt_ecb(keyslot, Z, KEYLENGTH(AES_BLOCK_SIZE), L);

	if ( (L[0] & 0x80) == 0 ) { /* If MSB(L) = 0, then K1 = L << 1 */
		leftshift_onebit(L,K1);
	} else {	/* Else K1 = ( L << 1 ) (+) Rb */
		leftshift_onebit(L,tmp);
		aes_block_buffer_xor(tmp, const_Rb, K1, KEYLENGTH(AES_BLOCK_SIZE));
	}

	if ( (K1[0] & 0x80) == 0 ) {
		leftshift_onebit(K1,K2);
	} else {
		leftshift_onebit(K1,tmp);
		aes_block_buffer_xor(tmp, const_Rb, K2, KEYLENGTH(AES_BLOCK_SIZE));
	}
	return;
}

static void padding ( unsigned char *lastb, unsigned char *pad, int length, int keylength) {
	int j = 0;

	/* original last block */
	for ( j=0; j<keylength; j++ ) {
		if ( j < length ) {
			pad[j] = lastb[j];
		} else if ( j == length ) {
			pad[j] = 0x80;
		} else {
			pad[j] = 0x00;
		}
	}
}

static void leftshift_onebit(unsigned char *input,unsigned char *output) {
	int i = 0;
	unsigned char overflow = 0;

	for ( i=15; i>=0; i-- ) {
		output[i] = input[i] << 1;
		output[i] |= overflow;
		overflow = (input[i] & 0x80)?1:0;
	}
	return;
}
