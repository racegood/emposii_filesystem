#ident "$Id: lib.h$"

#ifndef SMART_LIB_H
#define SMART_LIB_H

#include "types.h"
//#include <net.h>
#define SWAP32(A)               ((((A)&0x000000ff)<<24) | (((A)&0x0000ff00)<<8) | (((A)&0x00ff0000)>>8) | (((A)&0xff000000)>>24))

void	MemCpy32(unsigned long *dest, unsigned long *src, int numWords);
void	MemCpy(void *dest, void *src, int len);
void	MemSet(void *dest, const char c, int len);
int	StrCmp(char *dest, char *src);
int	StrNCmp(char *s1, char *s2, int len);

int 	MemCmp(void *addr1, void *addr2, int len);
bool	HexToInt(char *s, void *retval, int type);
u32	StrToInt(char *ch);
int 	StrLen(char *dest);
void 	StrCpy(char *dest, char *src);
int	StrChar ( char *s, char aChar );

void	printf(char *fmt, ...);
void	PrintString(char *fmt, char *s);
int	MOD(int num, int dev);		//armv5의 xscale용으로 코드를 최적화 하면 나누기를 처리하지 못한다.
int	DEV(int num, int dev);			//

void	PrintDec(char *fmt, int l);
void	PrintChar(char *fmt, char c);
void	PrintHex(char *fmt, int l);
bool	DecToLong(char *s, long *retval);
int	Power(int num, int cnt);
void	ClearLine(void);
//int	IoTest();
void	user_wait(unsigned long delay_factor);

char *	StrTok(char *s, const char *ct);
int	  	StrSpn(const char *s, const char *accept);
char *	StrpBrk(const char *cs, const char *ct);
#endif

