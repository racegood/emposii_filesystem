#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "config.h"
#include "lib.h"
#include "reg.h"

extern void SerialInit(void);
extern void PutChar(const char ch);
extern void PutString(const char *ch);

#endif
