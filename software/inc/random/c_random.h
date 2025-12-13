#ifndef _C_RANDOM_H_
#define _C_RANDOM_H_

	#error You have included an example of c_random.h config

	#include "random.h"

	#define RANDOM_ENTROPY_SOURCE RANDOM_ENTROPY_SOURCE_AD_NOISE
	#define RANDOM_ENTROPY_FLATTENER	RANDOM_ENTROPY_FLATTENER_SHA256
	#define RANDOM_AD_CHANNEL 5


#endif
