#ifndef _RANDOM_H_
#define _RANDOM_H_

	#include "k_stdtype.h"

	#define RANDOM_ENTROPY_SOURCE_SEED	1
	#define RANDOM_ENTROPY_SOURCE_TIME_JITTER	2
	#define RANDOM_ENTROPY_SOURCE_TIME_JITTER_BUFFER	3
	#define RANDOM_ENTROPY_SOURCE_AD_NOISE	4
	#define RANDOM_ENTROPY_SOURCE_5	5

	#define RANDOM_ENTROPY_FLATTENER_PRNG	1
	#define RANDOM_ENTROPY_FLATTENER_AES	2
	#define RANDOM_ENTROPY_FLATTENER_SHA256	3

	extern void random_setSeed(uint32 seed);
	extern uint32 random_getValue(void);

	extern void init_random(void);
	extern void do_random(void);
	extern void isr_random_1ms(void);

#endif
