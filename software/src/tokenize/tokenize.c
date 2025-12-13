#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "tokenize.h"

unsigned int tokenize(char * strBuffer, char * tokenBuffer[], int cnt, unsigned char token1, int token2, int token3) {
	unsigned int result = 0;
	{
		unsigned int i = 0;
		for (i = 0; i < cnt; i++) {
			unsigned char *item = NULL;
			item = getTokenCnt((unsigned char *)strBuffer, i, token1, token2, token3);
			if (item == NULL) {
				break;
			} else {
				result ++;
			}
			tokenBuffer[i] = (char *)item;
		}
	}
	return result;
}

unsigned char * getTokenCnt(unsigned char *dataIn, unsigned int item, unsigned char token1, int token2, int token3) {
	unsigned char * result = NULL;
	unsigned int i = 0;
	unsigned char ch = 0;
	unsigned int tokenCnt = 0;
	if (dataIn != NULL) {
		if (item == 0) {
			result = dataIn;
		} else {
			while (1) {
				ch = dataIn[i];
				i++;
				if (i == 0) {
					break;
				}
				if (ch != 0x00) {
					if ((ch == token1) || (ch == token2) || (ch == token3)) {
						tokenCnt++;
						if (tokenCnt == item) {
							result = dataIn + i;
							break;
						}
					} else {
					}
				} else {
					break;
				}
			}
		}
	}
	return result;
}

uint8 isStrPresent(char *str, const char *s, uint16 start, uint16 minimalLength) {
	uint8 result = 0;
	if ((str != NULL) && (s != NULL)) {
		uint16 strLen = strlen(str);
		if (strLen >= minimalLength) {
			uint16 x = 0;
			uint16 length = strlen(s);
			result = 1;
			for (x = 0; x < length; x++) {
				if (str[start + x] != s[x]) {
					result = 0;
					break;
				}
			}
		}
	}
	return result;
}

uint8 isStrPresentNonCaseSensitive(char *str, const char * s, uint16 start, uint16 minimalLength) {
	uint8 result = 0;
	if ((str != NULL) && (s != NULL)) {
		uint16 strLen = strlen(str);
		if (strLen >= minimalLength) {
			uint16 x = 0;
			uint16 length = strlen(s);
			result = 1;
			for (x = 0; x < length; x++) {
				char a = str[start + x];
				char b = s[x];
				if ((a >= 'A') && (a <= 'Z')) {
					a -= 'A';
					a += 'a';
				}
				if ((b >= 'A') && (b <= 'Z')) {
					b -= 'A';
					b += 'a';
				}
				if (a != b) {
					result = 0;
					break;
				}
			}
		}
	}
	return result;
}

uint8 isStrOnlyPresent(char *str, const char * s) {
	uint8 result = 0;
	if ((str != NULL) && (s != NULL)) {
		uint16 strLen = strlen(str);
		uint16 length = strlen(s);
		if (strLen == length) {
			uint16 x = 0;
			result = 1;
			for (x = 0; x < length; x++) {
				if (str[x] != s[x]) {
					result = 0;
					break;
				}
			}
		}
	}
	return result;
}

void safestrcpy(char *destStr, char *sourceStr, int maxSize) {
	int x = 0;
	for (x = 0; x < maxSize; x++) {
		if (*sourceStr == 0) {
			break;
		}
		*destStr = *sourceStr;
		destStr++;
		sourceStr++;
	}
}

sint16 hexStringToByte(char *str) {
	sint16 result = -1;
	if (str != NULL) {
		if (strlen(str) >= 2) {
			uint16 x = 0;
			for (x = 0; x < 2; x++) {
				char ch = str[x];
				if (
						(
							(
								(ch >= '0') &&
								(ch <= '9')
							) ||
							(
								(ch >= 'A') &&
								(ch <= 'F')
							) ||
							(
								(ch >= 'a') &&
								(ch <= 'f')
							)
						)
				) {
					result <<= 4;
					if ((ch >= '0') && (ch <= '9')) {
						result += (ch - '0');
					}
					if ((ch >= 'A') && (ch <= 'F')) {
						result += (ch - 'A') + 0x0A;
					}
					if ((ch >= 'a') && (ch <= 'f')) {
						result += (ch - 'a') + 0x0A;
					}
				}
			}
		}
	}
	return result;
}

uint16 strToInt16(char *str) {
	uint16 result = 0;
	if (str != NULL) {
		uint8 x = 0;
		uint16 itemCnt = 0;
		for (x = 0; x < strlen(str); x++) {
			char currentChar = str[x];
			if ((currentChar >= '0') && (currentChar <= '9')) {
				itemCnt *= 10;
				itemCnt += currentChar - '0';
			} else if (currentChar == '\r') {
				break;
			} else if (currentChar == '\n') {
				break;
			} else {
				break;
			}
		}
		result = itemCnt;
	}
	return result;
}

void tokenize_strlwr(char *str) {
	while (*str != 0) {
		*str = tolower(*str);
		str++;
	}
}

void tokenize_trim(char *str) {
	while (*str != 0) {
		if ((*str == '\r') || (*str == '\n')) {
			*str = 0;
			break;
		}
		str++;
	}
}

unsigned char tokenize_hexstring_to_charvalue(char *str, unsigned char *raw) {
	unsigned char result = 0;
	if ((str != NULL) && (raw != NULL)) {
		char byte = 0;
		char upper_lower = 1;
		result = 1;
		while (*str != 0) {
			byte = *str;
			if (isxdigit(byte)) {
				if (byte >= '0' && byte <= '9') byte = byte - '0';
				else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
				else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
				if (upper_lower % 2) {
					*raw = 0;
					*raw = byte;//high nibble
					*raw <<= 4;
				} else {
					*raw += byte;//low nibble
					raw++;
				}
				upper_lower++;
			} else {
				result = 0;
				break;
			}
			
			str++;
		}
	}
	return result;
}
