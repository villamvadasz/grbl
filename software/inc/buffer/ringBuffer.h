#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

	typedef struct _RingBuffer {
		volatile unsigned char *buffer;
		volatile unsigned char *bufferRead;
		volatile unsigned char *bufferWrite;
		volatile unsigned int bufferSize;
		volatile unsigned int bufferMaximumFillness;
		volatile unsigned int bufferFillness;
	} RingBuffer;

	extern int ringBuffer_initBuffer(RingBuffer *ringBuffer, unsigned char *buffer, unsigned int bufferSize);

	extern unsigned int ringBuffer_getFillness(RingBuffer *ringBuffer);
	extern unsigned int ringBuffer_getEmptyness(RingBuffer *ringBuffer);
	extern int ringBuffer_addItem(RingBuffer *ringBuffer, unsigned char data);

	extern unsigned int ringBuffer_getItem(RingBuffer *ringBuffer, unsigned char *data);
	extern unsigned int ringBuffer_readItem(RingBuffer *ringBuffer, unsigned char *data, unsigned int item);

	#ifdef RINGBUFFER_USE_BUFFER_INTERFACE
		extern int ringBuffer_addBuffer(RingBuffer *ringBuffer, unsigned char *data, unsigned int len);
		extern unsigned int ringBuffer_getBuffer(RingBuffer *ringBuffer, unsigned char *data, unsigned int len);
	#endif

#endif
