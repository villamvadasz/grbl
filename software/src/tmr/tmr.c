#include <string.h>
#include <stdio.h>
#include "mal.h"
#include "tmr.h"
#include "c_tmr.h"

#include "c_main.h"
#include "k_stdtype.h"
#include "c_isr.h"

#define T1_ON							(1 << _T1CON_ON_POSITION)	/* Timer1 ON */
#define T1_OFF							(0)

// Stop-in-idle control - values are mutually exclusive
#define T1_IDLE_STOP					(1 << _T1CON_SIDL_POSITION)	 /* stop during idle */
#define T1_IDLE_CON						(0)							 /* operate during idle */

// Asynchronous write control - values are mutually exclusive
#define T1_TMWDIS_ON					(1 << _T1CON_TWDIS_POSITION)	/* Asynchronous Write Disable */
#define T1_TMWDIS_OFF					(0)

// Timer gate control - values are mutually exclusive
#define T1_GATE_ON						(1 << _T1CON_TGATE_POSITION)	/* Timer Gate accumulation mode ON */
#define T1_GATE_OFF						(0)

// Timer prescaler control - values are mutually exclusive
#define T1_PS_1_256						(3 << _T1CON_TCKPS_POSITION)	/* Prescaler 1:256 */
#define T1_PS_1_64						(2 << _T1CON_TCKPS_POSITION)	/*		1:64 */
#define T1_PS_1_8						(1 << _T1CON_TCKPS_POSITION)	/*		1:8 */
#define T1_PS_1_1						(0)							 /*		1:1 */

// Sync option - values are mutually exclusive
#define T1_SYNC_EXT_ON					(1 << _T1CON_TSYNC_POSITION)	/* Synch external clk input */
#define T1_SYNC_EXT_OFF					(0)

// Source selection - values are mutually exclusive
#define T1_SOURCE_EXT					(1 << _T1CON_TCS_POSITION)	/* External clock source */
#define T1_SOURCE_INT					(0)							 /* Internal clock source */

// Interrupt on/off - values are mutually exclusive
#define T1_INT_ON						(1 << 15)
#define T1_INT_OFF						(0)

// Interrupt priority - values are mutually exclusive
#define T1_INT_PRIOR_7					(7) // Timer int priority 7
#define T1_INT_PRIOR_6					(6) // Timer int priority 6
#define T1_INT_PRIOR_5					(5) // Timer int priority 5
#define T1_INT_PRIOR_4					(4) // Timer int priority 4
#define T1_INT_PRIOR_3					(3) // Timer int priority 3
#define T1_INT_PRIOR_2					(2) // Timer int priority 2
#define T1_INT_PRIOR_1					(1) // Timer int priority 1
#define T1_INT_PRIOR_0					(0) // Timer int priority 0

// Interrupt sub-priority - values are mutually exclusive
#define T1_INT_SUB_PRIOR_3				(3 << 4) // Timer int sub priority 3
#define T1_INT_SUB_PRIOR_2				(2 << 4) // Timer int sub priority 2
#define T1_INT_SUB_PRIOR_1				(1 << 4) // Timer int sub priority 1
#define T1_INT_SUB_PRIOR_0				(0 << 4) // Timer int sub priority 0

#define T2_ON							(1 << _T2CON_ON_POSITION)	/* Timer2 ON */
#define T2_OFF							(0)

// Stop-in-idle control - values are mutually exclusive
#define T2_IDLE_STOP					(1 << _T2CON_SIDL_POSITION)	 /* stop during idle */
#define T2_IDLE_CON						(0)							 /* operate during idle */

// Timer gate control - values are mutually exclusive
#define T2_GATE_ON						(1 << _T2CON_TGATE_POSITION)	/* Timer Gate accumulation mode ON */
#define T2_GATE_OFF						(0)

