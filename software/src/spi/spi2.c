#include <stdio.h>
#include <string.h>
#include "spi2.h"
#include "c_spi2.h"

#include "k_stdtype.h"
#include "mal.h"
#include "c_main.h"

SPI2_StateEnum spi2StateEnum = SPI2_READY_STATE;
unsigned char *spi2bufferTxTemp = NULL;
unsigned char *spi2bufferRxTemp = NULL;
unsigned int spi2_sizeTemp = 0;
uint8 spi2_default_output = 0;
unsigned char spi2_user = 0;
unsigned char spi2_mode_32_16_8 = 8;

void init_spi2(void) {
	SPI2_SDO_TRIS = 0;
	SPI2_PORT_SYNC();
	#ifdef SPI2_SDI_TRIS
		SPI2_SDI_TRIS = 1;
		SPI2_PORT_SYNC();
	#endif
	SPI2_SCK_TRIS = 0;
	SPI2_PORT_SYNC();
	
	#ifdef __32MZ2048ECG144__
		IEC4CLR = _IEC4_SPI2EIE_MASK | _IEC4_SPI2RXIE_MASK | _IEC4_SPI2TXIE_MASK;
	#else
		IEC1CLR = _IEC1_SPI2EIE_MASK | _IEC1_SPI2RXIE_MASK | _IEC1_SPI2TXIE_MASK;
	#endif

	SPI2CON = 0;
	{
		//Errata: Read buffer only if bit is set
		if (SPI2STATbits.SPIRBF) {
			volatile unsigned char tempRead = SPI2BUF;
			tempRead;
		}
	}
	#ifdef __32MZ2048ECG144__
		IFS4CLR = _IFS4_SPI2EIF_MASK | _IFS4_SPI2RXIF_MASK | _IFS4_SPI2TXIF_MASK;
	#else
		IFS1CLR = _IEC1_SPI2EIE_MASK | _IEC1_SPI2RXIE_MASK | _IEC1_SPI2TXIE_MASK;
	#endif


	#if defined(__32MX470F512H__)
		IPC8bits.SPI2IP = 3;
		IPC8bits.SPI2IS = 3;
	#elif defined(__32MX470F512L__)
		IPC8bits.SPI2IP = 3;
		IPC8bits.SPI2IS = 3;
	#elif defined(__32MZ2048ECG144__)
		IPC35bits.SPI2EIP = 3;
		IPC35bits.SPI2EIS = 3;
		IPC35bits.SPI2RXIP = 3;
		IPC35bits.SPI2RXIS = 3;
		IPC36bits.SPI2TXIP = 3;
		IPC36bits.SPI2TXIS = 3;
	#else
		IPC7bits.SPI2IP = 3;
		IPC7bits.SPI2IS = 3;
	#endif
	
	SPI2BRG = SPI2_BRG_CONFIG_DEFAULT;
	SPI2STATbits.SPIROV = 0;
	SPI2CONbits.CKP	= SPI2_CKP_CONFIG_DEFAULT;
	SPI2CONbits.CKE	= SPI2_CKE_CONFIG_DEFAULT;

	SPI2CONbits.MSTEN = 1;
	SPI2CONbits.ON = 1;
	SPI2_PORT_SYNC();
}

void deinit_spi2(void) {
	SPI2_SDO_TRIS = 1;
	SPI2_PORT_SYNC();
	#ifdef SPI2_SDI_TRIS
		SPI2_SDI_TRIS = 1;
		SPI2_PORT_SYNC();
	#endif
	SPI2_SCK_TRIS = 1;
	SPI2_PORT_SYNC();
	
	#ifdef __32MZ2048ECG144__
		IEC4CLR = _IEC4_SPI2EIE_MASK | _IEC4_SPI2RXIE_MASK | _IEC4_SPI2TXIE_MASK;
	#else
		IEC1CLR = _IEC1_SPI2EIE_MASK | _IEC1_SPI2RXIE_MASK | _IEC1_SPI2TXIE_MASK;
	#endif
	SPI2CON = 0;
}

