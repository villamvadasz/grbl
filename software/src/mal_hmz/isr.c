#include "isr.h"
#include "c_isr.h"

#include "mal.h"

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

#if (ISR_IPLV_USB_DMA == 0)
	#define ISR_IPL_USB_DMA IPL0AUTO
#elif (ISR_IPLV_USB_DMA == 1)
	#define ISR_IPL_USB_DMA IPL1AUTO
#elif (ISR_IPLV_USB_DMA == 2)
	#define ISR_IPL_USB_DMA IPL2AUTO
#elif (ISR_IPLV_USB_DMA == 3)
	#define ISR_IPL_USB_DMA IPL3AUTO
#elif (ISR_IPLV_USB_DMA == 4)
	#define ISR_IPL_USB_DMA IPL4AUTO
#elif (ISR_IPLV_USB_DMA == 5)
	#define ISR_IPL_USB_DMA IPL5AUTO
#elif (ISR_IPLV_USB_DMA == 6)
	#define ISR_IPL_USB_DMA IPL6AUTO
#else
	#error Wrong IPL configured
#endif

void __ISR(_TIMER_1_VECTOR, ISR_IPL_TMR1) Timer1Handler(void) {
	IFS0bits.T1IF = 0;
	#ifdef ISR_USE_TMR1
		isr_tmr1();
	#endif
} 

void __ISR(_TIMER_2_VECTOR, ISR_IPL_TMR2) Timer2Handler(void) {
	IFS0bits.T2IF = 0;
	#ifdef ISR_USE_TMR2
		isr_tmr2();
	#endif
}

void __ISR(_TIMER_3_VECTOR, ISR_IPL_TMR3) Timer3Handler(void) {
	IFS0bits.T3IF = 0;
	#ifdef ISR_USE_TMR3
		isr_tmr3();
	#endif
	#ifdef ISR_USE_TMR3_GRBL
		isr_grbl_tmr_stepper();
	#endif
}

void __ISR(_TIMER_4_VECTOR, ISR_IPL_TMR4) Timer4Handler(void) {
	IFS0bits.T4IF = 0;
	#ifdef ISR_USE_TMR4
		isr_tmr4();
	#endif
	#ifdef ISR_USE_TMR4_CUSTOM
		isr_appCustom();
	#endif
	#ifdef ISR_USE_TMR4_GRBL
		isr_grbl_tmr_stepper_delay();
	#endif
}

void __ISR(_SPI2_FAULT_VECTOR, IPL3AUTO)__SPI2FAULTInterrupt(void) {
	#ifdef ISR_USE_SPI2
	#endif
}

void __ISR(_SPI2_RX_VECTOR, IPL3AUTO)__SPI2RXInterrupt(void) {
	#ifdef ISR_USE_SPI2
	#endif
}

void __ISR(_SPI2_TX_VECTOR, IPL3AUTO)__SPI2TXInterrupt(void) {
	#ifdef ISR_USE_SPI2
	#endif
}

void __ISR(_SPI4_FAULT_VECTOR, IPL3AUTO)__SPI4FAULTInterrupt(void) {
	#ifdef ISR_USE_SPI4
	#endif
}

void __ISR(_SPI4_RX_VECTOR, IPL3AUTO)__SPI4RXInterrupt(void) {
	#ifdef ISR_USE_SPI4
	#endif
}

void __ISR(_SPI4_TX_VECTOR, IPL3AUTO)__SPI4TXInterrupt(void) {
	#ifdef ISR_USE_SPI4
	#endif
}

void __ISR(_EXTERNAL_3_VECTOR, IPL3AUTO)__EINT3Interrupt(void) {
	#ifdef ISR_USE_EINT3_WIFI
		isr_eint_wifi();
	#endif
}

void __ISR(_DMA0_VECTOR, IPL5AUTO) DmaHandler0(void) {
	#ifdef ISR_USE_DMA0
		isr_dma0();
	#endif
} 

