#ident "$Id: net.h$"

#ifndef SMART_NET_H
#define SMART_NET_H

#include <main.h>
#include <header.h>

// endian을 채인지 한다.
#ifndef SWAP8
#define SWAP8(A)		(A)
#define SWAP16(A)		((((A)&0x00ff)<<8) | ((A)>>8))
#define SWAP32(A)		((((A)&0x000000ff)<<24) | (((A)&0x0000ff00)<<8) | (((A)&0x00ff0000)>>8) | (((A)&0xff000000)>>24))
#endif

#define MAX_PKT_SIZE			608

#define NOPROTOCOL			0
#define PROT_BOOTP			68
#define PROT_TFTP				69

// 전역변수.
extern u8		broadcastEther[6];	// Ethernet boardcast address.
extern u32		broadcastIP;
extern u8		clientEther[6];		// Our ethernet address.
extern u32		clientIP;		// Our IP addr (0 = unknown).
extern u8		hostEther[6];		// Boot server enet address.
extern u32		hostIP;			// Server IP addr (0 = unkn).
extern short		protocol;

extern u8		PktBuf[MAX_PKT_SIZE];


bool		TxPacket(char *pkt, int len);
bool		RxPacket(char *pkt);
bool		NetInit();
void		RxPacketHandle(char *pkt, int len);
void		SetEtherHeader(char *buf, char *htEth, u16 prot);
void		SetIPHeader(char *ipHeader, u32 clientIP, u32 hostIP, u16 len);
void		SetUdpHeader(char *udpHeader, u16 hPort, u16 cPort, u16 len);
unsigned	IPChksum(char *ptr, int len);
int		IPChksumOK(char *ptr, int len);
void		PrintIPAddr(long ipAddr);
void		PrintEthAddr(char *ethAddr);
void		*FindMacAddr(u32 ip);
bool 		SendArpRequest(u32 ip);
inline void 	*CheckArpCache(u32 ip);


#endif
