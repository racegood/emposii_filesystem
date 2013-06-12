#ident "$Id: bootp.h$"

#ifndef SMART_BOOTP_H
#define SMART_BOOTP_H

#define TIMEOUT						20	// 1√  1»∏ request
#define PORT_BOOTPS					67	// Server UDP port.
#define PORT_BOOTPC					68	// Client UDP port.

#define BOOTP_FAILURE					0
#define BOOTP_SUCCESS					1
#define BOOTP_CONTINUE					2

#include <main.h>

bool	DoBootp(int argc, char **argv);
bool	BootpRx(char *rxBuf, short len);

extern u16	bootps;
extern u16	bootpc;

#endif
