#include "isr.h"
#include "c_isr.h"

#include "mal.h"
#ifdef ISR_USE_UART1_SIO
	#include "sio.h"
#endif
#ifdef ISR_USE_TMR4_CUSTOM
	#include "main.h"
#endif
#ifdef ISR_USE_USB
	#include "usb.h"
#endif

#if (ISR_IPLV_TMR1 == 0)
	#define ISR_IPL_TMR1 IPL0AUTO
#elif (ISR_IPLV_TMR1 == 1)
	#define ISR_IPL_TMR1 IPL1AUTO
#elif (ISR_IPLV_TMR1 == 2)
	#define ISR_IPL_TMR1 IPL2AUTO
#elif (ISR_IPLV_TMR1 == 3)
	#define ISR_IPL_TMR1 IPL3AUTO
#elif (ISR_IPLV_TMR1 == 4)
	#define ISR_IPL_TMR1 IPL4AUTO
#elif (ISR_IPLV_TMR1 == 5)
	#define ISR_IPL_TMR1 IPL5AUTO
#elif (ISR_IPLV_TMR1 == 6)
	#define ISR_IPL_TMR1 IPL6AUTO
#elif (ISR_IPLV_TMR1 == 7)
	#define ISR_IPL_TMR1 IPL7AUTO
#else
	#error Wrong IPL configured
#endif

#if (ISR_IPLV_TMR2 == 0)
	#define ISR_IPL_TMR2 IPL0AUTO
#elif (ISR_IPLV_TMR2 == 1)
	#define ISR_IPL_TMR2 IPL1AUTO
#elif (ISR_IPLV_TMR2 == 2)
	#define ISR_IPL_TMR2 IPL2AUTO
#elif (ISR_IPLV_TMR2 == 3)
	#define ISR_IPL_TMR2 IPL3AUTO
#elif (ISR_IPLV_TMR2 == 4)
	#define ISR_IPL_TMR2 IPL4AUTO
#elif (ISR_IPLV_TMR2 == 5)
	#define ISR_IPL_TMR2 IPL5AUTO
#elif (ISR_IPLV_TMR2 == 6)
	#define ISR_IPL_TMR2 IPL6AUTO
#else
	#error Wrong IPL configured
#endif

#if (ISR_IPLV_TMR3 == 0)
	#define ISR_IPL_TMR3 IPL0AUTO
#elif (ISR_IPLV_TMR3 == 1)
	#define ISR_IPL_TMR3 IPL1AUTO
#elif (ISR_IPLV_TMR3 == 2)
	#define ISR_IPL_TMR3 IPL2AUTO
#elif (ISR_IPLV_TMR3 == 3)
	#define ISR_IPL_TMR3 IPL3AUTO
#elif (ISR_IPLV_TMR3 == 4)
	#define ISR_IPL_TMR3 IPL4AUTO
#elif (ISR_IPLV_TMR3 == 5)
	#define ISR_IPL_TMR3 IPL5AUTO
#elif (ISR_IPLV_TMR3 == 6)
	#define ISR_IPL_TMR3 IPL6AUTO
#else
	#error Wrong IPL configured
#endif

#if (ISR_IPLV_TMR4 == 0)
	#define ISR_IPL_TMR4 IPL0AUTO
#elif (ISR_IPLV_TMR4 == 1)
	#define ISR_IPL_TMR4 IPL1AUTO
#elif (ISR_IPLV_TMR4 == 2)
	#define ISR_IPL_TMR4 IPL2AUTO
#elif (ISR_IPLV_TMR4 == 3)
	#define ISR_IPL_TMR4 IPL3AUTO
#elif (ISR_IPLV_TMR4 == 4)
	#define ISR_IPL_TMR4 IPL4AUTO
#elif (ISR_IPLV_TMR4 == 5)
	#define ISR_IPL_TMR4 IPL5AUTO
#elif (ISR_IPLV_TMR4 == 6)
	#define ISR_IPL_TMR4 IPL6AUTO
#else
	#error Wrong IPL configured
#endif

#if (ISR_IPLV_CN == 0)
	#define ISR_IPL_CN IPL0AUTO
#elif (ISR_IPLV_CN == 1)
	#define ISR_IPL_CN IPL1AUTO
#elif (ISR_IPLV_CN == 2)
	#define ISR_IPL_CN IPL2AUTO
#elif (ISR_IPLV_CN == 3)
	#define ISR_IPL_CN IPL3AUTO
#elif (ISR_IPLV_CN == 4)
	#define ISR_IPL_CN IPL4AUTO
#elif (ISR_IPLV_CN == 5)
	#define ISR_IPL_CN IPL5AUTO
#elif (ISR_IPLV_CN == 6)
	#define ISR_IPL_CN IPL6AUTO
#else
	#error Wrong IPL configured
#endif

