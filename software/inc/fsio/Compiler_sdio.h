#ifndef _COMPILER_SDIO_H_
#define _COMPILER_FSIO_H_



	#define  SPI_ENABLE             (0x00008000)            /* Enable module */
	#define  SPI_DISABLE            (0x00000000)        /* Disable module */
	
	#define  SPI_IDLE_STOP          (0x00002000)        /* Discontinue module operation in idle mode */
	#define  SPI_IDLE_CON           (0x00000000)        /* Continue module operation in idle mode */
	
	#define  SPI_RX_OVFLOW          (0x00000040)            /* receive overflow bit */
	#define  SPI_RX_OVFLOW_CLR      (0x00000000)    /* Clear receive overflow bit */
	
	#define  FRAME_POL_ACTIVE_HIGH  (0x20000000)                    /* Frame pulse active high */
	#define  FRAME_POL_ACTIVE_LOW   (0x00000000)    /* Frame pulse active low */
	
	#define FRAME_SYNC_EDGE_COINCIDE (0x00020000)                   /* frame pulse coincides with the first bit clock */
	#define FRAME_SYNC_EDGE_PRECEDE (0x00000000)    /* frame pulse precedes the first bit clock */
	
	#define FIFO_BUFFER_ENABLE      (0x00000000)                    /* no enhanced buffer functionality */
	#define FIFO_BUFFER_DISABLE (0x00000000)        /* ineffective */
	
	#define  FRAME_ENABLE_ON        (0x80000000)        /* Frame SPI support enable */
	#define  FRAME_ENABLE_OFF       (0x00000000)  /* Frame SPI support Disable */
	
	#define  FRAME_SYNC_INPUT       (0x40000000)        /* Frame sync pulse Input (slave) */
	#define  FRAME_SYNC_OUTPUT      (0x00000000) /* Frame sync pulse Output (master) */
	
	#define  ENABLE_SCK_PIN         (0x00000000)            /* SCK pin is not handled here */
	#define  DISABLE_SCK_PIN        (0x00000000)    /* SCK pin is not handled here */
	
	#define  DISABLE_SDO_PIN        (0x00001000)        /* SDO pin is not used by module */
	#define  ENABLE_SDO_PIN         (0x00000000)    /* SDO pin is  used by module */
	
	#define  SPI_MODE32_ON          (0x00000800)            /* Communication is 32 bits wide */
	#define  SPI_MODE32_OFF         (0x00000000)        /* Communication is selected by SPI_MODE16_ON/OFF */
	
	#define  SPI_MODE16_ON          (0x00000400)            /* If SPI_MODE32_OFF, Communication is 16 bits wide */
	#define  SPI_MODE16_OFF         (0x00000000)            /* If SPI_MODE32_OFF, Communication is 8 bits wide */
	
	#define  SPI_MODE8_ON           (0x00000000)            /* If SPI_MODE32_OFF and SPI_MODE16_OFF, Communication is 8 bits wide */
	
	#define  SPI_SMP_ON             (0x00000200)        /* Input data sampled at end of data output time */
	#define  SPI_SMP_OFF            (0x00000000)    /* Input data sampled at middle of data output time */
	
	#define  SPI_CKE_ON             (0x00000100)        /* Transmit happens from active clock state to idle clock state*/
	#define  SPI_CKE_OFF            (0x00000000)    /* Transmit happens on transition from idle clock state to active clock state */
	
	#define  SLAVE_ENABLE_ON        (0x00000080)            /* Slave Select enbale */
	#define  SLAVE_ENABLE_OFF       (0x00000000)    /* Slave Select not used by module */
	
	#define  CLK_POL_ACTIVE_LOW     (0x00000040)                /* Idle state for clock is high, active is low */
	#define  CLK_POL_ACTIVE_HIGH    (0x00000000)    /* Idle state for clock is low, active is high */
	
	#define  MASTER_ENABLE_ON       (0x00000020)            /* Master Mode */
	#define  MASTER_ENABLE_OFF      (0x00000000)    /* Slave Mode */
	
	#define  SEC_PRESCAL_1_1        (0x0000001c)        /* Secondary Prescale 1:1   */
	#define  SEC_PRESCAL_2_1        (0x00000018)        /* Secondary Prescale 2:1   */
	#define  SEC_PRESCAL_3_1        (0x00000014)        /* Secondary Prescale 3:1   */
	#define  SEC_PRESCAL_4_1        (0x00000010)        /* Secondary Prescale 4:1   */
	#define  SEC_PRESCAL_5_1        (0x0000000c)        /* Secondary Prescale 5:1   */
	#define  SEC_PRESCAL_6_1        (0x00000008)        /* Secondary Prescale 6:1   */
	#define  SEC_PRESCAL_7_1        (0x00000004)        /* Secondary Prescale 7:1   */
	#define  SEC_PRESCAL_8_1        (0x00000000)        /* Secondary Prescale 8:1   */
	
	#define  PRI_PRESCAL_1_1        (0x00000003)        /* Primary Prescale 1:1     */
	#define  PRI_PRESCAL_4_1        (0x00000002)        /* Primary Prescale 4:1     */
	#define  PRI_PRESCAL_16_1       (0x00000001)        /* Primary Prescale 16:1    */
	#define  PRI_PRESCAL_64_1       (0x00000000)        /* Primary Prescale 64:1    */


	// Registers for the SPI module you want to use
	//#define MDD_USE_SPI_1
	#define MDD_USE_SPI_2

	//SPI Configuration
	#define SPI_START_CFG_1     (PRI_PRESCAL_64_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_ON)
	#define SPI_START_CFG_2     (SPI_ENABLE)

	// Define the SPI frequency
	#define SPI_FREQUENCY			(20000000)

	#if defined MDD_USE_SPI_1
		// Description: SD-SPI Chip Select Output bit
		#define SD_CS               LATBbits.LATB1
		// Description: SD-SPI Chip Select TRIS bit
		#define SD_CS_TRIS          TRISBbits.TRISB1

		// Description: SD-SPI Card Detect Input bit
		#define SD_CD               PORTFbits.RF0
		// Description: SD-SPI Card Detect TRIS bit
		#define SD_CD_TRIS          TRISFbits.TRISF0

		// Description: SD-SPI Write Protect Check Input bit
		#define SD_WE               PORTFbits.RF1
		// Description: SD-SPI Write Protect Check TRIS bit
		#define SD_WE_TRIS          TRISFbits.TRISF1
			  
		// Description: The main SPI control register
		#define SPICON1             SPI1CON
		// Description: The SPI status register
		#define SPISTAT             SPI1STAT
		// Description: The SPI Buffer
		#define SPIBUF              SPI1BUF
		// Description: The receive buffer full bit in the SPI status register
		#define SPISTAT_RBF         SPI1STATbits.SPIRBF
		// Description: The bitwise define for the SPI control register (i.e. _____bits)
		#define SPICON1bits         SPI1CONbits
		// Description: The bitwise define for the SPI status register (i.e. _____bits)
		#define SPISTATbits         SPI1STATbits
		// Description: The enable bit for the SPI module
		#define SPIENABLE           SPICON1bits.ON
		// Description: The definition for the SPI baud rate generator register (PIC32)
		#define SPIBRG			    SPI1BRG

		// Tris pins for SCK/SDI/SDO lines
		#if defined(__32MX360F512L__) // All PIC32MX3XX 
			// Description: The TRIS bit for the SCK pin
			#define SPICLOCK            TRISFbits.TRISF6
			// Description: The TRIS bit for the SDI pin
			#define SPIIN               TRISFbits.TRISF7
			// Description: The TRIS bit for the SDO pin
			#define SPIOUT              TRISFbits.TRISF8
		#else	// example: PIC32MX360F512L
			// Description: The TRIS bit for the SCK pin
			#define SPICLOCK            TRISDbits.TRISD10
			// Description: The TRIS bit for the SDI pin
			#define SPIIN               TRISCbits.TRISC4
			// Description: The TRIS bit for the SDO pin
			#define SPIOUT              TRISDbits.TRISD0
		#endif

			//SPI library functions
		#define putcSPI             putcSPI1
		#define getcSPI             getcSPI1
		#define OpenSPI(config1, config2)   OpenSPI1(config1, config2)

	#elif defined MDD_USE_SPI_2
		// Description: SD-SPI Chip Select Output bit
		#define SD_CS				PORTBbits.RB13
		// Description: SD-SPI Chip Select TRIS bit
		#define SD_CS_TRIS			TRISBbits.TRISB13

		// Description: SD-SPI Card Detect Input bit
		//#define SD_CD               PORTGbits.RG0
		// Description: SD-SPI Card Detect TRIS bit
		//#define SD_CD_TRIS          TRISGbits.TRISG0

		// Description: SD-SPI Write Protect Check Input bit
		//#define SD_WE               PORTGbits.RG1
		// Description: SD-SPI Write Protect Check TRIS bit
		//#define SD_WE_TRIS          TRISGbits.TRISG1

		// Description: The main SPI control register
		#define SPICON1             SPI2CON
		// Description: The SPI status register
		#define SPISTAT             SPI2STAT
		// Description: The SPI Buffer
		#define SPIBUF              SPI2BUF
		// Description: The receive buffer full bit in the SPI status register
		#define SPISTAT_RBF         SPI2STATbits.SPIRBF
		// Description: The bitwise define for the SPI control register (i.e. _____bits)
		#define SPICON1bits         SPI2CONbits
		// Description: The bitwise define for the SPI status register (i.e. _____bits)
		#define SPISTATbits         SPI2STATbits
		// Description: The enable bit for the SPI module
		#define SPIENABLE           SPI2CONbits.ON
		// Description: The definition for the SPI baud rate generator register (PIC32)
		#define SPIBRG			    SPI2BRG

		// Tris pins for SCK/SDI/SDO lines

		// Description: The TRIS bit for the SCK pin
		#define SPICLOCK            TRISGbits.TRISG6
		// Description: The TRIS bit for the SDI pin
		#define SPIIN               TRISGbits.TRISG7
		// Description: The TRIS bit for the SDO pin
		#define SPIOUT              TRISGbits.TRISG8
		//SPI library functions
		#define putcSPI             putcSPI2
		#define getcSPI             getcSPI2
		#define OpenSPI(config1, config2)   OpenSPI2(config1, config2)
	#endif       

	#define putcSPI1(data_out)  do{while(!SPI1STATbits.SPITBE); SPI1BUF=(data_out); }while(0)
	#define putcSPI2(data_out)  do{while(!SPI2STATbits.SPITBE); SPI2BUF=(data_out); }while(0)
    unsigned int getcSPI1(void);
    unsigned int getcSPI2(void);
    void OpenSPI1(unsigned int config1, unsigned int config2 );
    void OpenSPI2(unsigned int config1, unsigned int config2 );

#endif
