#ident "$Id: time.h$"

#ifndef SMART_TIME_H
#define SMART_TIME_H

#include <types.h>

clock_t clock(void);
void InitTimer();
u32  GetTime(u32 *t);
void msleep(unsigned int msec);

#endif