#if (ISR_IPLV_USB == 0)
	#define ISR_IPL_USB IPL0AUTO
#elif (ISR_IPLV_USB == 1)
	#define ISR_IPL_USB IPL1AUTO
#elif (ISR_IPLV_USB == 2)
	#define ISR_IPL_USB IPL2AUTO
#elif (ISR_IPLV_USB == 3)
	#define ISR_IPL_USB IPL3AUTO
#elif (ISR_IPLV_USB == 4)
	#define ISR_IPL_USB IPL4AUTO
#elif (ISR_IPLV_USB == 5)
	#define ISR_IPL_USB IPL5AUTO
#elif (ISR_IPLV_USB == 6)
	#define ISR_IPL_USB IPL6AUTO
#elif (ISR_IPLV_USB == 7)
	#define ISR_IPL_USB IPL7AUTO
#else
	#error Wrong IPL configured
#endif

#if (ISR_IPLV_I2C1_MASTER == 0)
	#define ISR_IPL_I2C1_MASTER IPL0AUTO
#elif (ISR_IPLV_I2C1_MASTER == 1)
	#define ISR_IPL_I2C1_MASTER IPL1AUTO
#elif (ISR_IPLV_I2C1_MASTER == 2)
	#define ISR_IPL_I2C1_MASTER IPL2AUTO
#elif (ISR_IPLV_I2C1_MASTER == 3)
	#define ISR_IPL_I2C1_MASTER IPL3AUTO
#elif (ISR_IPLV_I2C1_MASTER == 4)
	#define ISR_IPL_I2C1_MASTER IPL4AUTO
#elif (ISR_IPLV_I2C1_MASTER == 5)
	#define ISR_IPL_I2C1_MASTER IPL5AUTO
#elif (ISR_IPLV_I2C1_MASTER == 6)
	#define ISR_IPL_I2C1_MASTER IPL6AUTO
#else
	#error Wrong IPL configured
#endif

#if (ISR_IPLV_I2C2_MASTER == 0)
	#define ISR_IPL_I2C2_MASTER IPL0AUTO
#elif (ISR_IPLV_I2C2_MASTER == 1)
	#define ISR_IPL_I2C2_MASTER IPL1AUTO
#elif (ISR_IPLV_I2C2_MASTER == 2)
	#define ISR_IPL_I2C2_MASTER IPL2AUTO
#elif (ISR_IPLV_I2C2_MASTER == 3)
	#define ISR_IPL_I2C2_MASTER IPL3AUTO
#elif (ISR_IPLV_I2C2_MASTER == 4)
	#define ISR_IPL_I2C2_MASTER IPL4AUTO
#elif (ISR_IPLV_I2C2_MASTER == 5)
	#define ISR_IPL_I2C2_MASTER IPL5AUTO
#elif (ISR_IPLV_I2C2_MASTER == 6)
	#define ISR_IPL_I2C2_MASTER IPL6AUTO
#else
	#error Wrong IPL configured
#endif

#if defined (__32MX440F256H__) || defined (__32MX440F512H__) || defined (__32MX470F512H__) || defined (__32MX460F512L__)  || defined (__32MX470F512L__)  || defined (__32MX795F512H__)
#else
	#error TODO Implement
#endif

#ifdef __32MX440F256H__
	#define IFSx_U1 IFS0
	#define IECx_U1 IEC0
	#define IFSx_U1CLR	IFS0CLR
	#define _IFSx_U1_U1EIF_MASK _IFS0_U1EIF_MASK
	#define _IECx_U1_U1EIE_MASK _IEC0_U1EIE_MASK
	#define _IFSx_U1_U1RXIF_MASK _IFS0_U1RXIF_MASK
	#define _IECx_U1_U1RXIE_MASK _IEC0_U1RXIE_MASK
	#define _IFSx_U1_U1TXIF_MASK _IFS0_U1TXIF_MASK
	#define _IECx_U1_U1TXIE_MASK _IEC0_U1TXIE_MASK
	#define _IFSx_U1_U1EIF_POSITION _IFS0_U1EIF_POSITION
	#define _IFSx_U1_U1RXIF_POSITION _IFS0_U1RXIF_POSITION
#endif
#ifdef __32MX440F512H__
	#define IFSx_U1 IFS0
	#define IECx_U1 IEC0
	#define IFSx_U1CLR	IFS0CLR
	#define _IFSx_U1_U1EIF_MASK _IFS0_U1EIF_MASK
	#define _IECx_U1_U1EIE_MASK _IEC0_U1EIE_MASK
	#define _IFSx_U1_U1RXIF_MASK _IFS0_U1RXIF_MASK
	#define _IECx_U1_U1RXIE_MASK _IEC0_U1RXIE_MASK
	#define _IFSx_U1_U1TXIF_MASK _IFS0_U1TXIF_MASK
	#define _IECx_U1_U1TXIE_MASK _IEC0_U1TXIE_MASK
	#define _IFSx_U1_U1EIF_POSITION _IFS0_U1EIF_POSITION
	#define _IFSx_U1_U1RXIF_POSITION _IFS0_U1RXIF_POSITION