// Prescale values - values are mutually exclusive
#define T2_PS_1_256						(7 << _T2CON_TCKPS_POSITION)	/* Prescaler 1:256 */
#define T2_PS_1_64						(6 << _T2CON_TCKPS_POSITION)	/*		1:64 */
#define T2_PS_1_32						(5 << _T2CON_TCKPS_POSITION)	/*		1:32 */
#define T2_PS_1_16						(4 << _T2CON_TCKPS_POSITION)	/*		1:16 */
#define T2_PS_1_8						(3 << _T2CON_TCKPS_POSITION)	/*		1:8 */
#define T2_PS_1_4						(2 << _T2CON_TCKPS_POSITION)	/*		1:4 */
#define T2_PS_1_2						(1 << _T2CON_TCKPS_POSITION)	/*		1:2 */
#define T2_PS_1_1						(0)							 /*		1:1 */

// 32-bit or 16-bit - values are mutually exclusive
#define T2_32BIT_MODE_ON				(1 << _T2CON_T32_POSITION)	/* Enable 32-bit mode */
#define T2_32BIT_MODE_OFF				(0)

// Sync external clock option - values are mutually exclusive
#define T2_SOURCE_EXT					(1 << _T2CON_TCS_POSITION)	/* External clock source */
#define T2_SOURCE_INT					(0)							 /* Internal clock source */

// Interrupt on/off - values are mutually exclusive
#define T2_INT_ON						(1 << 15)	/* T2 Interrupt Enable */
#define T2_INT_OFF						(0)

// Interrupt priority - values are mutually exclusive
#define T2_INT_PRIOR_7					(7)
#define T2_INT_PRIOR_6					(6)
#define T2_INT_PRIOR_5					(5)
#define T2_INT_PRIOR_4					(4)
#define T2_INT_PRIOR_3					(3)
#define T2_INT_PRIOR_2					(2)
#define T2_INT_PRIOR_1					(1)
#define T2_INT_PRIOR_0					(0)

// Interrupt sub-priority - values are mutually exclusive
#define T2_INT_SUB_PRIOR_3				(3 << 4)
#define T2_INT_SUB_PRIOR_2				(2 << 4)
#define T2_INT_SUB_PRIOR_1				(1 << 4)
#define T2_INT_SUB_PRIOR_0				(0 << 4)



#define T3_ON							(1 << _T3CON_ON_POSITION)	/* Timer1 ON */
#define T3_OFF							(0)

// Stop-in-idle control - values are mutually exclusive
#define T3_IDLE_STOP					(1 << _T3CON_SIDL_POSITION)	 /* stop during idle */
#define T3_IDLE_CON						(0)							 /* operate during idle */

// Asynchronous write control - values are mutually exclusive
#define T3_TMWDIS_ON					(1 << _T3CON_TWDIS_POSITION)	/* Asynchronous Write Disable */
#define T3_TMWDIS_OFF					(0)

// Timer gate control - values are mutually exclusive
#define T3_GATE_ON						(1 << _T3CON_TGATE_POSITION)	/* Timer Gate accumulation mode ON */
#define T3_GATE_OFF						(0)

// Timer prescaler control - values are mutually exclusive
#define T3_PS_1_256						(7 << _T3CON_TCKPS_POSITION)	/* Prescaler 1:256 */
#define T3_PS_1_64						(6 << _T3CON_TCKPS_POSITION)	/*		1:64 */
#define T3_PS_1_32						(5 << _T3CON_TCKPS_POSITION)	/*		1:32 */
#define T3_PS_1_16						(4 << _T3CON_TCKPS_POSITION)	/*		1:16 */
#define T3_PS_1_8						(3 << _T3CON_TCKPS_POSITION)	/*		1:8 */
#define T3_PS_1_4						(2 << _T3CON_TCKPS_POSITION)	/*		1:4 */
#define T3_PS_1_2						(1 << _T3CON_TCKPS_POSITION)	/*		1:2 */
#define T3_PS_1_1						(0)							 /*		1:1 */

// Sync option - values are mutually exclusive
#define T3_SYNC_EXT_ON					(1 << _T3CON_TSYNC_POSITION)	/* Synch external clk input */
#define T3_SYNC_EXT_OFF					(0)

