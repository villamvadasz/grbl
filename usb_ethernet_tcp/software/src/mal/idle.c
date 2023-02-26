#include "idle.h"

void __attribute__((nomips16)) PowerSaveIdle(void);

void idle_request(void) {
	PowerSaveIdle();
}
