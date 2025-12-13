#include <string.h>
#include <stdio.h>
#include "aes_128.h"

#include "k_stdtype.h"

uint8 do_aes_128_1ms = 0;

__attribute__(( weak )) void aes_cmac_init(void) {}

void init_aes_128(void) {
	init_aes_key();
	aes_cmac_init();
}

void deinit_aes_128(void) {
}

void do_aes_128(void) {
	if (do_aes_128_1ms) {
		do_aes_128_1ms = 0;
		do_aes_key_1ms();
	}
}

void isr_aes_128_1ms(void) {
	do_aes_128_1ms = 1;
}

void aes_block_xor(unsigned char *a, unsigned char *b, unsigned int len) {
	unsigned int x = 0;
	for (x = 0; x < len; x++) {
		*(a + x) = (*(a + x) ^ *(b + x));
	}
}

void aes_block_buffer_xor(unsigned char *a, unsigned char *b, unsigned char *out, unsigned int len) {
    unsigned int x = 0;
    for (x = 0; x < len; x++)
    {
        out[x] = a[x] ^ b[x];
    }
}
