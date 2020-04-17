#ifndef _STACKMEASURE_H_
#define _STACKMEASURE_H_

	extern void init_stackmeasure(void);
	extern void do_stackmeasure(void);
	extern unsigned int getStackFillnessMax(void);
	extern unsigned int getStackSize(void);
	
	extern void fill_stack(void); // Call as early as possible. Does not write over already used stack space.
	extern unsigned int stack_usage(void); // Returns the number of unused stack words.

#endif