// Source selection - values are mutually exclusive
#define T3_SOURCE_EXT					(1 << _T3CON_TCS_POSITION)	/* External clock source */
#define T3_SOURCE_INT					(0)							 /* Internal clock source */

// Interrupt on/off - values are mutually exclusive
#define T3_INT_ON						(1 << 15)
#define T3_INT_OFF						(0)

// Interrupt priority - values are mutually exclusive
#define T3_INT_PRIOR_7					(7) // Timer int priority 7
#define T3_INT_PRIOR_6					(6) // Timer int priority 6
#define T3_INT_PRIOR_5					(5) // Timer int priority 5
#define T3_INT_PRIOR_4					(4) // Timer int priority 4
#define T3_INT_PRIOR_3					(3) // Timer int priority 3
#define T3_INT_PRIOR_2					(2) // Timer int priority 2
#define T3_INT_PRIOR_1					(1) // Timer int priority 1
#define T3_INT_PRIOR_0					(0) // Timer int priority 0

// Interrupt sub-priority - values are mutually exclusive
#define T3_INT_SUB_PRIOR_3				(3 << 4) // Timer int sub priority 3
#define T3_INT_SUB_PRIOR_2				(2 << 4) // Timer int sub priority 2
#define T3_INT_SUB_PRIOR_1				(1 << 4) // Timer int sub priority 1
#define T3_INT_SUB_PRIOR_0				(0 << 4) // Timer int sub priority 0




#define T4_ON							(1 << _T4CON_ON_POSITION)	/* Timer1 ON */
#define T4_OFF							(0)

// Stop-in-idle control - values are mutually exclusive
#define T4_IDLE_STOP					(1 << _T4CON_SIDL_POSITION)	 /* stop during idle */
#define T4_IDLE_CON						(0)							 /* operate during idle */

// Asynchronous write control - values are mutually exclusive
#define T4_TMWDIS_ON					(1 << _T4CON_TWDIS_POSITION)	/* Asynchronous Write Disable */
#define T4_TMWDIS_OFF					(0)

// Timer gate control - values are mutually exclusive
#define T4_GATE_ON						(1 << _T4CON_TGATE_POSITION)	/* Timer Gate accumulation mode ON */
#define T4_GATE_OFF						(0)

// Timer prescaler control - values are mutually exclusive
#define T4_PS_1_256						(7 << _T4CON_TCKPS_POSITION)	/* Prescaler 1:256 */
#define T4_PS_1_64						(6 << _T4CON_TCKPS_POSITION)	/*		1:64 */
#define T4_PS_1_32						(5 << _T4CON_TCKPS_POSITION)	/*		1:32 */
#define T4_PS_1_16						(4 << _T4CON_TCKPS_POSITION)	/*		1:16 */
#define T4_PS_1_8						(3 << _T4CON_TCKPS_POSITION)	/*		1:8 */
#define T4_PS_1_4						(2 << _T4CON_TCKPS_POSITION)	/*		1:4 */
#define T4_PS_1_2						(1 << _T4CON_TCKPS_POSITION)	/*		1:2 */
#define T4_PS_1_1						(0)							 /*		1:1 */

// Sync option - values are mutually exclusive
#define T4_SYNC_EXT_ON					(1 << _T4CON_TSYNC_POSITION)	/* Synch external clk input */
#define T4_SYNC_EXT_OFF					(0)

// Source selection - values are mutually exclusive
#define T4_SOURCE_EXT					(1 << _T4CON_TCS_POSITION)	/* External clock source */
#define T4_SOURCE_INT					(0)							 /* Internal clock source */

// Interrupt on/off - values are mutually exclusive
#define T4_INT_ON						(1 << 15)
#define T4_INT_OFF						(0)

