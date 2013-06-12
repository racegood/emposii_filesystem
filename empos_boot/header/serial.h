#ident "$Id: serial.h$"

#ifndef SMART_SERIAL_H
#define SMART_SERIAL_H

extern void SerialInit(void);
extern void PutChar(const char ch);
extern void PutString(const char *ch);
extern int GetChar(char * ch);
extern int Download(int base, int length);

#endif
