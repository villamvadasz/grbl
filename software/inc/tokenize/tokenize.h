#ifndef _TOKENIZE_H_
#define _TOKENIZE_H_

	#include "k_stdtype.h"

	extern unsigned int tokenize(char * strBuffer, char * tokenBuffer[], int cnt, unsigned char token1, int token2, int token3);
	extern unsigned char * getTokenCnt(unsigned char *dataIn, unsigned int item, unsigned char token1, int token2, int token3);
	extern uint8 isStrPresent(char *str, const char * s, uint16 start, uint16 minimalLength);
	extern uint8 isStrPresentNonCaseSensitive(char *str, const char * s, uint16 start, uint16 minimalLength);
	extern uint8 isStrOnlyPresent(char *str, const char * s);
	extern void safestrcpy(char *destStr, char *sourceStr, int maxSize);
	extern sint16 hexStringToByte(char *str);
	extern uint16 strToInt16(char *str);

	extern void tokenize_strlwr(char *str);
	extern void tokenize_trim(char *str);
	extern unsigned char tokenize_hexstring_to_charvalue(char *str, unsigned char *raw);

#endif
