#include "ringBuffer.h"
#include "c_ringBuffer.h"
#include <string.h>
#include "mal.h"

int ringBuffer_initBuffer(RingBuffer *ringBuffer, unsigned char *buffer, unsigned int bufferSize) {
	int result = -1;
	if ((ringBuffer != NULL) && (buffer != NULL) && (bufferSize > 1)) {
		ringBuffer->buffer = buffer;
		ringBuffer->bufferSize = bufferSize;
		ringBuffer->bufferRead = buffer;
		ringBuffer->bufferWrite = buffer;
		ringBuffer->bufferMaximumFillness = 0;
		ringBuffer->bufferFillness = 0;
		memset((void *)ringBuffer->buffer, 0x00 , ringBuffer->bufferSize);
		result = 0;
	}
	return result;
}

unsigned int ringBuffer_getFillness(RingBuffer *ringBuffer) {
	unsigned int result = 0;
	if (ringBuffer != NULL) {
		int delta = 0;
		#ifdef RINGBUFFER_USE_DOUBLE_READ
			volatile unsigned char *bufferReadTemp = ringBuffer->bufferRead;
			volatile unsigned char *bufferWriteTemp = ringBuffer->bufferWrite;
			volatile unsigned char *bufferReadTemp2 = ringBuffer->bufferRead;
			volatile unsigned char *bufferWriteTemp2 = ringBuffer->bufferWrite;
			while (1) {
				bufferReadTemp = ringBuffer->bufferRead;
				bufferReadTemp2 = ringBuffer->bufferRead;
				if (bufferReadTemp == bufferReadTemp2) {
					break;
				}
			}
			while (1) {
				bufferWriteTemp = ringBuffer->bufferWrite;
				bufferWriteTemp2 = ringBuffer->bufferWrite;
				if (bufferWriteTemp == bufferWriteTemp2) {
					break;
				}
			}
		
		#else
			volatile unsigned char *bufferReadTemp = ringBuffer->bufferRead;	//This must be atomic
			volatile unsigned char *bufferWriteTemp = ringBuffer->bufferWrite;	//This must be atomic
		#endif
		if (bufferReadTemp > bufferWriteTemp) {
			delta = (int)bufferReadTemp - (int)bufferWriteTemp;
			delta = ringBuffer->bufferSize - delta;
		} else {
			delta = (int)bufferWriteTemp - (int)bufferReadTemp;
		}		
		if (delta != 0) {
			result = delta;
		}
	}
	return result;
}

unsigned int ringBuffer_getEmptyness(RingBuffer *ringBuffer) {
	unsigned int result = 0;
	if (ringBuffer != NULL) {
		int delta = 0;
		#ifdef RINGBUFFER_USE_DOUBLE_READ
			volatile unsigned char *bufferReadTemp = ringBuffer->bufferRead;
			volatile unsigned char *bufferWriteTemp = ringBuffer->bufferWrite;
			volatile unsigned char *bufferReadTemp2 = ringBuffer->bufferRead;
			volatile unsigned char *bufferWriteTemp2 = ringBuffer->bufferWrite;
			while (1) {
				bufferReadTemp = ringBuffer->bufferRead;
				bufferReadTemp2 = ringBuffer->bufferRead;
				if (bufferReadTemp == bufferReadTemp2) {
					break;
				}
			}
			while (1) {
				bufferWriteTemp = ringBuffer->bufferWrite;
				bufferWriteTemp2 = ringBuffer->bufferWrite;
				if (bufferWriteTemp == bufferWriteTemp2) {
					break;
				}
			}
		
		#else
			volatile unsigned char *bufferReadTemp = ringBuffer->bufferRead;	//This must be atomic
			volatile unsigned char *bufferWriteTemp = ringBuffer->bufferWrite;	//This must be atomic
		#endif
		if (bufferReadTemp > bufferWriteTemp) {
			delta = (int)bufferReadTemp - (int)bufferWriteTemp;
		} else {
			delta = (int)bufferWriteTemp - (int)bufferReadTemp;
			delta = ringBuffer->bufferSize - delta;
		}		
		if (delta != 0) {
			result = delta;
		}
	}
	return result;
}

