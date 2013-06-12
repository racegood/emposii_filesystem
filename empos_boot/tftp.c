#include <net.h>
#include <tftp.h>
#include <bootp.h>
#include <lib.h>
#include <config.h>
#include <time.h>
#include <debug_msg.h>

static short	tftpState = 0;
static unsigned short	tftpBlock = 0;
static short	tftpLastBlock = 0;
static u32	loadAddr = 0;
u16		tftpPort = 69;
u16		tftpHostPort=0;
u16		tftpClientPort = 7777;
static char	requestFile[128];

u32	tftpTimeout;
u8 *dmac=hostEther;

bool DoTftp(int argc, char **argv){
	// tftp로 받은 file data를 기록할 주소를 생성.
	if (argc!=3){
		return false;
	}if (!StrCmp(argv[2], "kernel")){
		loadAddr = (long)KERNEL_RAM_BASE;
	}else if (!StrCmp(argv[2], "ramdisk")){
		loadAddr = (long)RAMDISK_RAM_BASE;
	}else if (!StrCmp(argv[2], "booter")){
		loadAddr = (long)BOOTLOADER_RAM_BASE;
	}else if (!StrCmp(argv[2], "usr")){
		loadAddr = (long)USR_RAM_BASE;
	}else {
		return false;
	}

	// IP가 없으면 bootp를 실행하여 IP를 할당 받음.
	if (clientIP==0){
		printf("No IP.  Run bootp first to get a valiable IP...\n");
		if (DoBootp(1,0)==false){
			printf("\tBootp failed. try again.\n");
			return false;
		}
	}
	if (TftpStart(argv[1]))	// argv[1] : filename.
		return true;
	else 
		return false;
}


bool TftpStart(char *filename){
	char		*txPktBuf, *rxPktBuf;
	time_t		now;

	txPktBuf = PktBuf;
	rxPktBuf = PktBuf;

	// check filename.
	if (!filename || !filename[0]){
		printf("boot file name is not exists.\n");
		return false;
	}
	
	printf("Tftp start...\n");
	
	printf("\tHost   (server) IP       : ");
	PrintIPAddr(hostIP);
	printf("\n");
	
	printf("\tClient (target) IP       : ");
	PrintIPAddr(clientIP);
	printf("\n");

	StrCpy(requestFile, filename);
	printf("\tLoading Filename         : %s\n", requestFile);
	printf("\tSave Address             : 0x%08lx\n", loadAddr);
	
	printf("\nLoading start...\n");

	protocol = PROT_TFTP;
	tftpState = TFTP_STATE_RRQ;
	tftpHostPort = 0;

	TftpTx(txPktBuf);

	tftpTimeout = GetTime(&now) + TFTP_TIMEOUT * HZ;
	
	while (tftpState!=TFTP_STATE_SUCCESS && tftpState!=TFTP_STATE_FAILURE){
		RxPacket(rxPktBuf);
		if (GetTime(&now)>tftpTimeout){
			printf("\n\tTftp is failed. Try again.\n\n");
			tftpState=TFTP_STATE_FAILURE;
		}
	}

	protocol = NOPROTOCOL;
	tftpState = TFTP_NONE;
	tftpBlock = 0;
	tftpHostPort = 0;
	return true;
}	// TftpStart.