void do_spi2(void) {
	if (spi2StateEnum == SPI2_BUSY_STATE) {
		unsigned int j = 0;

		for (j = 0; j < spi2_sizeTemp; j++) {
			volatile unsigned int currentBufferOut = spi2_default_output;
			volatile unsigned int currentBufferIn = 0;
			if (spi2bufferTxTemp != NULL) {
				if (spi2_mode_32_16_8 == 32) {
					currentBufferOut = spi2bufferTxTemp[(j * 4)];
					currentBufferOut <<= 8;
					currentBufferOut += spi2bufferTxTemp[(j * 4) + 1];
					currentBufferOut <<= 8;
					currentBufferOut += spi2bufferTxTemp[(j * 4) + 2];
					currentBufferOut <<= 8;
					currentBufferOut += spi2bufferTxTemp[(j * 4) + 3];
				} else if (spi2_mode_32_16_8 == 16) {
					currentBufferOut = spi2bufferTxTemp[(j * 2)];
					currentBufferOut <<= 8;
					currentBufferOut += spi2bufferTxTemp[(j * 2) + 1];
				} else {
					currentBufferOut = spi2bufferTxTemp[j];
				}
			}

			lock_isr();//Errata
			SPI2BUF = currentBufferOut;
			unlock_isr();
			while (1) {
				if ((SPI2STATbits.SPITBE) && (SPI2STATbits.SPIRBF)) {
					break;
				}
			}
			#ifdef __32MZ2048ECG144__
				IFS4CLR = _IFS4_SPI2EIF_MASK | _IFS4_SPI2TXIF_MASK;
				IFS5CLR = _IFS5_SPI4RXIF_MASK;
			#elif defined(__32MX795F512H__)
				IFS1CLR = _IFS1_SPI2EIF_MASK | _IFS1_SPI2RXIF_MASK | _IFS1_SPI2TXIF_MASK;
			#else
				IFS1CLR = _IFS1_SPI2EIF_MASK | _IFS1_SPI2RXIF_MASK | _IFS1_SPI2TXIF_MASK;
			#endif
		
			//Errata: Read buffer only if bit is set
			if (SPI2STATbits.SPIRBF) {
				currentBufferIn = SPI2BUF;
			} else {
			}

			if (spi2bufferRxTemp != NULL) {
				if (spi2_mode_32_16_8 == 32) {
					spi2bufferRxTemp[(j * 4) + 3] = (currentBufferIn >> 24) & 0xFF;
					spi2bufferRxTemp[(j * 4) + 2] = (currentBufferIn >> 16) & 0xFF;
					spi2bufferRxTemp[(j * 4) + 1] = (currentBufferIn >> 8) & 0xFF;
					spi2bufferRxTemp[(j * 4) + 0] = (currentBufferIn >> 0) & 0xFF;
				} else if (spi2_mode_32_16_8 == 16) {
					spi2bufferRxTemp[(j * 2) + 1] = (currentBufferIn >> 8) & 0xFF;
					spi2bufferRxTemp[(j * 2) + 0] = (currentBufferIn >> 0) & 0xFF;
				} else {
					spi2bufferRxTemp[j] = currentBufferIn;
				}
			}
		}
		spi2bufferTxTemp = NULL;
		spi2bufferRxTemp = NULL;
		spi2_sizeTemp = 0;
		spi2StateEnum = SPI2_READY_STATE;
	}
}

void isr_spi2_1ms(void) {
}

void isr_spi2(void) {
}

void spi2_reconfigure(unsigned char user, uint8 SMP, uint8 CKP, uint8 CKE, uint32 BRG) {
	if (user == spi2_user) {
		SPI2CONbits.ON = 0;
		SPI2_PORT_SYNC();
		SPI2BRG = BRG;
		SPI2CONbits.SMP	= SMP;
		SPI2CONbits.CKP	= CKP;
		SPI2CONbits.CKE	= CKE;
		SPI2CONbits.ON = 1;
		SPI2_PORT_SYNC();
	} else {
		SPI2_LOCK_COLLISION_CALLOUT();
	}
}

