#ifndef _SPI2_H_
#define _SPI2_H_

	#include "k_stdtype.h"

	typedef enum _SPI2_StateEnum {
		SPI2_READY_STATE,
		SPI2_BUSY_STATE,
		SPI2_ERROR_STATE
	} SPI2_StateEnum;

	extern uint8 spi2_islocked(void);
	extern uint8 spi2_lock(unsigned char user);
	extern uint8 spi2_unlock(unsigned char user);

	extern void spi2_reconfigure(unsigned char user, uint8 SMP, uint8 CKP, uint8 CKE, uint32 BRG);
	extern void spi2_mode8(unsigned char user);
	extern void spi2_mode16(unsigned char user);
	extern void spi2_mode32(unsigned char user);
		
	extern SPI2_StateEnum spi2_readWrite_asynch(unsigned char user, unsigned char *bufferOut, unsigned char *bufferIn, uint32 size);
	extern SPI2_StateEnum spi2_get_state(void);

	extern SPI2_StateEnum spi2_readWrite_synch(unsigned char user, unsigned char *bufferOut, unsigned char *bufferIn, uint32 size);

	extern uint16 spi2_calculate_BRG(uint32 spi_clk);

	extern void init_spi2(void);
	extern void deinit_spi2(void);
	extern void do_spi2(void);
	extern void isr_spi2_1ms(void);
	extern void isr_spi2_hw(void); //only required if used
	extern void isr_dma(void); //only required if used

#endif