// Interrupt priority - values are mutually exclusive
#define T4_INT_PRIOR_7					(7) // Timer int priority 7
#define T4_INT_PRIOR_6					(6) // Timer int priority 6
#define T4_INT_PRIOR_5					(5) // Timer int priority 5
#define T4_INT_PRIOR_4					(4) // Timer int priority 4
#define T4_INT_PRIOR_3					(3) // Timer int priority 3
#define T4_INT_PRIOR_2					(2) // Timer int priority 2
#define T4_INT_PRIOR_1					(1) // Timer int priority 1
#define T4_INT_PRIOR_0					(0) // Timer int priority 0

// Interrupt sub-priority - values are mutually exclusive
#define T4_INT_SUB_PRIOR_3				(3 << 4) // Timer int sub priority 3
#define T4_INT_SUB_PRIOR_2				(2 << 4) // Timer int sub priority 2
#define T4_INT_SUB_PRIOR_1				(1 << 4) // Timer int sub priority 1
#define T4_INT_SUB_PRIOR_0				(0 << 4) // Timer int sub priority 0



uint32 globalTime = 0;
uint32 globalTimeUs = 0;
#ifdef TMR_TMR4_LORA
	uint32 globalTimeUs_Lora = 0;
#endif
static volatile uint32 delayCnt = 0;
static volatile uint8 delayCntLock = 0;
static Timer *ptrList[TIMERCNT];
unsigned int ptrListCnt = 0;
#define SIZEOFPTR (sizeof(ptrList)/sizeof(*ptrList))
volatile uint32 tmr_stop_watchdog_triggering = 0;