void spi2_mode8(unsigned char user) {
	if (user == spi2_user) {
		SPI2CONbits.MODE16 = 0;
		SPI2CONbits.MODE32 = 0;
		SPI2_PORT_SYNC();
		spi2_mode_32_16_8 = 8;
	} else {
		SPI2_LOCK_COLLISION_CALLOUT();
	}
}

void spi2_mode16(unsigned char user) {
	if (user == spi2_user) {
		SPI2CONbits.MODE16 = 1;
		SPI2CONbits.MODE32 = 0;
		SPI2_PORT_SYNC();
		spi2_mode_32_16_8 = 16;
	} else {
		SPI2_LOCK_COLLISION_CALLOUT();
	}
}

void spi2_mode32(unsigned char user) {
	if (user == spi2_user) {
		SPI2CONbits.MODE16 = 0;
		SPI2CONbits.MODE32 = 1;
		SPI2_PORT_SYNC();
		spi2_mode_32_16_8 = 32;
	} else {
		SPI2_LOCK_COLLISION_CALLOUT();
	}
}

SPI2_StateEnum spi2_readWrite_synch(unsigned char user, unsigned char *bufferOut, unsigned char *bufferIn, uint32 size) {
	SPI2_StateEnum result = SPI2_ERROR_STATE;
	if (spi2StateEnum == SPI2_READY_STATE) {
		if (spi2_readWrite_asynch(user, bufferOut, bufferIn, size) == SPI2_BUSY_STATE) {
			while (1) {
				do_spi2();
				result = spi2_get_state();
				if (result != SPI2_BUSY_STATE) {
					break;
				}
			}
		}
	}
	return result;
}

SPI2_StateEnum spi2_readWrite_asynch(unsigned char user, unsigned char *bufferOut, unsigned char *bufferIn, uint32 size) {
	SPI2_StateEnum result = SPI2_ERROR_STATE;
	if (user == spi2_user) {
		if (spi2StateEnum == SPI2_READY_STATE) {
			spi2bufferTxTemp = bufferOut;
			spi2bufferRxTemp = bufferIn;
			spi2_sizeTemp = size;
			spi2StateEnum = SPI2_BUSY_STATE;
			result = SPI2_BUSY_STATE;
		}
	} else {
		SPI2_LOCK_COLLISION_CALLOUT();
	}
	return result;
}

uint16 spi2_calculate_BRG(uint32 spi_clk) {
	#ifdef __32MZ2048ECG144__
		uint32 pb_clk = GetPeripheral2Clock();
	#else
		uint32 pb_clk = GetPeripheralClock();
	#endif
	uint32 brg = 0;

	brg = pb_clk / (2 * spi_clk);

	if(pb_clk % (2 * spi_clk)) {
		brg++;
	}

	if(brg > 0x100) {
		brg = 0x100;
	}

	if(brg) {
		brg--;
	}

	return (uint16) brg;
}

SPI2_StateEnum spi2_get_state(void) {
	return spi2StateEnum;
}

uint8 spi2_islocked(void) {
	uint8 result = 0;
	result = spi2_user;
	return result;
}

uint8 spi2_lock(unsigned char user) {
	uint8 result = 0;
	if (spi2_user == 0) {
		spi2_user = user;
	} else {
		if (spi2_user != user) {
			SPI2_LOCK_COLLISION_CALLOUT();
		}
	}
	result = spi2_user;
	return result;
}

uint8 spi2_unlock(unsigned char user) {
	uint8 result = 0;
	if (spi2_user == user) {
		spi2_user = 0;
	} else {
		SPI2_LOCK_COLLISION_CALLOUT();
	}
	result = spi2_user;
	return result;
}