#endif
#ifdef __32MX470F512H__
	#define IFSx_U1 IFS1
	#define IECx_U1 IEC1
	#define IFSx_U1CLR	IFS1CLR
	#define _IFSx_U1_U1EIF_MASK _IFS1_U1EIF_MASK
	#define _IECx_U1_U1EIE_MASK _IEC1_U1EIE_MASK
	#define _IFSx_U1_U1RXIF_MASK _IFS1_U1RXIF_MASK
	#define _IECx_U1_U1RXIE_MASK _IEC1_U1RXIE_MASK
	#define _IFSx_U1_U1TXIF_MASK _IFS1_U1TXIF_MASK
	#define _IECx_U1_U1TXIE_MASK _IEC1_U1TXIE_MASK
	#define _IFSx_U1_U1EIF_POSITION _IFS1_U1EIF_POSITION
	#define _IFSx_U1_U1RXIF_POSITION _IFS1_U1RXIF_POSITION
#endif
#ifdef __32MX460F512L__
	#define IFSx_U1 IFS0
	#define IECx_U1 IEC0
	#define IFSx_U1CLR	IFS0CLR
	#define _IFSx_U1_U1EIF_MASK _IFS0_U1EIF_MASK
	#define _IECx_U1_U1EIE_MASK _IEC0_U1EIE_MASK
	#define _IFSx_U1_U1RXIF_MASK _IFS0_U1RXIF_MASK
	#define _IECx_U1_U1RXIE_MASK _IEC0_U1RXIE_MASK
	#define _IFSx_U1_U1TXIF_MASK _IFS0_U1TXIF_MASK
	#define _IECx_U1_U1TXIE_MASK _IEC0_U1TXIE_MASK
	#define _IFSx_U1_U1EIF_POSITION _IFS0_U1EIF_POSITION
	#define _IFSx_U1_U1RXIF_POSITION _IFS0_U1RXIF_POSITION
#endif
#ifdef __32MX470F512L__
	#define IFSx_U1 IFS1
	#define IECx_U1 IEC1
	#define IFSx_U1CLR	IFS1CLR
	#define _IFSx_U1_U1EIF_MASK _IFS1_U1EIF_MASK
	#define _IECx_U1_U1EIE_MASK _IEC1_U1EIE_MASK
	#define _IFSx_U1_U1RXIF_MASK _IFS1_U1RXIF_MASK
	#define _IECx_U1_U1RXIE_MASK _IEC1_U1RXIE_MASK
	#define _IFSx_U1_U1TXIF_MASK _IFS1_U1TXIF_MASK
	#define _IECx_U1_U1TXIE_MASK _IEC1_U1TXIE_MASK
	#define _IFSx_U1_U1EIF_POSITION _IFS1_U1EIF_POSITION
	#define _IFSx_U1_U1RXIF_POSITION _IFS1_U1RXIF_POSITION
#endif
#ifdef __32MX795F512H__
	#define IFSx_U1 IFS0
	#define IECx_U1 IEC0
	#define IFSx_U1CLR	IFS0CLR
	#define _IFSx_U1_U1EIF_MASK _IFS0_U1EIF_MASK
	#define _IECx_U1_U1EIE_MASK _IEC0_U1EIE_MASK
	#define _IFSx_U1_U1RXIF_MASK _IFS0_U1RXIF_MASK
	#define _IECx_U1_U1RXIE_MASK _IEC0_U1RXIE_MASK
	#define _IFSx_U1_U1TXIF_MASK _IFS0_U1TXIF_MASK
	#define _IECx_U1_U1TXIE_MASK _IEC0_U1TXIE_MASK
	#define _IFSx_U1_U1EIF_POSITION _IFS0_U1EIF_POSITION
	#define _IFSx_U1_U1RXIF_POSITION _IFS0_U1RXIF_POSITION
#endif

#ifdef __32MX440F256H__
	#define IFSx_U2 IFS1
	#define IECx_U2 IEC1
	#define IFSx_U2CLR	IFS1CLR
	#define _IFSx_U2_U2EIF_MASK _IFS1_U2EIF_MASK
	#define _IECx_U2_U2EIE_MASK _IEC1_U2EIE_MASK
	#define _IFSx_U2_U2RXIF_MASK _IFS1_U2RXIF_MASK
	#define _IECx_U2_U2RXIE_MASK _IEC1_U2RXIE_MASK
	#define _IFSx_U2_U2TXIF_MASK _IFS1_U2TXIF_MASK
	#define _IECx_U2_U2TXIE_MASK _IEC1_U2TXIE_MASK
	#define _IFSx_U2_U2EIF_POSITION _IFS1_U2EIF_POSITION
	#define _IFSx_U2_U2RXIF_POSITION _IFS1_U2RXIF_POSITION