int ringBuffer_addItem(RingBuffer *ringBuffer, unsigned char data) {
	int result = -1;
	if ((ringBuffer != NULL) && (ringBuffer->bufferWrite != NULL)) {
		*ringBuffer->bufferWrite = data;
		ringBuffer->bufferWrite++;
		
		#ifdef RINGBUFFER_USE_FILLNESS
			ringBuffer->bufferFillness++;
			if (ringBuffer->bufferMaximumFillness < ringBuffer->bufferFillness) {
				ringBuffer->bufferMaximumFillness = ringBuffer->bufferFillness;
			}
		#endif

		if (ringBuffer->buffer + ringBuffer->bufferSize == ringBuffer->bufferWrite) {
			ringBuffer->bufferWrite = ringBuffer->buffer;
		}
		result = 0;
	}
	return result;
}

unsigned int ringBuffer_getItem(RingBuffer *ringBuffer, unsigned char *data) {
	unsigned int result = 0;
	if ((ringBuffer != NULL) && (data != NULL) && (ringBuffer->bufferRead != NULL)) {
		#ifdef RINGBUFFER_USE_DOUBLE_READ
			volatile unsigned char *bufferWriteTemp = ringBuffer->bufferWrite;
			volatile unsigned char *bufferWriteTemp2 = ringBuffer->bufferWrite;
			while (1) {
				bufferWriteTemp = ringBuffer->bufferWrite;
				bufferWriteTemp2 = ringBuffer->bufferWrite;
				if (bufferWriteTemp == bufferWriteTemp2) {
					break;
				}
			}
		
		#else
			volatile unsigned char *bufferWriteTemp = ringBuffer->bufferWrite;	//This must be atomic
		#endif
		if (ringBuffer->bufferRead != bufferWriteTemp) {
			result = 1;
			*data = *ringBuffer->bufferRead;
			*ringBuffer->bufferRead = 0x00;
			ringBuffer->bufferRead++;
			
			#ifdef RINGBUFFER_USE_FILLNESS
				ringBuffer->bufferFillness--;
			#endif

			if (ringBuffer->buffer + ringBuffer->bufferSize == ringBuffer->bufferRead) {
				ringBuffer->bufferRead = ringBuffer->buffer;
			}
		}
	}
	return result;
}

unsigned int ringBuffer_readItem(RingBuffer *ringBuffer, unsigned char *data, unsigned int item) {
	unsigned int result = 0;
	if ((ringBuffer != NULL) && (data != NULL) && (ringBuffer->bufferRead != NULL)) {
		#ifdef RINGBUFFER_USE_DOUBLE_READ
			volatile unsigned char *bufferWriteTemp = ringBuffer->bufferWrite;
			volatile unsigned char *bufferWriteTemp2 = ringBuffer->bufferWrite;
			while (1) {
				bufferWriteTemp = ringBuffer->bufferWrite;
				bufferWriteTemp2 = ringBuffer->bufferWrite;
				if (bufferWriteTemp == bufferWriteTemp2) {
					break;
				}
			}
		
		#else
			volatile unsigned char *bufferWriteTemp = ringBuffer->bufferWrite;	//This must be atomic
		#endif
		if (ringBuffer->bufferRead != bufferWriteTemp) {
			result = 1;
			if ((unsigned int)(ringBuffer->bufferRead - ringBuffer->buffer + item) > (unsigned int)ringBuffer->bufferSize) {
				*data = *((ringBuffer->bufferRead + item - ringBuffer->bufferSize) );
			} else {
				*data = *((ringBuffer->bufferRead + item) );
			}
		}
	}
	return result;
}

#ifdef RINGBUFFER_USE_BUFFER_INTERFACE

int ringBuffer_addBuffer(RingBuffer *ringBuffer, unsigned char *data, unsigned int len) {
	int result = -1;
	unsigned int i = 0;
	for (i = 0; i < len; i++) {
		result = ringBuffer_addItem(ringBuffer, data[i]);
	}
	return result;
}

unsigned int ringBuffer_getBuffer(RingBuffer *ringBuffer, unsigned char *data, unsigned int len) {
	unsigned int result = 0;
	unsigned int i = 0;
	for (i = 0; i < len; i++) {
		result = ringBuffer_getItem(ringBuffer, data + i);
	}
	return result;
}

#endif
