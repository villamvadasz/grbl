#ifndef _GRBL_SERIAL_H_
#define _GRBL_SERIAL_H_

	#define GRBL_RX_BUFFER_SIZE	512

	extern void init_serial_grbl(void);
	extern void do_serial_grbl(void);
	extern void isr_serial_grbl_1ms(void);

	extern int isCharInBuffer_grbl(void);
	extern int getChar_grbl(void);
	extern void putChar_grbl(unsigned char data);

#endif
