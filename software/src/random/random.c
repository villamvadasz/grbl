#include <string.h>
#include "random.h"
#include "c_random.h"

#ifdef USE_RANDOM

#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_TIME_JITTER)
	#include "tmr.h"
#endif
#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_TIME_JITTER_BUFFER)
	#include "tmr.h"
	#include "ringBuffer.h"
#endif
#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_AD_NOISE)
	#include "ad.h"
	#include "ringBuffer.h"
#endif

#if (RANDOM_ENTROPY_FLATTENER == RANDOM_ENTROPY_FLATTENER_AES)
	#include "aes_128.h"
#endif
#if (RANDOM_ENTROPY_FLATTENER == RANDOM_ENTROPY_FLATTENER_SHA256)
	#include "sha256.h"
#endif


uint32 random_seed = 0;

#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_TIME_JITTER)
	uint32 random_time_prev = 0;
	uint32 random_time_curr = 0;
	sint32 random_time_diff = 0;
	sint32 random_time_diff_prev = 0;
	sint32 random_time_diff_curr = 0;
#endif

#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_TIME_JITTER_BUFFER)
	uint32 random_time_prev = 0;
	uint32 random_time_curr = 0;
	sint32 random_time_diff = 0;
	sint32 random_time_diff_prev = 0;
	sint32 random_time_diff_curr = 0;
	uint8 random_seed_cnt = 0;
	uint8 random_seed_accu = 0;
	#define RANDOM_USE_ACCU
	#define RANDOM_USE_BUFFER
#endif
#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_AD_NOISE)
	uint16 random_adValue_curr = 0;
	uint16 random_adValue_prev = 0;
	uint8 random_seed_cnt = 0;
	uint8 random_seed_accu = 0;
	#define RANDOM_USE_ACCU
	#define RANDOM_USE_BUFFER
#endif

#ifdef RANDOM_USE_BUFFER
	unsigned char random_time_buffer[64];
	RingBuffer random_time;
#endif

#if (RANDOM_ENTROPY_FLATTENER == RANDOM_ENTROPY_FLATTENER_AES)
	unsigned char random_aes_data[16] = {0};
#endif
#if (RANDOM_ENTROPY_FLATTENER == RANDOM_ENTROPY_FLATTENER_SHA256)
	SHA256_CTX random_ctx;
	unsigned char random_sha256_data[32] = {0};
#endif


volatile uint32 do_random_1ms = 0;
#ifdef RANDOM_USE_ACCU
	void random_accumulate_value(uint32 prev, uint32 curr, uint8 *accu, uint8 *cnt, RingBuffer *buff);
#endif

uint32 random_prng(uint32 *seed);
	
void random_setSeed(uint32 seed) {
	random_seed = seed;
}

uint32 random_getValue(void) {
	uint32 result = 0;
	#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_SEED)
		result = random_prng(&random_seed);
	#endif
	#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_TIME_JITTER)
		result = random_prng(&random_seed);
	#endif
	#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_TIME_JITTER_BUFFER)
		unsigned char result1 = random_prng(&random_seed);
		unsigned char result2 = random_prng(&random_seed);
		unsigned char result3 = random_prng(&random_seed);
		unsigned char result4 = random_prng(&random_seed);
		
		ringBuffer_getItem(&random_time, &result1);
		ringBuffer_getItem(&random_time, &result2);
		ringBuffer_getItem(&random_time, &result3);
		ringBuffer_getItem(&random_time, &result4);
		
		random_seed = 0;
		random_seed <<= 8;
		random_seed += result1;
		random_seed <<= 8;
		random_seed += result2;
		random_seed <<= 8;
		random_seed += result3;
		random_seed <<= 8;
		random_seed += result4;
		
		result = random_prng(&random_seed);
	#endif
	#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_AD_NOISE)
		result = random_prng(&random_seed);
	#endif
	return result;
}

void init_random(void) {
	#ifdef RANDOM_USE_BUFFER
		ringBuffer_initBuffer(&random_time, random_time_buffer, sizeof(random_time_buffer) / sizeof(*random_time_buffer));
	#endif
}