void __ISR(_DMA1_VECTOR, IPL5AUTO) DmaHandler1(void) {
	#ifdef ISR_USE_DMA1
		isr_dma1();
	#endif
} 

void __ISR(_DMA2_VECTOR, IPL6AUTO) DmaHandler2(void) {
	#ifdef ISR_USE_DMA2
		isr_dma2();
	#endif
}
#ifdef _CHANGE_NOTICE_A_VECTOR
	void __ISR(_CHANGE_NOTICE_A_VECTOR, ISR_IPL_CN) ChangeNotificationA(void) {
		#ifdef ISR_USE_CN
			PORTA;
			isr_cn();
			IFS3bits.CNAIF = 0;
		#endif
	}
#endif

void __ISR(_CHANGE_NOTICE_B_VECTOR, ISR_IPL_CN) ChangeNotificationB(void) {
	#ifdef ISR_USE_CN
		PORTB;
		isr_cn();
		IFS3bits.CNBIF = 0;
	#endif
}

void __ISR(_CHANGE_NOTICE_C_VECTOR, ISR_IPL_CN) ChangeNotificationC(void) {
	#ifdef ISR_USE_CN
		PORTC;
		isr_cn();
		IFS3bits.CNCIF = 0;
	#endif
}

void __ISR(_CHANGE_NOTICE_D_VECTOR, ISR_IPL_CN) ChangeNotificationD(void) {
	#ifdef ISR_USE_CN
		PORTD;
		isr_cn();
		IFS3bits.CNDIF = 0;
	#endif
}

void __ISR(_CHANGE_NOTICE_E_VECTOR, ISR_IPL_CN) ChangeNotificationE(void) {
	#ifdef ISR_USE_CN
		PORTE;
		isr_cn();
		IFS3bits.CNEIF = 0;
	#endif
}

void __ISR(_CHANGE_NOTICE_F_VECTOR, ISR_IPL_CN) ChangeNotificationF(void) {
	#ifdef ISR_USE_CN
		PORTF;
		isr_cn();
		IFS3bits.CNFIF = 0;
	#endif
}

void __ISR(_CHANGE_NOTICE_G_VECTOR, ISR_IPL_CN) ChangeNotificationG(void) {
	#ifdef ISR_USE_CN
		PORTG;
		isr_cn();
		IFS3bits.CNGIF = 0;
	#endif
}

#ifdef _CHANGE_NOTICE_H_VECTOR
	void __ISR(_CHANGE_NOTICE_H_VECTOR, ISR_IPL_CN) ChangeNotificationH(void) {
		#ifdef ISR_USE_CN
			PORTH;
			isr_cn();
			IFS3bits.CNHIF = 0;
		#endif
	}
#endif

#ifdef _CHANGE_NOTICE_J_VECTOR
	void __ISR(_CHANGE_NOTICE_J_VECTOR, ISR_IPL_CN) ChangeNotificationJ(void) {
		#ifdef ISR_USE_CN
			PORTJ;
			isr_cn();
			IFS3bits.CNJIF = 0;
		#endif
	}
#endif

#ifdef _CHANGE_NOTICE_K_VECTOR
	void __ISR(_CHANGE_NOTICE_K_VECTOR, ISR_IPL_CN) ChangeNotificationK(void) {
		#ifdef ISR_USE_CN
			PORTK;
			isr_cn();
			IFS3bits.CNKIF = 0;
		#endif
	}
#endif

void __ISR(_UART1_FAULT_VECTOR, IPL2AUTO) IntUart1FAULTHandler(void) {
	if ((IFS3bits.U1EIF) && (IEC3bits.U1EIE)) {
		IFS3bits.U1EIF = 0;
		#ifdef ISR_USE_UART1
			isrSerialError_usart1();
		#endif
	}
}

void __ISR(_UART1_RX_VECTOR, IPL2AUTO) IntUart1RXHandler(void) {
	if ((IFS3bits.U1RXIF) && (IEC3bits.U1RXIE)) {
		IFS3bits.U1RXIF = 0;
		#ifdef ISR_USE_UART1
			isrSerialRx_usart1(UARTGetDataByte(UART1));
		#endif
		#ifdef ISR_USE_UART1_SIO
			isr_communication();
		#endif
	}
}

