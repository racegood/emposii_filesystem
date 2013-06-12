#include <net.h>
#include <bootp.h>
#include <tftp.h>
#include <lib.h>
#include <reg.h>
#include <header.h>
#include <smc91111.h>
#include <time.h>
#include <debug_msg.h>

extern u32 tftpTimeout;

u8	hostEther[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u8	broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
u32	hostIP=0;
#if 0	/* for bootp debug :defined by smc91111.c */
u8	clientEther[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
#else
extern u8 clientEtehr[6];
#endif
u32	clientIP=0;

short	protocol=NOPROTOCOL;
short	ipID;

u8	PktBuf[MAX_PKT_SIZE];

bool TxPacket(char *txPktBuf, int len){
	return eth_send(txPktBuf, len);
}	// TxPacket.


bool RxPacket(char *rxPktBuf){
	return smc_rcv(rxPktBuf);
}



bool NetInit(){
	return eth_init();
}


struct arp_cache {
        bool exist;
        u32 ip;
        u8 haddr[8];
        time_t last;
};

static struct arp_cache arp_cache1;

bool SendArpRequest(u32 ip){
        u8 *rxPktBuf;
        rxPktBuf = PktBuf;

	// ethernet header.
	MemCpy(&rxPktBuf[0], broadcast, 6);
	MemCpy(&rxPktBuf[6], clientEther, 6);
	*(short *)(&rxPktBuf[12]) = SWAP16(PROT_ARP);
	// ARP packet.
	// hard type.
	*(short *)(&rxPktBuf[14]) = SWAP16(0x0001);
	// prot type.
	*(short *)(&rxPktBuf[16]) = SWAP16(0x0800);
	// hardware address size.
	rxPktBuf[18]              = 0x06;
	// prot size.
	rxPktBuf[19]              = 0x04;
	// op.
	*(short *)(&rxPktBuf[20]) = SWAP16(ARPOP_REQUEST);
	// Sender Ethernet Address.
	MemCpy(&(rxPktBuf[32]), "\0\0\0\0\0\0", 6);
	// Sender IP Address.
	MemCpy(&(rxPktBuf[38]), &(ip), 4);
	// Target Ethernet Address.
	MemCpy(&(rxPktBuf[22]), clientEther, 6);
	// Target IP Address.
	MemCpy(&(rxPktBuf[28]), &(clientIP), 4);

        return TxPacket(rxPktBuf,ETHER_HDR_SIZE+ARP_HDR_SIZE);
}

void RxPacketHandle(char *rxPktBuf, int len){
	ETH_HEADER		*et = (ETH_HEADER *)rxPktBuf;
	IP_HEADER		*ip = (IP_HEADER *)(rxPktBuf+ETHER_HDR_SIZE);
	UDP_HEADER		*udp = (UDP_HEADER *)(rxPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE);
	ARP_HEADER		*arp = (ARP_HEADER *)(rxPktBuf+ETHER_HDR_SIZE);
	u16			prot;
	time_t          	now;


	prot = SWAP16(et->et_protlen);

	dbg("Rcv packet len %d, prot=%s(%x)", len, (prot==PROT_ARP?"PROT_ARP":(prot==PROT_IP?"PROT_IP":"ALSE")), prot);

	switch (prot){
		case PROT_ARP:

			// error check.
			if (len<ARP_HDR_SIZE) return;
	//		if (SWAP16(arp->ar_op) != ARPOP_REQUEST) return;
			if (SWAP16(arp->ar_hrd) != ARP_ETHER) return;
			if (SWAP16(arp->ar_pro) != PROT_IP) return;
			if (SWAP16(arp->ar_hln != 6)) return;
			if (SWAP16(arp->ar_pln != 4)) return;

			if(SWAP16(arp->ar_op) == ARPOP_REQUEST) {
				if (clientIP==0 || MemCmp(&(arp->ar_data[16]), &clientIP, 4)) return;

				// ethernet header.
				MemCpy(&rxPktBuf[0], arp->ar_data, 6);
				MemCpy(&rxPktBuf[6], clientEther, 6);
				*(short *)(&rxPktBuf[12]) = SWAP16(PROT_ARP);
				// ARP packet.
				// hard type.
				*(short *)(&rxPktBuf[14]) = SWAP16(0x0001);
				// prot type.
				*(short *)(&rxPktBuf[16]) = SWAP16(0x0800);
				// hardware address size.
				rxPktBuf[18]              = 0x06;
				// prot size.
				rxPktBuf[19]              = 0x04;
				// op.
				*(short *)(&rxPktBuf[20]) = SWAP16(ARPOP_REPLY);
				// Sender Ethernet Address.
				MemCpy(&(rxPktBuf[32]), arp->ar_data, 6);
				// Sender IP Address.
				MemCpy(&(rxPktBuf[38]), arp->ar_data+6, 4);
				// Target Ethernet Address.
				MemCpy(&(rxPktBuf[22]), clientEther, 6);
				// Target IP Address.
				MemCpy(&(rxPktBuf[28]), &(clientIP), 4);

				dbg("%s","Rx ARP_REQ");

				tftpTimeout = GetTime(&now) + TFTP_TIMEOUT * HZ;

				// ARP packet을 보냄.
				eth_send(rxPktBuf, ETHER_HDR_SIZE+ARP_HDR_SIZE);

				dbg("%s","Tx ARP_REPLY");

			} else if (SWAP16(arp->ar_op) == ARPOP_REPLY) {
				time_t now;
				struct arp_cache *ap = &arp_cache1;
				if (ap->exist) return;
				if (MemCmp(&ap->ip, arp->ar_data+6,4)) return;  // arp->ar_data+6 : Host Ip address 
				ap->exist = true;
				ap->last = GetTime(&now);
				MemCpy(ap->haddr, arp->ar_data, 6);

				
				dbg("%s","Rx ARP_REPLY");
			}

			break;
		case PROT_IP:
			// error check.
			// length check.
			if (len < ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE) return;	// udp만 사용하므로 udp 포함하여 길이 check.
			if (len < ETHER_HDR_SIZE+SWAP16(ip->ip_len)) return;
			// 현재 ip version은 4.
			if ((ip->ip_hl_v & 0xf0) != 0x40) return;
						// Can't deal fragments.
			if (ip->ip_off & SWAP16(0x1fff)) return;
						// check checksum.
			if (!IPChksumOK((char *)ip, IP_HDR_SIZE / 2)) return;

			// client의 ip와 받은 packet의 ip 비교.
		//	if (clientIP && MemCmp(&ip->ip_dest, &clientIP, 4)) return; //comment by jsbae :ip address stored flash rom 
										    //setip 를 사용하여 저장된 주소와 bootp 를 사용하여
										   // 얻은 주소가 다를수 있다 

			// udp인지 check. 17은 udp의 번호.
			if (ip->ip_p!=17) return;

			// packet을 처리할 수 있는 함수를 호출.
			switch (protocol){
				case PROT_BOOTP :

					dbg("%s","Rx PROT_BOOTP");
					// check udp.
					if (udp->udp_src!=SWAP16(bootps) || udp->udp_dest!=SWAP16(bootpc)) break;
					// bootp 처리 함수 호출. 올바른 Bootp Reply이면, Host의 Ethernet 주소를 기록함.
					if (BootpRx((char *)(rxPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE),
							SWAP16(udp->udp_len)-UDP_HDR_SIZE)==true){
						MemCpy(hostEther, (char *)et->et_src,6);
					}
					break;
				case PROT_TFTP :
					dbg("%s","Rx PROT_TFTP and check udp");
					// ckeck udp.
					if (udp->udp_src!=SWAP16(tftpPort) && (tftpHostPort!=0 && udp->udp_src!=SWAP16(tftpHostPort))) break;
					if (udp->udp_dest!=SWAP16(tftpPort) && udp->udp_dest!=SWAP16(tftpClientPort)) break;
					if (tftpHostPort==0) tftpHostPort=SWAP16(udp->udp_src);

					dbg("%s","Rx PROT_TFTP and check udp ... OK");
					// tftp 처리 함수 호출.
					TftpRx((char *)(rxPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE),
							SWAP16(udp->udp_len)-UDP_HDR_SIZE);
					break;
				default :
					break;
			}
			break;
	}


	return;
}	// RxPacketHandle.


int IPChksumOK(char *ptr, int len){
	return !((IPChksum(ptr, len) + 1) & 0xfffe);
}	// IPCksumOk.


unsigned IPChksum(char *ptr, int len){
	u32		xsum;

	xsum = 0;
	while (len-- > 0)
		xsum += *((u16 *)ptr)++;
	xsum = (xsum & 0xffff) + (xsum >> 16);
	xsum = (xsum & 0xffff) + (xsum >> 16);
	return xsum & 0xffff;
}	// IPChksum.


inline void *CheckArpCache(u32 ip) {
        time_t now;
        struct arp_cache *ap = &arp_cache1;

        if (!ap->exist) goto failed;                            /* not exist */
        if (GetTime(&now) > ap->last + 5) goto failed;             /* expired */
        if (ip != ap->ip) goto failed;                          /* no match */
        ap->last = GetTime(&now);
        return ap->haddr;
failed :
        return 0;
}

void *FindMacAddr(u32 ip) {
	void 	*mac;
	clock_t timeout;
	time_t 	now;
	struct 	arp_cache *ap = &arp_cache1;
	
	mac = CheckArpCache(ip);
	if (mac) return mac;

        ap->exist = false;
        ap->ip = ip;
        timeout =  GetTime(&now) + TFTP_TIMEOUT * HZ;
	SendArpRequest(ip);
        while (GetTime(&now) < timeout){
                RxPacket(PktBuf);
                if (ap->exist && (ap->ip == ip))
		{
                        return ap->haddr;
		}
        }
        return 0;
}

void SetEtherHeader(char *ethHeader, char *hEth, u16 prot){
	volatile ETH_HEADER		*et = (ETH_HEADER *)ethHeader;

	MemCpy((char *)et->et_dest, hEth, 6);
	MemCpy((char *)et->et_src, clientEther, 6);
	et->et_protlen = SWAP16(prot);	// set 800 (ip).
	return;
}	// SetEtherHeader.


void SetIPHeader(char *ipHeader, u32 clientIP, u32 hostIP, u16 len){
	volatile IP_HEADER	*ip = (volatile IP_HEADER *)ipHeader;

	// If the data is an odd number of bytes, zero the
	// byte after the last byte so that the checksum will work.
	if (len & 1) ipHeader[IP_HDR_SIZE + len] = 0;

	// Construct an IP header(need to set no fragment bit - XXX).
	ip->ip_hl_v                   = 0x45;					// 현재 ip version 4, IP_HDR_SIZE / 4 (not including UDP)
	ip->ip_tos                    = 0;						// type of service. 현재 network에서 지원안함. 항상 0.
	ip->ip_len                    = SWAP16(IP_HDR_SIZE + len);		// total length of ip packet.
	ip->ip_id                     = SWAP16(ipID++);				// identifier.
	ip->ip_off                    = SWAP16(0x0000);				// No fragmentation.
	ip->ip_ttl                    = 64;						// time to live.
	ip->ip_p                      = 17;						// UDP.
	ip->ip_chksum                 = 0;
	MemCpy((char *)&(ip->ip_src), &(clientIP), 4);
	MemCpy((char *)&(ip->ip_dest), &(hostIP), 4);
//	(volatile long)ip->ip_src     = clientIP;
//	(volatile long)ip->ip_dest    = hostIP;
	ip->ip_chksum = ~IPChksum((char *)ip, IP_HDR_SIZE / 2);
}	// SetIPHeader.


void SetUdpHeader(char *udpHeader, u16 hPort, u16 cPort, u16 len){
	volatile UDP_HEADER		*udp = (UDP_HEADER *)udpHeader;

	udp->udp_dest    = SWAP16(hPort);
	udp->udp_src     = SWAP16(cPort);
	udp->udp_len     = SWAP16(UDP_HDR_SIZE + len);
	udp->udp_chksum  = 0;
	return;
}	// SetUdpHeader.


void PrintIPAddr(long ip){
	ip = SWAP32(ip);
	printf("%d.%d.%d.%d", (char)((ip>>24)&0xff), (char)((ip>>16)&0xff),
					(char)((ip>>8)&0xff), (char)((ip>>0)&0xff));
	return;
}	// NetPrintIPaddr.


void PrintEthAddr(char *ethAddr){
	int		i;

	for (i=0; i<6; i++){
		if (i && i%2==0) printf(" ");
		printf("%02x", ethAddr[i]);
	}
	return;
}
