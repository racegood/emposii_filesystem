#include <time.h>
#include <net.h>
#include <bootp.h>
#include <lib.h>

u32	bootpID;
u32	hostIP;
short	bootpState;
u16	bootps=67;			// bootp server port #.
u16	bootpc=68;			// bootp client port #.

u8	broadcastEther[6]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u32	broadcastIP = 0xffffffff;
u32	noIP		= 0x00000000;

void SetBootpHeader(char *bootpHeader){
	time_t now;
	BOOTP_HEADER	*bhp = (BOOTP_HEADER *)bootpHeader;

	bhp->bh_opcode = OP_BOOTREQUEST;			// 1 : request		2 : reply.
	bhp->bh_htype  = HWT_ETHER;					// 10 Base Ethernet : 1.
	bhp->bh_hlen   = HWL_ETHER;					// 10 Base Ethernet : 6.
	bhp->bh_hops   = 0;							// client에서 0으로 setting. gateway가 사용.
	bhp->bh_secs   = SWAP16(GetTime(&now) / HZ);	// 

	MemSet(&(bhp->bh_ciaddr), 0, 4);
	MemSet(&(bhp->bh_yiaddr), 0, 4);
	MemSet(&(bhp->bh_siaddr), 0, 4);
	MemSet(&(bhp->bh_giaddr), 0, 4);
	MemCpy((char *)bhp->bh_chaddr, clientEther, 6);

	bootpID = ((u32)clientEther[2] << 24)
		| ((u32)clientEther[3] << 16)
		| ((u32)clientEther[4] << 8)
		| (u32)clientEther[5];
	bootpID += GetTime(&now);
	MemCpy(&(bhp->bh_tid), &(bootpID), 4);
}	// SetBootpHeader.

bool BootpTx(void){
	int			i;
	char		*txPktBuf, *rxPktBuf;
	long		delay;		// delay : 1초 delay후 실패면 재시도.
	time_t now;

	txPktBuf = PktBuf;
	rxPktBuf = PktBuf;

	bootpState = BOOTP_CONTINUE;
	protocol = PROT_BOOTP;

	// make boop packet.
	MemSet(txPktBuf, 0, MAX_PKT_SIZE);
	SetBootpHeader((char *)(txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE));
	SetUdpHeader((char *)(txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE), bootps, bootpc, BOOTP_HDR_SIZE);
	SetIPHeader((char *)(txPktBuf+ETHER_HDR_SIZE), noIP, broadcastIP, UDP_HDR_SIZE+BOOTP_HDR_SIZE);
	SetEtherHeader(txPktBuf, broadcastEther, PROT_IP);

	//if (!NetInit()) return false;

	// view info.
	printf("Our Ethernet address    : ");
	PrintEthAddr(clientEther);
	printf(".\n");
	printf("Sending bootp packet...\n");
	
	// bootp operation.
	for (i=0; i<TIMEOUT; i++){
		// transmit bootp packet to host.
		printf(".");
		if (!TxPacket(txPktBuf, ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE+BOOTP_HDR_SIZE)) break;
		// receive bootp packet from host.
		delay = GetTime(&now)+HZ;
		while (GetTime(&now)<delay && bootpState==BOOTP_CONTINUE){
			RxPacket(rxPktBuf);
			if (bootpState==BOOTP_SUCCESS) break;

		}
		if (bootpState==BOOTP_SUCCESS) break;
	}
	printf("\n");

	protocol = NOPROTOCOL;

	if (bootpState==BOOTP_SUCCESS){
		printf("Bootp Packet received.\n");
		
		printf("\tHost   (server) Ethernet : ");
		PrintEthAddr(hostEther);
		printf("\n");

		printf("\tHost   (server) IP       : ");
		PrintIPAddr(hostIP);
		printf("\n");

		printf("\tClient (target) Ethernet : ");
		PrintEthAddr(clientEther);
		printf("\n");

		printf("\tClient (target) IP       : ");
		PrintIPAddr(clientIP);
		printf("\n");

		printf("\n");
		return true;
	}
	else {
		printf("Bootp packet is not received.\n\n");
		return false;
	}
	return true;
}	// BootpTx.


bool BootpRx(char *bootpHeader, short len){
	BOOTP_HEADER	*bhp = (BOOTP_HEADER *)bootpHeader;

	if (len!=BOOTP_HDR_SIZE)		return false;

	// check bootp.
	if (bhp->bh_opcode!=2)			return false;
	if (bhp->bh_htype!=1)			return false;
	if (bhp->bh_hlen!=6)			return false;
	if (MemCmp(&bhp->bh_tid, &bootpID, 4)) return false;
	if (MemCmp(&bhp->bh_chaddr, &clientEther, 6)) return false;

	// get infomation from bootp packet.
	MemCpy(&clientIP, &bhp->bh_yiaddr, 4);
	MemCpy(&hostIP  , &bhp->bh_siaddr, 4);

	bootpState = BOOTP_SUCCESS;
	return true;
}	// BootpRx.

bool DoBootp(int argc, char **argv){

	if(argc !=1) return false;

	BootpTx();
		return true;
}