void do_random(void) {
	if (do_random_1ms) {
		do_random_1ms = 0;
		{
			#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_TIME_JITTER)
				random_time_prev = random_time_curr;
				random_time_curr = getGlobalTimeUs();
				random_time_diff = random_time_curr - random_time_prev;
				
				random_time_diff_prev = random_time_diff_curr;
				random_time_diff_curr = random_time_diff;
				
				random_seed << = 1;
				if (random_time_diff_prev > random_time_diff_curr) {
					random_seed |= 1;
				}
			#endif
			#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_TIME_JITTER_BUFFER)
				random_time_prev = random_time_curr;
				random_time_curr = getGlobalTimeUs();
				random_time_diff = random_time_curr - random_time_prev;
				
				random_time_diff_prev = random_time_diff_curr;
				random_time_diff_curr = random_time_diff;
				
				random_accumulate_value(random_time_diff_prev, random_time_diff_curr, &random_seed_accu, &random_seed_cnt, &random_time);
			#endif
			#if (RANDOM_ENTROPY_SOURCE == RANDOM_ENTROPY_SOURCE_AD_NOISE)
				uint16 ad_value = getAd(RANDOM_AD_CHANNEL);
				random_adValue_prev = random_adValue_curr;
				random_adValue_curr = ad_value;

				random_accumulate_value(random_adValue_prev, random_adValue_curr, &random_seed_accu, &random_seed_cnt, &random_time);
			#endif
		}
	}
}

void isr_random_1ms(void) {
	do_random_1ms = 1;
}

uint32 random_prng(uint32 *seed) {
	uint32 result = 0;
	#if (RANDOM_ENTROPY_FLATTENER == RANDOM_ENTROPY_FLATTENER_PRNG)
		uint32 a = 0x41A7;
		uint32 m = 0x7FFFFFFF;
		*seed = (a * *seed) % m;
		result = random_seed / m;
	#endif
	#if (RANDOM_ENTROPY_FLATTENER == RANDOM_ENTROPY_FLATTENER_AES)

		AES_KeySlot keyslot = AES_ZERO_KEY;
		unsigned char dataIn[16];
		unsigned int sizeIn = 16;
		unsigned char dataOut[16];

		memcpy(dataIn, random_aes_data, 16);
		dataIn[0] ^= (*seed) & 0xFF;
		dataIn[1] ^= ((*seed) >> 8) & 0xFF;
		dataIn[2] ^= ((*seed) >> 16) & 0xFF;
		dataIn[3] ^= ((*seed) >> 24) & 0xFF;
		aes_encrypt_cmac(keyslot, dataIn, sizeIn, dataOut);
		memcpy(random_aes_data, dataOut , 16);

		result = 0;
		result <<= 8;
		result += dataOut[0];
		result <<= 8;
		result += dataOut[1];
		result <<= 8;
		result += dataOut[2];
		result <<= 8;
		result += dataOut[3];

	#endif
	#if (RANDOM_ENTROPY_FLATTENER == RANDOM_ENTROPY_FLATTENER_SHA256)
		BYTE dataIn[SHA256_BLOCK_SIZE] = {0};
		size_t len = 4;
		BYTE hash[SHA256_BLOCK_SIZE];

		memcpy(dataIn, random_sha256_data, SHA256_BLOCK_SIZE);
		dataIn[0] ^= (*seed) & 0xFF;
		dataIn[1] ^= ((*seed) >> 8) & 0xFF;
		dataIn[2] ^= ((*seed) >> 16) & 0xFF;
		dataIn[3] ^= ((*seed) >> 24) & 0xFF;

		sha256_init(&random_ctx);
		sha256_update(&random_ctx, dataIn, len);
		sha256_final(&random_ctx, hash);

		memcpy(random_sha256_data, hash , SHA256_BLOCK_SIZE);

		result = 0;
		result <<= 8;
		result += hash[0];
		result <<= 8;
		result += hash[1];
		result <<= 8;
		result += hash[2];
		result <<= 8;
		result += hash[3];

	#endif
	
	return result;
}

#ifdef RANDOM_USE_ACCU
	void random_accumulate_value(uint32 prev, uint32 curr, uint8 *accu, uint8 *cnt, RingBuffer *buff) {
		if (accu != NULL) {
			if (prev != curr) {
				*accu <<= 1;
				if (prev > curr) {
					*accu |= 1;
				}
				if (cnt != NULL) {
					(*cnt) += 1;
					if (*cnt >= (sizeof(uint8) * 8 )) {
						*cnt = 0;
						#ifdef RANDOM_USE_BUFFER
							if (buff != NULL) {
								ringBuffer_addItem(buff, *accu);
							}
						#endif
					}
				}
			}
		}
	}
#endif

#endif