#endif
#ifdef __32MX440F512H__
	#define IFSx_U2 IFS1
	#define IECx_U2 IEC1
	#define IFSx_U2CLR	IFS1CLR
	#define _IFSx_U2_U2EIF_MASK _IFS1_U2EIF_MASK
	#define _IECx_U2_U2EIE_MASK _IEC1_U2EIE_MASK
	#define _IFSx_U2_U2RXIF_MASK _IFS1_U2RXIF_MASK
	#define _IECx_U2_U2RXIE_MASK _IEC1_U2RXIE_MASK
	#define _IFSx_U2_U2TXIF_MASK _IFS1_U2TXIF_MASK
	#define _IECx_U2_U2TXIE_MASK _IEC1_U2TXIE_MASK
	#define _IFSx_U2_U2EIF_POSITION _IFS1_U2EIF_POSITION
	#define _IFSx_U2_U2RXIF_POSITION _IFS1_U2RXIF_POSITION
#endif
#ifdef __32MX470F512H__
	#define IFSx_U2 IFS1
	#define IECx_U2 IEC1
	#define IFSx_U2CLR	IFS1CLR
	#define _IFSx_U2_U2EIF_MASK _IFS1_U2EIF_MASK
	#define _IECx_U2_U2EIE_MASK _IEC1_U2EIE_MASK
	#define _IFSx_U2_U2RXIF_MASK _IFS1_U2RXIF_MASK
	#define _IECx_U2_U2RXIE_MASK _IEC1_U2RXIE_MASK
	#define _IFSx_U2_U2TXIF_MASK _IFS1_U2TXIF_MASK
	#define _IECx_U2_U2TXIE_MASK _IEC1_U2TXIE_MASK
	#define _IFSx_U2_U2EIF_POSITION _IFS1_U2EIF_POSITION
	#define _IFSx_U2_U2RXIF_POSITION _IFS1_U2RXIF_POSITION
#endif
#ifdef __32MX460F512L__
	#define IFSx_U2 IFS1
	#define IECx_U2 IEC1
	#define IFSx_U2CLR	IFS1CLR
	#define _IFSx_U2_U2EIF_MASK _IFS1_U2EIF_MASK
	#define _IECx_U2_U2EIE_MASK _IEC1_U2EIE_MASK
	#define _IFSx_U2_U2RXIF_MASK _IFS1_U2RXIF_MASK
	#define _IECx_U2_U2RXIE_MASK _IEC1_U2RXIE_MASK
	#define _IFSx_U2_U2TXIF_MASK _IFS1_U2TXIF_MASK
	#define _IECx_U2_U2TXIE_MASK _IEC1_U2TXIE_MASK
	#define _IFSx_U2_U2EIF_POSITION _IFS1_U2EIF_POSITION
	#define _IFSx_U2_U2RXIF_POSITION _IFS1_U2RXIF_POSITION
#endif
#ifdef __32MX470F512L__
	#define IFSx_U2 IFS1
	#define IECx_U2 IEC1
	#define IFSx_U2CLR	IFS1CLR
	#define _IFSx_U2_U2EIF_MASK _IFS1_U2EIF_MASK
	#define _IECx_U2_U2EIE_MASK _IEC1_U2EIE_MASK
	#define _IFSx_U2_U2RXIF_MASK _IFS1_U2RXIF_MASK
	#define _IECx_U2_U2RXIE_MASK _IEC1_U2RXIE_MASK
	#define _IFSx_U2_U2TXIF_MASK _IFS1_U2TXIF_MASK
	#define _IECx_U2_U2TXIE_MASK _IEC1_U2TXIE_MASK
	#define _IFSx_U2_U2EIF_POSITION _IFS1_U2EIF_POSITION
	#define _IFSx_U2_U2RXIF_POSITION _IFS1_U2RXIF_POSITION
#endif
#ifdef __32MX795F512H__
	#define IFSx_U2 IFS1
	#define IECx_U2 IEC1
	#define IFSx_U2CLR	IFS1CLR
	#define _IFSx_U2_U2EIF_MASK _IFS1_U2EIF_MASK
	#define _IECx_U2_U2EIE_MASK _IEC1_U2EIE_MASK
	#define _IFSx_U2_U2RXIF_MASK _IFS1_U2RXIF_MASK
	#define _IECx_U2_U2RXIE_MASK _IEC1_U2RXIE_MASK
	#define _IFSx_U2_U2TXIF_MASK _IFS1_U2TXIF_MASK
	#define _IECx_U2_U2TXIE_MASK _IEC1_U2TXIE_MASK
	#define _IFSx_U2_U2EIF_POSITION _IFS1_U2EIF_POSITION
	#define _IFSx_U2_U2RXIF_POSITION _IFS1_U2RXIF_POSITION