void init_tmr (void) {
	uint8 x = 0;

	#ifndef TMR_USE_TMR4_1MS
		//TMR1 1ms
		unsigned int tmr1_prescaler_val = 0;
		#if PRESCALE1 == 256
			tmr1_prescaler_val = T1_PS_1_256;
		#elif PRESCALE1 == 32
			tmr1_prescaler_val = T1_PS_1_32;
		#elif PRESCALE1 == 16
			tmr1_prescaler_val = T1_PS_1_16;
		#else
			#error Not implemented prescaler
		#endif
		T1CON = ((T1_ON | T1_SOURCE_INT | tmr1_prescaler_val)&~(T1_ON));
		TMR1 = 0;
		PR1 = (PR1_CONFIG);
		T1CONSET =((T1_ON | T1_SOURCE_INT | tmr1_prescaler_val)&(T1_ON));

		IFS0CLR = _IFS0_T1IF_MASK;
		IPC1CLR = _IPC1_T1IP_MASK, IPC1SET = (((T1_INT_ON | ISR_IPLV_TMR1) & 7) << _IPC1_T1IP_POSITION);
		IPC1CLR = _IPC1_T1IS_MASK, IPC1SET = ((((T1_INT_ON | ISR_IPLV_TMR1) >> 4) & 3) << _IPC1_T1IS_POSITION);
		IEC0CLR = _IEC0_T1IE_MASK, IEC0SET = (((T1_INT_ON | ISR_IPLV_TMR1) >> 15) << _IEC0_T1IE_POSITION);
	#endif
	#ifdef TMR_USE_TMR4_1MS
		//TMR4 1ms
		unsigned int tmr4_prescaler_val = 0;
		#if PRESCALE1 == 256
			tmr4_prescaler_val = T4_PS_1_256;
		#elif PRESCALE1 == 32
			tmr4_prescaler_val = T4_PS_1_32;
		#elif PRESCALE1 == 16
			tmr4_prescaler_val = T4_PS_1_16;
		#else
			#error Not implemented prescaler
		#endif
		OpenTimer4(T4_ON | T4_SOURCE_INT | tmr4_prescaler_val, PR1_CONFIG);
		ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_2);
	#endif
	#ifdef TMR_USE_TMR1_SOSC
		OSCCONbits.SOSCEN = 1;
		OpenTimer1(T1_ON | T1_SOURCE_EXT | T1_PS_1_1, 32768);
		ConfigIntTimer1(T1_INT_ON | ISR_IPLV_TMR1);
	#endif

	#ifndef TMR_DISABLE_TMR2_100US
		//TMR2 100uS
		unsigned int tmr2_prescaler_val = 0;
		#if PRESCALE2 == 256
			tmr2_prescaler_val = T2_PS_1_256;
		#elif PRESCALE2 == 32
			tmr2_prescaler_val = T2_PS_1_32;
		#elif PRESCALE2 == 16
			tmr2_prescaler_val = T2_PS_1_16;
		#else
			#error Not implemented prescaler
		#endif
		T2CON = ((T2_ON | T2_SOURCE_INT | tmr2_prescaler_val)&~(T2_ON));
		TMR2 = 0;
		PR2 = (PR2_CONFIG);
		T2CONSET =((T2_ON | T2_SOURCE_INT | tmr2_prescaler_val)&(T2_ON));

		IFS0CLR = _IFS0_T2IF_MASK;
		IPC2CLR = _IPC2_T2IP_MASK, IPC2SET = (((T2_INT_ON | ISR_IPLV_TMR2) & 7) << _IPC2_T2IP_POSITION);
		IPC2CLR = _IPC2_T2IS_MASK, IPC2SET = ((((T2_INT_ON | ISR_IPLV_TMR2) >> 4) & 3) << _IPC2_T2IS_POSITION);
		IEC0CLR = _IEC0_T2IE_MASK, IEC0SET = (((T2_INT_ON | ISR_IPLV_TMR2) >> 15) << _IEC0_T2IE_POSITION);

	#endif

	#ifdef TMR_USE_TMR4_10US
		//TMR4 10uS
		T4CON = ((T4_ON | T4_SOURCE_INT | T4_PS_1_8)&~(T4_ON));
		TMR4 = 0;
		PR4 = (PR4_CONFIG);
		T4CONSET = ((T4_ON | T4_SOURCE_INT | T4_PS_1_8)&(T4_ON));

		#ifndef TMR_USE_TMR4_10US_DISABLE_INTERRUPT
			IFS0CLR = _IFS0_T4IF_MASK;
			IPC4CLR = _IPC4_T4IP_MASK, IPC4SET = (((T4_INT_ON | ISR_IPLV_TMR4) & 7) << _IPC4_T4IP_POSITION);
			IPC4CLR = _IPC4_T4IS_MASK, IPC4SET = ((((T4_INT_ON | ISR_IPLV_TMR4) >> 4) & 3) << _IPC4_T4IS_POSITION);
			IEC0CLR = _IEC0_T4IE_MASK, IEC0SET = (((T4_INT_ON | ISR_IPLV_TMR4) >> 15) << _IEC0_T4IE_POSITION);
		#endif
	#endif

	#ifdef TMR_ENABLE_TMR3_100US
		//TMR3 10uS
		T3CON = ((T3_ON | T3_SOURCE_INT | T3_PS_1_8)&~(T3_ON));
		TMR3 = 0;
		PR3 = (PR3_CONFIG);
		T3CONSET = ((T3_ON | T3_SOURCE_INT | T3_PS_1_8)&(T3_ON));

		IFS0CLR = _IFS0_T3IF_MASK;
		IPC3CLR = _IPC3_T3IP_MASK, IPC3SET = (((T3_INT_ON | ISR_IPLV_TMR4) & 7) << _IPC3_T3IP_POSITION);
		IPC3CLR = _IPC3_T3IS_MASK, IPC3SET = ((((T3_INT_ON | ISR_IPLV_TMR4) >> 4) & 3) << _IPC3_T3IS_POSITION);
		IEC0CLR = _IEC0_T3IE_MASK, IEC0SET = (((T3_INT_ON | ISR_IPLV_TMR4) >> 15) << _IEC0_T3IE_POSITION);
	#endif

	#ifdef TMR_TMR4_LORA
		#ifdef TMR_USE_TMR4_10US_DISABLE_INTERRUPT
			#error TMR4 can not be used twice (TMR_USE_TMR4_10US_DISABLE_INTERRUPT or TMR_TMR4_LORA)
		#endif
		#ifdef TMR_USE_TMR4_1MS
			#error TMR4 can not be used twice (TMR_USE_TMR4_1MS or TMR_TMR4_LORA)
		#endif
		T4CONbits.ON = 0;
		//T4CONbits.FRZ = 0;		//Continue to run in Debug Mode
		T4CONbits.SIDL = 0;		//Runs in IDLE mode
		T4CONbits.TGATE = 0;	//

		if (GetPeripheralClock() == 40000000) {
			T4CONbits.TCKPS = 3;	//1:8 Prescaler
		} else if (GetPeripheralClock() == 20000000) {
			T4CONbits.TCKPS = 2;	//1:4 Prescaler
		} else if (GetPeripheralClock() == 10000000) {
			T4CONbits.TCKPS = 1;	//1:2 Prescaler
		}

		T4CONbits.T32 = 0;		//TMR4 is a 16-bit timer
		//T4CONbits.TCS = 0;		//Internal Peripheral Clock
		PR4 = 0xFFFF;
		
		T4CONbits.ON = 1;

		IFS0CLR = _IFS0_T4IF_MASK;
		IPC4CLR = _IPC4_T4IP_MASK, IPC4SET = (((T4_INT_ON | ISR_IPLV_TMR4) & 7) << _IPC4_T4IP_POSITION);
		IPC4CLR = _IPC4_T4IS_MASK, IPC4SET = ((((T4_INT_ON | ISR_IPLV_TMR4) >> 4) & 3) << _IPC4_T4IS_POSITION);
		IEC0CLR = _IEC0_T4IE_MASK, IEC0SET = (((T4_INT_ON | ISR_IPLV_TMR4) >> 15) << _IEC0_T4IE_POSITION);
	#endif
	
	for (x = 0; x < SIZEOFPTR; x++) {
		ptrList[x] = NULL;
	}
}

