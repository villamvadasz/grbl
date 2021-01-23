#include "mal.h"
#include "c_isr.h"

#include "k_stdtype.h"

typedef enum _EXCEP {
	EXCEP_IRQ = 0,			// interrupt
	EXCEP_AdEL = 4,			// address error exception (load or ifetch)
	EXCEP_AdES,				// address error exception (store)
	EXCEP_IBE,				// bus error (ifetch)
	EXCEP_DBE,				// bus error (load/store)
	EXCEP_Sys,				// syscall
	EXCEP_Bp,				// breakpoint
	EXCEP_RI,				// reserved instruction
	EXCEP_CpU,				// coprocessor unusable
	EXCEP_Overflow,			// arithmetic overflow
	EXCEP_Trap,				// trap (possible divide by zero)
	EXCEP_IS1 = 16,			// implementation specfic 1
	EXCEP_CEU,				// CorExtend Unuseable
	EXCEP_C2E				// coprocessor 2
} EXCEP;

typedef struct _ExceptionLog {
	unsigned int _excep_code;
	unsigned int _excep_addr;
	unsigned int magicWord;
} ExceptionLog;

EXCEP _excep_code;
unsigned int _excep_addr;
unsigned int _return_addr;

#pragma GCC diagnostic error "-w"
volatile ExceptionLog exceptionLog[16] __attribute__ ((persistent, address(0xA0000200)));
#pragma GCC diagnostic pop

uint32 exceptionCounter = 0;
uint16 generateException = 0;
volatile int tempA;
volatile int tempB;
volatile int tempResult;

// this function overrides the normal _weak_ generic handler
void _general_exception_handler(void) {
	uint16 exceptionLogCnt = 0;
	uint8 doSkipInstruction = 0;
	uint8 doReset = 1;
	asm volatile("mfc0 %0,$13" : "=r" (_excep_code));
	asm volatile("mfc0 %0,$14" : "=r" (_excep_addr));

	_excep_code = (_excep_code & 0x0000007C) >> 2;
	
	{//Find empty index
		unsigned int x = 0;
		for (x = 0; x < (sizeof(exceptionLog) / sizeof(*exceptionLog)); x++) {
			if (exceptionLog[x].magicWord == 0xDEAD) {//Hopefully magic word wont be written by POR
				exceptionLogCnt = x + 1;
			} else {
                break;
            }
		}
	}

	if (exceptionLogCnt < (sizeof(exceptionLog) / sizeof(*exceptionLog))) {
		exceptionLog[exceptionLogCnt]._excep_code = _excep_code;
		exceptionLog[exceptionLogCnt]._excep_addr = _excep_addr;
		exceptionLog[exceptionLogCnt].magicWord = 0xDEAD;
		
		exceptionLogCnt++;
	}

	//Here add a store to eeprom
	
	exceptionCounter++;
	
	switch (_excep_code) {
		case EXCEP_IRQ : {
			break;
		}
		case EXCEP_AdEL : {
			break;
		}
		case EXCEP_AdES : {
			break;
		}
		case EXCEP_IBE : {
			doReset = 1;
			break;
		}
		case EXCEP_DBE : {
			doReset = 0;
			doSkipInstruction = 1;
			break;
		}
		case EXCEP_Sys : {
			break;
		}
		case EXCEP_Bp : {
			break;
		}
		case EXCEP_RI : {
			doReset = 0;
			break;
		}
		case EXCEP_CpU : {
			break;
		}
		case EXCEP_Overflow : {
			doReset = 0;
			break;
		}
		case EXCEP_Trap : {
			doReset = 0;
			doSkipInstruction = 1;
			break;
		}
		case EXCEP_IS1 : {
			break;
		}
		case EXCEP_CEU : {
			break;
		}
		case EXCEP_C2E : {
			break;
		}
		default : {
			break;
		}
	}
	#ifdef exceptions_user_callout
		exceptions_user_callout();
	#endif
	
	if (doReset) {
		doReset = 0;
		mal_reset();
	}

	if (doSkipInstruction) {
		doSkipInstruction = 0;
		_return_addr = _excep_addr + 4;
		//asm volatile("mfc0 %0,$31" : "=r" (_return_addr)); 
		asm volatile("mtc0 %0,$14" : "=r" (_return_addr)); 
	}
}

void do_exception_examples(void) {
/*	if (generateException != 0) {
		switch (generateException) {
			case 1 : {//Divison by 0
				{
					tempA = 5;
					tempB = 0;
					tempResult = tempA / tempB;
				}
				break;
			}
			case 2 : {//Null ptr read
				{
					unsigned int * ptr = 0;
					unsigned int temp = 0xcafe;
					temp = *ptr;
				}
				break;
			}
			case 3 : {//Null ptr write
				{
					unsigned int * ptr = 0;
					unsigned int temp = 0xcafe;
					*ptr = temp;
				}
				break;
			}
			case 4 : {//Arithmetic overflow
				{
					tempA = 0x7FFFFFFF;
					tempB = 0x7FFFFFFF;
					tempResult = tempA * tempB;
				}
				break;
			}
		}
		generateException = 0;
	}*/
}

void exceptions_clearException(void) {
	exceptionCounter = 0;
}

uint32 exceptions_getException(void) {
	uint32 result = 0;
	result = exceptionCounter;
	return result;
}
