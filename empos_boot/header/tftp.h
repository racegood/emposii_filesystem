#ident "$Id: tftp.h$"

#ifndef SMART_TFTP_H
#define SMART_TFTP_H

#define TFTP_PORT			69		// Well known TFTP port #
#define TFTP_TIMEOUT			5		// Seconds to tftpTimeout for a lost pkt
//#define TFTP_TIMEOUT			3		// Seconds to tftpTimeout for a lost pkt
#define TIMEOUT_COUNT			10		// # of tftpTimeouts before giving up

// TFTP operations.
#define TFTP_RRQ			1
#define TFTP_WRQ			2
#define TFTP_DATA			3
#define TFTP_ACK			4
#define TFTP_ERROR			5

#define TFTP_NONE			0
#define TFTP_STATE_RRQ			1
#define TFTP_STATE_DATA			2
#define TFTP_STATE_TOO_LARGE		3
#define TFTP_STATE_BAD_MAGIC		4
#define TFTP_STATE_SUCCESS		5
#define TFTP_STATE_FAILURE		6

#include <main.h>

bool		TftpStart(char *filename);
void		TftpTx(char *txBuf);
bool		DoTftp(int argc, char **argv);	// Begin TFTP get.
bool		TftpRx(char *rxBuf, int len);

extern u16	tftpPort;
extern u16	tftpHostPort;
extern u16	tftpClientPort;

#endif		