void TftpTx(char *txPktBuf){
	char		*pktPtr = (char *)(txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE);
	u16		hostPort = 0;
	int		len = 0;

	
	//MemSet(txPktBuf, 0, MAX_PKT_SIZE);
	MemSet(txPktBuf, 0, MAX_PKT_SIZE);

	// make tftp header.
	switch (tftpState){
		// tftp request packet을 만듬.
		case TFTP_STATE_RRQ:
			//printf("tftp state rrq\n");
			*((u16 *)pktPtr)++ = SWAP16(TFTP_RRQ);
			StrCpy((char *)pktPtr, requestFile);
			pktPtr += StrLen(requestFile) + 1;
			StrCpy((char *)pktPtr, "octet");
			pktPtr += 5 + 1;
			len = pktPtr - (txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE);
			hostPort = tftpPort;

			dbg("%s", "TX RRQ");
			break;

		// 수신한 data packet에 대한 acknowledge packet을 만듬.
		case TFTP_STATE_DATA:
			//printf("tftp state data\n");
			*((u16 *)pktPtr)++ = SWAP16(TFTP_ACK);
			*((u16 *)pktPtr)++ = SWAP16(tftpBlock);
			len = pktPtr - (txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE);
			hostPort = tftpHostPort;

			dbg("%s", "TX TFTP_ACK");
		//	break;

		// error packet을 만듬.
		case TFTP_STATE_TOO_LARGE:
			//printf("tftp too large\n");
			*((u16 *)pktPtr)++ = SWAP16(TFTP_ERROR);
			*((u16 *)pktPtr)++ = SWAP16(3);
			StrCpy((char *)pktPtr, "File too large");
			pktPtr += 14 + 1;
			len = pktPtr - (txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE);
			hostPort = tftpHostPort;

			dbg("%s", "TX TFTP_ERROR1");

			break;

		// error packet을 만듬.
		case TFTP_STATE_BAD_MAGIC:
			//printf("tftp bad magic\n");
			*((u16 *)pktPtr)++ = SWAP16(TFTP_ERROR);
			*((u16 *)pktPtr)++ = SWAP16(2);
			StrCpy((char *)pktPtr, "File has bad magic");
			pktPtr += 18 + 1;
			len = pktPtr - (txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE);
			hostPort = tftpHostPort;

			dbg("%s", "TX TFTP_ERROR2");
			break;
		default :

			dbg("%s", "TX default");
			return;
	}

	dmac =  FindMacAddr(hostIP);
	if (!dmac) { 
		dmac = hostEther;
	}
		
	// Ethernet, IP, UDP Header를 만듬.
	SetUdpHeader((char *)(txPktBuf+ETHER_HDR_SIZE+IP_HDR_SIZE), hostPort, tftpClientPort, len);
	SetIPHeader((char *)(txPktBuf+ETHER_HDR_SIZE), clientIP, hostIP, UDP_HDR_SIZE+len);
//	SetEtherHeader(txPktBuf, hostEther, PROT_IP);
	SetEtherHeader(txPktBuf, dmac, PROT_IP);

	// tftp packet 전송.
	TxPacket(txPktBuf, ETHER_HDR_SIZE+IP_HDR_SIZE+UDP_HDR_SIZE+len);

	dbg("Tx packet(%d)-42", len);
	return;
}	// TftpTx.


bool TftpRx(char *rxPktBuf, int len){
	char			*pktPtr = rxPktBuf;
	short			value;
	unsigned long		rlen=0;			// tftp로 전송받은 data의 길이(임시 buffer).
	time_t			now;

	dbg("%s","Rev TFTP");
	// handling received data.
	value = SWAP16(*(short *)pktPtr);	// packet type.
	pktPtr+=2;
	len-=2;
	switch (value){
		case TFTP_RRQ :
		case TFTP_WRQ :
		case TFTP_ACK :
		default:
			break;
		// tftp data packet.
		case TFTP_DATA :
			// error. check data index.
			if (len < 2) return false;
			tftpBlock = SWAP16(*(u16 *)pktPtr);	// tftp data packet에서 block 번호.
			
			pktPtr += 2;
			len -= 2;
			// tftp receive 진행 표시.
			if (((tftpBlock)%128)==0){	// each 64 kbyte.
				ClearLine();
				rlen = (unsigned long)tftpBlock * 512;
				printf("\t0x%08lx bytes received.", rlen);
			}

			// request에 대한 첫번째 data packet일때 필요한 정보 설정.
			if (tftpState == TFTP_STATE_RRQ){
				tftpState = TFTP_STATE_DATA;
				tftpLastBlock = 0;

				if (tftpBlock != 1){
					tftpState = TFTP_STATE_FAILURE;
					return false;
				}
			}
			//tftpState = TFTP_STATE_RRQ;
			//tftpState = TFTP_STATE_DATA;
			// 이전의 packet을 다시 받았을 때는 무시.
			if (tftpBlock <= tftpLastBlock) break;

			tftpLastBlock = tftpBlock;

			MemCpy((char *)(loadAddr+(tftpBlock-1)*512), (char *)pktPtr, len);

			tftpTimeout = GetTime(&now) + TFTP_TIMEOUT * HZ;

			dbg("Rx TFTP_DATA(%d)", tftpLastBlock);

			TftpTx(rxPktBuf);


			if (len < 512){
				// We received the whole thing.  Try to run it.
				ClearLine();
				rlen = ((long)tftpBlock-1) * 512 + len;
				printf("\t0x%08lx (%ld) bytes received.\n", rlen, rlen);
				printf("\ttftp done.\n\n");
				tftpState = TFTP_STATE_SUCCESS;
			}
			break;
		// error 처리.
		case TFTP_ERROR :
			// tftp RRQ or tftp ack pakcet has some error.
			printf("\tTFTP error check the file name..\n");

		//	printf("\nTFTP error: '%s' (%d)\n", pktPtr+2, SWAP16(*(u16 *)pktPtr));
			tftpState = TFTP_STATE_FAILURE;
			break;
	}

	return true;
}	// TftpRx.
