#include "idle.h"
#include "mal.h"

void idle_request(void) {
 	mSysUnlockOpLock(OSCCONCLR = (1 << _OSCCON_SLPEN_POSITION));
    asm("WAIT");
}