#endif

void __ISR(_TIMER_1_VECTOR, ISR_IPL_TMR1) Timer1Handler(void) {
	clear_isr(IFS0CLR, _IFS0_T1IF_POSITION);
	#ifdef ISR_USE_TMR1
		isr_tmr1();
	#endif
} 

void __ISR(_TIMER_2_VECTOR, ISR_IPL_TMR2) Timer2Handler(void) {
	clear_isr(IFS0CLR, _IFS0_T2IF_POSITION);
	#ifdef ISR_USE_TMR2
		isr_tmr2();
	#endif
}

void __ISR(_TIMER_3_VECTOR, ISR_IPL_TMR3) Timer3Handler(void) {
	clear_isr(IFS0CLR, _IFS0_T3IF_POSITION);
	#ifdef ISR_USE_TMR3
		isr_tmr3();
	#endif
	#ifdef ISR_USE_TMR3_GRBL
		isr_grbl_tmr_stepper();
	#endif
}

void __ISR(_TIMER_4_VECTOR, ISR_IPL_TMR4) Timer4Handler(void) {
	clear_isr(IFS0CLR, _IFS0_T4IF_POSITION);
	#ifdef ISR_USE_TMR4
		isr_tmr4();
	#endif
	#ifdef ISR_USE_TMR4_CUSTOM
		isr_main_custom();
	#endif
	#ifdef ISR_USE_TMR4_GRBL
		isr_grbl_tmr_stepper_delay();
	#endif
}

void __ISR(_TIMER_5_VECTOR, IPL2SOFT) Timer5Handler(void) {
	clear_isr(IFS0CLR, _IFS0_T5IF_POSITION);
	#ifdef ISR_USE_TMR5
		isr_tmr5();
	#endif
}

void __ISR(_SPI_2_VECTOR, IPL3SOFT)__SPI2Interrupt(void) {
	#ifdef ISR_USE_SPI2
		ISR_FNC_SPI2();
	#endif
	#ifdef ISR_USE_SPI2_WIFI
		isr_spi_wifi();
	#endif
}

void __ISR(_EXTERNAL_3_VECTOR, IPL3SOFT)__EINT3Interrupt(void) {
	#ifdef ISR_USE_EINT3_WIFI
		isr_eint_wifi();
	#endif
}

void __ISR(_EXTERNAL_4_VECTOR, IPL3SOFT)__EINT4nterrupt(void) {
	#ifdef ISR_USE_EINT4_WIFI
		isr_eint_wifi();
	#endif
	#ifdef ISR_USE_EINT4_DHT11
		#ifdef __32MX440F256H__
	        clear_isr(IFS0CLR, _IFS0_INT4IF_POSITION);
		#else
			#error Not implemented
		#endif
		isr_dht11_external_interrupt();
	#endif
}

void __ISR(_DMA_0_VECTOR, IPL5SOFT) DmaHandler0(void) {
	#ifdef ISR_USE_DMA0
		extern void isr_dma0(void);
		isr_dma0();
	#endif
} 

void __ISR(_DMA_1_VECTOR, IPL5SOFT) DmaHandler1(void) {
	#ifdef ISR_USE_DMA1
		isr_dma1();
	#endif
} 

void __ISR(_DMA_2_VECTOR, IPL6SOFT) DmaHandler2(void) {
	#ifdef ISR_USE_DMA2
		isr_dma2();
	#endif
}

void __ISR(_DMA_3_VECTOR, IPL6SOFT) DmaHandler3(void) {
	#ifdef ISR_USE_DMA3
		isr_dma3();
	#endif
}

void __ISR(_CHANGE_NOTICE_VECTOR, ISR_IPL_CN) ChangeNotification(void) {
	#ifdef ISR_USE_CN
		isr_cn();
		#ifdef __32MX470F512H__
			PORTB;
			PORTC;
			PORTD;
			PORTE;
			PORTF;
			PORTG;
			clear_isr(IFS1CLR, _IFS1_CNAIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNBIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNCIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNDIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNEIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNFIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNGIF_POSITION );
		#else
		#ifdef __32MX470F512L__
			PORTB;
			PORTC;
			PORTD;
			PORTE;
			PORTF;
			PORTG;
			clear_isr(IFS1CLR, _IFS1_CNAIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNBIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNCIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNDIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNEIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNFIF_POSITION );
			clear_isr(IFS1CLR, _IFS1_CNGIF_POSITION );
		#else
			PORTB;
			PORTC;
			PORTD;
			PORTE;
			PORTF;
			PORTG;
			clear_isr(IFS1CLR, _IFS1_CNIF_POSITION);
		#endif
		#endif

	#endif
}

