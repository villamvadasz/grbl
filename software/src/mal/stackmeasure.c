#include "stackmeasure.h"

#define STACKMEASURE_FILL_PATTERN 0xAA5555AA

extern unsigned int _heap	  __attribute__((section(".data")));		// Start of Heap storage
extern unsigned int _splim	 __attribute__((section(".data")));		// Stack pointer limit?
extern unsigned int _stack	 __attribute__((section(".data")));		// Start of Stack. 
//extern unsigned int _heap;		// Start of Heap storage
//extern unsigned int _splim;		// Stack pointer limit?
//extern unsigned int _stack;		// Start of Stack. 

unsigned int stackmeasure_current_pos = 0;
unsigned int stackmeasure_from = ((unsigned int)&_splim);
unsigned int stackmeasure_to = ((unsigned int)&_stack);

unsigned int stackmeasure_maxfillness = 0;

void init_stackmeasure(void) {
	stackmeasure_maxfillness = ((unsigned int)&_stack) - ((unsigned int)&_splim);
	stackmeasure_current_pos = stackmeasure_from;
	stackmeasure_from = ((unsigned int)&_splim);
	stackmeasure_to = ((unsigned int)&_stack);
	fill_stack();
}

void do_stackmeasure(void) {
	if (
		(*((unsigned int *)stackmeasure_current_pos) == STACKMEASURE_FILL_PATTERN) &&
		(stackmeasure_current_pos <= stackmeasure_to)
	) {
		stackmeasure_current_pos += 4;
		//stack pattern ok
	} else {
		//stack pattern not ok
		stackmeasure_maxfillness = ((unsigned int)&_stack) - stackmeasure_current_pos;
		//stackmeasure_maxfillness *= 4;
		stackmeasure_from = ((unsigned int)&_splim);
		stackmeasure_to = ((unsigned int)&_stack);
		stackmeasure_current_pos = stackmeasure_from;
	}
}

unsigned int getStackFillnessMax(void) {
	unsigned int result = 0;
	
	result = stackmeasure_maxfillness;
	
	return result;
}

unsigned int getStackSize(void) {
	unsigned int result = 0;
	
	result = stackmeasure_to - stackmeasure_from;
	//result *= 4;

	return result;	
}