void __ISR(_UART1_TX_VECTOR, IPL2AUTO) IntUart1TXHandler(void) {
	if ((IFS3bits.U1TXIF) && (IEC3bits.U1TXIE)) {
		//INTClearFlag(INT_SOURCE_UART_TX(UART1)); //must not be cleared
		#ifdef ISR_USE_UART1
			isrSerialTx_usart1();
		#endif
	}
}

void __ISR(_UART2_FAULT_VECTOR, IPL7AUTO) IntUart2FAULTHandler(void) {
	if ((IFS4bits.U2EIF) && (IEC4bits.U2EIE)) {
		IFS4bits.U2EIF = 0;
		#ifdef ISR_USE_UART2
			isrSerialError_usart2();
		#endif
	}
}

void __ISR(_UART2_RX_VECTOR, IPL7AUTO) IntUart2RXHandler(void) {
	if ((IFS4bits.U2RXIF) && (IEC4bits.U2RXIE)) {
		IFS4bits.U2RXIF = 0;
		#ifdef ISR_USE_UART2
			isrSerialRx_usart2(UARTGetDataByte(UART2));
		#endif
		#ifdef ISR_USE_UART2_SIO
			isr_communication();
		#endif
	}
}

void __ISR(_UART2_TX_VECTOR, IPL7AUTO) IntUart2TXHandler(void) {
	if ((IFS4bits.U2TXIF) && (IEC4bits.U2TXIE)) {
		 //INTClearFlag(INT_SOURCE_UART_TX(UART2)); //must not be cleared
		#ifdef ISR_USE_UART2
			isrSerialTx_usart2();
		#endif
	}
}

void __ISR(_OUTPUT_COMPARE_1_VECTOR, IPL5AUTO) IntOC1Handler(void) {
	#ifdef ISR_USE_OC1
	#endif
}

void __ISR(_OUTPUT_COMPARE_2_VECTOR, IPL5AUTO) IntOC2Handler(void) {
	#ifdef ISR_USE_OC2
	#endif
}

void __ISR(_OUTPUT_COMPARE_3_VECTOR, IPL5AUTO) IntOC3Handler(void) {
	#ifdef ISR_USE_OC3
	#endif
}

void __ISR(_OUTPUT_COMPARE_4_VECTOR, IPL5AUTO) IntOC4Handler(void) {
	#ifdef ISR_USE_OC4
		isr_oc4();
	#endif
}

void __ISR(_CORE_TIMER_VECTOR, IPL2AUTO) CoreTimerHandler(void) {
	// clear the interrupt flag
	IFS0CLR = _IFS0_CTIF_MASK; 
}

void __ISR(_USB_VECTOR, ISR_IPL_USB) _IntHandlerUSBInstance0(void) {
	#ifdef ISR_USE_USB
	    isr_usb();
	#endif
}