void __ISR(_UART1_VECTOR, IPL2SOFT) IntUart1Handler(void) {
	if ((IFSx_U1 & _IFSx_U1_U1EIF_MASK) && (IECx_U1 & _IECx_U1_U1EIE_MASK)) {
		clear_isr(IFSx_U1CLR, _IFSx_U1_U1EIF_POSITION);
		#ifdef ISR_USE_UART1
			isrSerialError_usart1();
		#endif
	}
	if ((IFSx_U1 & _IFSx_U1_U1RXIF_MASK) && (IECx_U1 & _IECx_U1_U1RXIE_MASK)) {
		#ifdef ISR_USE_UART1
			isrSerialRx_usart1();
		#endif
		#ifdef ISR_USE_UART1_SIO
			isr_communication();
		#endif
		//Must be cleared after byte is read
		clear_isr(IFSx_U1CLR, _IFSx_U1_U1RXIF_POSITION);
	}
	if ((IFSx_U1 & _IFSx_U1_U1TXIF_MASK) && (IECx_U1 & _IECx_U1_U1TXIE_MASK)) {
		//INTClearFlag(INT_SOURCE_UART_TX(UART1)); //must not be cleared
		#ifdef ISR_USE_UART1
			isrSerialTx_usart1();
		#endif
	}
}

void __ISR(_UART2_VECTOR, IPL7SOFT) IntUart2Handler(void) {

	if ((IFSx_U2 & _IFSx_U2_U2EIF_MASK) && (IECx_U2 & _IECx_U2_U2EIE_MASK)) {
		clear_isr(IFSx_U2CLR, _IFSx_U2_U2EIF_POSITION);
		#ifdef ISR_USE_UART2
			isrSerialError_usart2();
		#endif
	}
	if ((IFSx_U2 & _IFSx_U2_U2RXIF_MASK) && (IECx_U2 & _IECx_U2_U2RXIE_MASK)) {
		#ifdef ISR_USE_UART2
			isrSerialRx_usart2();
		#endif
		#ifdef ISR_USE_UART1_SIO
			isr_communication();
		#endif
		//Must be cleared after byte is read
		clear_isr(IFSx_U2CLR, _IFSx_U2_U2RXIF_POSITION);
	}
	if ((IFSx_U2 & _IFSx_U2_U2TXIF_MASK) && (IECx_U2 & _IECx_U2_U2TXIE_MASK)) {
		//INTClearFlag(INT_SOURCE_UART_TX(UART1)); //must not be cleared
		#ifdef ISR_USE_UART2
			isrSerialTx_usart2();
		#endif
	}
}

void __ISR(_USB_1_VECTOR, ISR_IPL_USB) IntUSBHandler(void) {
	#ifdef ISR_USE_USB
		isr_usb();
	#endif
}

void __ISR(_CORE_TIMER_VECTOR, IPL2SOFT) CoreTimerHandler(void) {
	clear_isr(IFS0CLR, _IFS0_CTIF_POSITION);
}

void __ISR(_I2C_1_VECTOR, ISR_IPL_I2C1_MASTER) IntI2C1Handler(void) {
	#pragma GCC diagnostic ignored "-Wunused-variable"
	unsigned char i2c_interrupt_type = 0;


	#ifdef __32MX440F256H__
		if (IFS0bits.I2C1BIF) {
			i2c_interrupt_type |= 1;
			clear_isr(IFS0CLR, _IFS0_I2C1BIF_POSITION);
		}
		if (IFS0bits.I2C1SIF) {
			i2c_interrupt_type |= 2;
			clear_isr(IFS0CLR, _IFS0_I2C1SIF_POSITION);
		}
		if (IFS0bits.I2C1MIF) {
			i2c_interrupt_type |= 4;
			clear_isr(IFS0CLR, _IFS0_I2C1MIF_POSITION);
		}
	#endif
	#ifdef __32MX470F512H__
		if (IFS1bits.I2C1BIF) {
			i2c_interrupt_type |= 1;
			clear_isr(IFS1CLR, _IFS1_I2C1BIF_POSITION);
		}
		if (IFS1bits.I2C1SIF) {
			i2c_interrupt_type |= 2;
			clear_isr(IFS1CLR, _IFS1_I2C1SIF_POSITION);
		}
		if (IFS1bits.I2C1MIF) {
			i2c_interrupt_type |= 4;
			clear_isr(IFS1CLR, _IFS1_I2C1MIF_POSITION);
		}
	#endif
	#ifdef __32MX460F512L__
		if (IFS0bits.I2C1BIF) {
			i2c_interrupt_type |= 1;
			clear_isr(IFS0CLR, _IFS0_I2C1BIF_POSITION);
		}
		if (IFS0bits.I2C1SIF) {
			i2c_interrupt_type |= 2;
			clear_isr(IFS0CLR, _IFS0_I2C1SIF_POSITION);
		}
		if (IFS0bits.I2C1MIF) {
			i2c_interrupt_type |= 4;
			clear_isr(IFS0CLR, _IFS0_I2C1MIF_POSITION);
		}
	#endif
	#ifdef __32MX470F512L__
		if (IFS1bits.I2C1BIF) {
			i2c_interrupt_type |= 1;
			clear_isr(IFS1CLR, _IFS1_I2C1BIF_POSITION);
		}
		if (IFS1bits.I2C1SIF) {
			i2c_interrupt_type |= 2;
			clear_isr(IFS1CLR, _IFS1_I2C1SIF_POSITION);
		}
		if (IFS1bits.I2C1MIF) {
			i2c_interrupt_type |= 4;
			clear_isr(IFS1CLR, _IFS1_I2C1MIF_POSITION);
		}
	#endif

	#ifdef ISR_USE_I2C1_SLAVE
		isr_iic_slave(i2c_interrupt_type);
	#endif
	#ifdef ISR_USE_I2C1_MASTER
		isr_iic_master(i2c_interrupt_type);
	#endif
}

