#include <stdio.h>
#include <string.h>

#include "ad.h"
#include "c_ad.h"

#include "mal.h"

uint16 adResults[AD_CHANNEL_NUM];

uint8 doAd100ms = 0;

void init_ad(void) {
	memset(adResults, 0x00, sizeof(adResults));
	//V_BAT
	TRISBbits.TRISB12 = 1;
	#ifdef __32MX440F256H__
		AD1PCFGbits.PCFG12 = 0;
	#else
	#ifdef __32MX470F512H__
	    ANSELBbits.ANSB12 = 1;
	#else
	#ifdef __32MX460F512L__
		AD1PCFGbits.PCFG12 = 0;
	#else
	#ifdef __32MX470F512L__
		ANSELA = 0;
		ANSELB = 0;
		ANSELC = 0;
		ANSELD = 0;
		ANSELE = 0;
		ANSELF = 0;
		ANSELG = 0;
	#else
	#ifdef __32MZ2048ECG144__
		
	#else
		#error TODO Implement
	#endif
	#endif
	#endif
	#endif
	#endif

	#ifndef __32MZ2048ECG144__
		AD1CON1CLR = 0x8000;    // disable ADC before configuration

		AD1CON1 = 0x00E0;       // internal counter ends sampling and starts conversion (auto-convert), manual sample
		AD1CON2 = 0;            // AD1CON2<15:13> set voltage reference to pins AVSS/AVDD
		AD1CON3 = 0x1f01;       // TAD = 4*TPB, acquisition time = 15*TAD 
		AD1CON1SET = 0x8000;    // Enable ADC

		AD1CHSbits.CH0SA = 12;       // AD1CHS<16:19> controls which analog pin goes to the ADC
		AD1CON1bits.SAMP = 1;           // Begin sampling
	#endif
}

void do_ad(void) {
	if (doAd100ms) {
		doAd100ms = 0;
		{
			#ifndef __32MZ2048ECG144__
				if ((AD1CON1bits.SAMP == 0) && (AD1CON1bits.DONE == 1)) {
					adResults[0] = ADC1BUF0;                // result stored in ADC1BUF0
					AD1CON1bits.SAMP = 1;           // Begin sampling
				}
			#endif
		}
	}
}

void isr_ad_1ms(void) {
	static uint8 doAd100msCnt = 0;
	doAd100msCnt ++;
	if (doAd100msCnt >= 100) {
		doAd100msCnt = 0;
		doAd100ms = 1;
	}
}

uint16 getAd(unsigned char ch) {
	uint16 result = 0;
	if (ch < AD_CHANNEL_NUM) {
		result = adResults[ch];
	}
	return result;
}
