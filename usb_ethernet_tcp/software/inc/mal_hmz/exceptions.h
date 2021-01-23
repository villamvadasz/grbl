#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

	#include "k_stdtype.h"

	typedef struct _ExceptionLog {
		unsigned int _excep_code;
		unsigned int _excep_addr;
		unsigned int magicWord;
	} ExceptionLog;

	extern volatile ExceptionLog exceptionLog[16] __attribute__ ((persistent, address(0x80000200)));

	extern void exceptions_clearException(void);
	extern uint32 exceptions_getException(void);

	extern void do_exception_examples(void);

#endif