void __ISR(_I2C_2_VECTOR, ISR_IPL_I2C2_MASTER) IntI2C2Handler(void) {
	unsigned char i2c_interrupt_type = 0;
	#ifdef __32MX795F512H__
		if (IFS1bits.I2C2ABIF) {
			i2c_interrupt_type |= 1;
			clear_isr(IFS1CLR, _IFS1_I2C2ABIF_POSITION);
		}
		if (IFS1bits.I2C2ASIF) {
			i2c_interrupt_type |= 2;
			clear_isr(IFS1CLR, _IFS1_I2C2ASIF_POSITION);
		}
		if (IFS1bits.I2C2AMIF) {
			i2c_interrupt_type |= 4;
			clear_isr(IFS1CLR, _IFS1_I2C2AMIF_POSITION);
		}
	#else
		if (IFS1bits.I2C2BIF) {
			i2c_interrupt_type |= 1;
			clear_isr(IFS1CLR, _IFS1_I2C2BIF_POSITION);
		}
		if (IFS1bits.I2C2SIF) {
			i2c_interrupt_type |= 2;
			clear_isr(IFS1CLR, _IFS1_I2C2SIF_POSITION);
		}
		if (IFS1bits.I2C2MIF) {
			i2c_interrupt_type |= 4;
			clear_isr(IFS1CLR, _IFS1_I2C2MIF_POSITION);
		}
	#endif
	#ifdef ISR_USE_I2C2_SLAVE
		isr_iic_slave(i2c_interrupt_type);
	#endif
	#ifdef ISR_USE_I2C2_MASTER
		isr_iic_master(i2c_interrupt_type);
	#endif
}


/*void __attribute__((naked, nomips16,noreturn,unique_section)) _DefaultInterrupt (void) {
	volatile char exitLoop = 0;
	while (!exitLoop) {
		volatile int catcherTemp = 0;
		catcherTemp++;
		catcherTemp++;
		catcherTemp++;
		catcherTemp++;
	}
}*/

void init_isr(void) {
	unsigned int status = 0;
	unsigned int val = 0;
	// set the CP0 cause IV bit high
	asm volatile("mfc0   %0,$13" : "=r"(val));
	val |= 0x00800000;
	asm volatile("mtc0   %0,$13" : "+r"(val));
	INTCONSET = _INTCON_MVEC_MASK;

	asm volatile("ei    %0" : "=r"(status));
	
	#ifdef ISR_USE_CN
		#ifdef __32MX470F512H__
			CNCONBbits.ON = 1;
			CNCONCbits.ON = 1;
			CNCONDbits.ON = 1;
			CNCONEbits.ON = 1;
			CNCONFbits.ON = 1;
			CNCONGbits.ON = 1;
			PORTB;
			PORTC;
			PORTD;
			PORTE;
			PORTF;
			PORTG;
			//CNEN are configured in respective application
			//CNPUE are configured in respective application
			IPC8bits.CNIS = 0;
			IPC8bits.CNIP = ISR_IPLV_CN;
			clear_isr(IFS1CLR, _IFS1_CNAIF_POSITION);
			IEC1bits.CNAIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNBIF_POSITION);
			IEC1bits.CNBIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNCIF_POSITION);
			IEC1bits.CNCIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNDIF_POSITION);
			IEC1bits.CNDIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNEIF_POSITION);
			IEC1bits.CNEIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNFIF_POSITION);
			IEC1bits.CNFIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNGIF_POSITION);
			IEC1bits.CNGIE = 1;
		#else
		#ifdef __32MX470F512L__
			CNCONBbits.ON = 1;
			CNCONCbits.ON = 1;
			CNCONDbits.ON = 1;
			CNCONEbits.ON = 1;
			CNCONFbits.ON = 1;
			CNCONGbits.ON = 1;
			PORTB;
			PORTC;
			PORTD;
			PORTE;
			PORTF;
			PORTG;
			//CNEN are configured in respective application
			//CNPUE are configured in respective application
			IPC8bits.CNIS = 0;
			IPC8bits.CNIP = ISR_IPLV_CN;
			clear_isr(IFS1CLR, _IFS1_CNAIF_POSITION);
			IEC1bits.CNAIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNBIF_POSITION);
			IEC1bits.CNBIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNCIF_POSITION);
			IEC1bits.CNCIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNDIF_POSITION);
			IEC1bits.CNDIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNEIF_POSITION);
			IEC1bits.CNEIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNFIF_POSITION);
			IEC1bits.CNFIE = 1;
			clear_isr(IFS1CLR, _IFS1_CNGIF_POSITION);
			IEC1bits.CNGIE = 1;
		#else
			CNCONbits.ON = 1;
			PORTB;
			PORTC;
			PORTD;
			PORTE;
			PORTF;
			PORTG;
			//CNEN are configured in respective application
			//CNPUE are configured in respective application
			IPC6bits.CNIS = 0;
			IPC6bits.CNIP = ISR_IPLV_CN;
			clear_isr(IFS1CLR, _IFS1_CNIF_POSITION);
			IEC1bits.CNIE = 1;
		#endif
		#endif
	#endif

	#ifdef ISR_USE_EINT4_DHT11
		IPC4bits.INT4IP = 3;
		IPC4bits.INT4IS = 0;

		//Do not enable here. It will be managed by the driver.
		//INTCONbits.INT4EP = 0; //falling edge
		//IFS0bits.INT4IF = 0;
		//IEC0bits.INT4IE = 1;
	#endif
}