void __ISR ( _USB_DMA_VECTOR, ISR_IPL_USB_DMA) _IntHandlerUSBInstance0_USBDMA ( void ) {
	#ifdef ISR_USE_USB_DMA
	    isr_usb_dma();
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
// Configure SRS Priority Selection
	PRISSbits.SS0 = 0;
	PRISSbits.PRI1SS = 1;
	PRISSbits.PRI2SS = 2;
	PRISSbits.PRI3SS = 3;
	PRISSbits.PRI4SS = 4;
	PRISSbits.PRI5SS = 5;
	PRISSbits.PRI6SS = 6;
	PRISSbits.PRI7SS = 7;

	/* Enable multi-vector interrupts. */
	_CP0_BIS_CAUSE( 0x00800000U );
	INTCONSET = _INTCON_MVEC_MASK;
	__builtin_enable_interrupts();
	#ifdef ISR_USE_CN
		#ifdef _CHANGE_NOTICE_A_VECTOR
			CNCONAbits.ON = 1;
		#endif
		CNCONBbits.ON = 1;
		CNCONCbits.ON = 1;
		CNCONDbits.ON = 1;
		CNCONEbits.ON = 1;
		CNCONFbits.ON = 1;
		CNCONGbits.ON = 1;
		#ifdef _CHANGE_NOTICE_H_VECTOR
			CNCONHbits.ON = 1;
		#endif
		#ifdef _CHANGE_NOTICE_J_VECTOR
			CNCONJbits.ON = 1;
		#endif
		#ifdef _CHANGE_NOTICE_K_VECTOR
			CNCONKbits.ON = 1;
		#endif
		#ifdef _CHANGE_NOTICE_A_VECTOR
			PORTA;
		#endif
		PORTB;
		PORTC;
		PORTD;
		PORTE;
		PORTF;
		PORTG;
		#ifdef _CHANGE_NOTICE_H_VECTOR
			PORTH;
		#endif
		#ifdef _CHANGE_NOTICE_J_VECTOR
			PORTJ;
		#endif
		#ifdef _CHANGE_NOTICE_K_VECTOR
			PORTK;
		#endif
		//CNEN are configured in respective application
		//CNPUE are configured in respective application
		#ifdef _CHANGE_NOTICE_A_VECTOR
			IPC29bits.CNAIS = 0;
			IPC29bits.CNAIP = ISR_IPLV_CN;
		#endif

		IPC29bits.CNBIS = 0;
		IPC29bits.CNBIP = ISR_IPLV_CN;

		IPC30bits.CNCIS = 0;
		IPC30bits.CNCIP = ISR_IPLV_CN;

		IPC30bits.CNDIS = 0;
		IPC30bits.CNDIP = ISR_IPLV_CN;

		IPC30bits.CNEIS = 0;
		IPC30bits.CNEIP = ISR_IPLV_CN;

		IPC30bits.CNFIS = 0;
		IPC30bits.CNFIP = ISR_IPLV_CN;

		IPC31bits.CNGIS = 0;
		IPC31bits.CNGIP = ISR_IPLV_CN;

		#ifdef _CHANGE_NOTICE_H_VECTOR
			IPC31bits.CNHIS = 0;
			IPC31bits.CNHIP = ISR_IPLV_CN;
		#endif

		#ifdef _CHANGE_NOTICE_J_VECTOR
			IPC31bits.CNJIS = 0;
			IPC31bits.CNJIP = ISR_IPLV_CN;
		#endif

		#ifdef _CHANGE_NOTICE_K_VECTOR
			IPC31bits.CNKIS = 0;
			IPC31bits.CNKIP = ISR_IPLV_CN;
		#endif


		#ifdef _CHANGE_NOTICE_A_VECTOR
			IFS3bits.CNAIF = 0;
			IEC3bits.CNAIE = 1;
		#endif

		IFS3bits.CNBIF = 0;
		IEC3bits.CNBIE = 1;

		IFS3bits.CNCIF = 0;
		IEC3bits.CNCIE = 1;

		IFS3bits.CNDIF = 0;
		IEC3bits.CNDIE = 1;

		IFS3bits.CNEIF = 0;
		IEC3bits.CNEIE = 1;

		IFS3bits.CNFIF = 0;
		IEC3bits.CNFIE = 1;

		IFS3bits.CNGIF = 0;
		IEC3bits.CNGIE = 1;

		#ifdef _CHANGE_NOTICE_H_VECTOR
			IFS3bits.CNHIF = 0;
			IEC3bits.CNHIE = 1;
		#endif
		
		#ifdef _CHANGE_NOTICE_J_VECTOR
			IFS3bits.CNJIF = 0;
			IEC3bits.CNJIE = 1;
		#endif

		#ifdef _CHANGE_NOTICE_K_VECTOR
			IFS3bits.CNKIF = 0;
			IEC3bits.CNKIE = 1;
		#endif

	#endif
}