void deinit_tmr (void) {
	uint8 x = 0;

	#ifndef TMR_USE_TMR4_1MS
		//TMR1 1ms
		T1CON = ((T1_OFF | T1_SOURCE_INT | T1_PS_1_256)&~(T1_ON));
		TMR1 = 0;
		PR1 = (0);
		T1CONSET=((T1_OFF | T1_SOURCE_INT | T1_PS_1_256)&(T1_ON));
	#endif
	#ifdef TMR_USE_TMR4_1MS
		//TMR4 1ms
		OpenTimer4(T4_OFF, 0);
	#endif
	#ifdef TMR_USE_TMR1_SOSC
		OSCCONbits.SOSCEN = 0;
		OpenTimer1(T1_OFF, 0);
	#endif

	#ifndef TMR_DISABLE_TMR2_100US
		//TMR2 100uS
		T2CON = ((T2_OFF)&~(T2_ON));
		TMR2 = 0;
		PR2 = (0);
		T2CONSET=((T2_OFF)&(T2_ON));
	#endif

	#ifdef TMR_USE_TMR4_10US
		//TMR4 10uS
		T4CON = 0;
		TMR4 = 0;
		PR4 = (0);
	#endif
	
	#ifdef TMR_TMR4_LORA
		T4CONbits.ON = 0;
	#endif
		
	for (x = 0; x < SIZEOFPTR; x++) {
		ptrList[x] = NULL;
	}
}

void do_tmr(void) {
}

void delayms(uint32 dt) {
	uint16 temp1 = 0;
	uint16 temp2 = 1;
	delayCntLock = 1;
	delayCnt = dt;
	delayCntLock = 0;
	while (1) {
		temp1 = delayCnt;
		temp2 = delayCnt;
		if (temp1 == temp2) {
			if (temp1 == 0) {
				break;
			}
		}
	}
}

void init_timer(Timer *ptr) {
	remove_timer(ptr);
	add_timer(ptr);
	write_timer(ptr, 0);
}

void add_timer(Timer *ptr) {
	uint8 x = 0;
	lock_isr();
	for (x = 0; x < SIZEOFPTR; x++) {
		if (ptrList[x] == NULL) {
			ptrList[x] = ptr;
			ptrListCnt++;
			break;
		}
	}
	unlock_isr();
}