void deinit_isr(void) {
	unsigned int status = 0;


	#ifdef ISR_USE_CN
		#ifdef __32MX470F512H__
			CNCONBbits.ON = 0;
			CNCONCbits.ON = 0;
			CNCONDbits.ON = 0;
			CNCONEbits.ON = 0;
			CNCONFbits.ON = 0;
			CNCONGbits.ON = 0;
			PORTB;
			PORTC;
			PORTD;
			PORTE;
			PORTF;
			PORTG;
			//CNEN are configured in respective application
			//CNPUE are configured in respective application
			IPC8bits.CNIS = 0;
			IPC8bits.CNIP = ISR_IPLV_CN;
			clear_isr(IFS1CLR, _IFS1_CNAIF_POSITION);
			IEC1bits.CNAIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNBIF_POSITION);
			IEC1bits.CNBIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNCIF_POSITION);
			IEC1bits.CNCIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNDIF_POSITION);
			IEC1bits.CNDIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNEIF_POSITION);
			IEC1bits.CNEIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNFIF_POSITION);
			IEC1bits.CNFIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNGIF_POSITION);
			IEC1bits.CNGIE = 0;
		#else
		#ifdef __32MX470F512L__
			CNCONBbits.ON = 0;
			CNCONCbits.ON = 0;
			CNCONDbits.ON = 0;
			CNCONEbits.ON = 0;
			CNCONFbits.ON = 0;
			CNCONGbits.ON = 0;
			PORTB;
			PORTC;
			PORTD;
			PORTE;
			PORTF;
			PORTG;
			//CNEN are configured in respective application
			//CNPUE are configured in respective application
			IPC8bits.CNIS = 0;
			IPC8bits.CNIP = ISR_IPLV_CN;
			clear_isr(IFS1CLR, _IFS1_CNAIF_POSITION);
			IEC1bits.CNAIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNBIF_POSITION);
			IEC1bits.CNBIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNCIF_POSITION);
			IEC1bits.CNCIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNDIF_POSITION);
			IEC1bits.CNDIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNEIF_POSITION);
			IEC1bits.CNEIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNFIF_POSITION);
			IEC1bits.CNFIE = 0;
			clear_isr(IFS1CLR, _IFS1_CNGIF_POSITION);
			IEC1bits.CNGIE = 0;
		#else
			CNCONbits.ON = 0;
			PORTB;
			PORTC;
			PORTD;
			PORTE;
			PORTF;
			PORTG;
			//CNEN are configured in respective application
			//CNPUE are configured in respective application
			IPC6bits.CNIS = 0;
			IPC6bits.CNIP = ISR_IPLV_CN;
			clear_isr(IFS1CLR, _IFS1_CNIF_POSITION);
			IEC1bits.CNIE = 0;
		#endif
		#endif
	#endif

	#ifdef ISR_USE_EINT4_DHT11
		IPC4bits.INT4IP = 3;
		IPC4bits.INT4IS = 0;

		//Do not enable here. It will be managed by the driver.
		//INTCONbits.INT4EP = 0; //falling edge
		//IFS0bits.INT4IF = 0;
		//IEC0bits.INT4IE = 1;
	#endif

	asm volatile("di    %0" : "=r"(status));

}
