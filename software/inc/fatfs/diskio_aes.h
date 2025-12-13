#ifndef _DISKIO_AES_H_
#define _DISKIO_AES_H_

#include "k_stdtype.h"

extern void encrpytDisk(BYTE drv);
extern void decryptDisk(BYTE drv);

extern void diskIsEncrypted(void);
extern void diskIsDecrypted(void);

#endif