void remove_timer(Timer *ptr) {
	uint8 x = 0;
	lock_isr();
	for (x = 0; x < SIZEOFPTR; x++) {
		if (ptrList[x] == ptr) {
			ptrList[x] = NULL;
			ptrListCnt--;
			break;
		}
	}
	unlock_isr();
}

uint32 read_timer(const Timer *ptr) {
	uint32 result = 0;
	if (ptr != NULL) {
		lock_isr();
		result = *ptr;
		unlock_isr();	
	}
	return result;
}

void write_timer(Timer *ptr, uint32 newValue) {
	if (ptr != NULL) {
		lock_isr();
		*ptr = newValue;
		unlock_isr();	
	}
}

void setTimer(uint32 d) {
	delayCntLock = 1;
	delayCnt = d;
	delayCntLock = 0;
}

uint32 getTimer(void) {
	uint32 temp1 = 0;
	uint32 temp2 = 1;
	while (temp1 != temp2) {
		temp1 = delayCnt;
		temp2 = delayCnt;
	}
	return temp2;
}

void isr_tmr4(void) { //xxus
	#ifdef TMR_TMR4_LORA
		globalTimeUs_Lora++;
	#endif

	TMR4_FUNCTIONS();
}

void isr_tmr3(void) { //custom
	TMR3_FUNCTIONS();
}

void isr_tmr2(void) { //100us
	globalTimeUs += 100;
	TMR2_FUNCTIONS();
}

void isr_tmr1(void) { //1ms
	uint8 x = 0;
	static uint8 ms10Cnt = 0;
	static uint8 ms100Cnt = 0;
	static uint8 ms250Cnt = 0;
	static uint16 ms500Cnt = 0;
	static uint16 ms1000Cnt = 0;
	
	if (tmr_stop_watchdog_triggering == 0) {
		ClearWDT();
	}

	globalTime++;

	TMR1_FUNCTIONS();

	if (delayCntLock == 0) {
		if (delayCnt != 0) {
			delayCnt--;
		}
	}

	ms10Cnt ++;
	if (ms10Cnt == 10) {
		ms10Cnt = 0;
	}

	ms100Cnt ++;
	if (ms100Cnt == 100) {
		ms100Cnt = 0;
	}
	
	ms250Cnt ++;
	if (ms250Cnt == 250) {
		ms250Cnt = 0;
	}

	ms500Cnt ++;
	if (ms500Cnt == 500) {
		ms500Cnt = 0;
	}

	ms1000Cnt ++;
	if (ms1000Cnt == 1000) {
		ms1000Cnt = 0;
	}

	for (x = 0; x < SIZEOFPTR; x++) {
		if (ptrList[x] != NULL) {
			if (*(ptrList[x]) != 0) {
				(*(ptrList[x]))--;
			}
		}
	}
}

uint32 getGlobalTime(void) {
	uint32 result = 0;
	lock_isr();
	result = globalTime;
	unlock_isr();
	return result;
}

uint32 getGlobalTimeUs(void) {
	uint32 result = 0;
	lock_isr();
	result = globalTimeUs;
	unlock_isr();
	return result;
}

#ifdef TMR_TMR4_LORA
	uint32 getGlobalTimeUs_Lora(void) {//5MHz timer 16 bit
		uint32 result = 0;
		volatile uint32 result_high = 0;
		volatile uint32 result_low_1 = 0;
		volatile uint32 result_low_2 = 0;

		while (1) {
			result_low_1 = TMR4;				//0xFFFF 	0x1200	0x1200
			result_high = globalTimeUs_Lora;	//Overflow	Ok		OK
			result_low_2 = TMR4;				//0x0000	0x1200	0x1201
			if (result_low_1 <= result_low_2) {	//>			==		<
				break;
			}
		}
		result = result_high;
		result <<= 16;
		result += result_low_1;

		result /= 5;
		
		return result;
	}
#endif
