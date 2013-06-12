#ident "$Id: header.h$"

#ifndef SMART_HEADER_H
#define SMART_HEADER_H

#include <types.h>

// ethernet packet page.
typedef struct {
	// Bus Interface Registers
	u32	pp_id;				// Product identification.
	u8	pp_res1[28];			// reserved.
	u16	pp_baddr;			// ip base address.
	u16	pp_int_num;			// interrupt number.
	u16	pp_dchan;			// DMA Channel Number.
	u16	pp_dstart_frame;		// DMA Start of Frame.
	u16	pp_dframe_cnt;		// DMA Frame Count.
	u16	pp_rx_dbyte_cnt;		// RxDMA Byte Count.
	u32	pp_mbase_addr;		// Memory Base Address Register.
	u32	pp_bprom_base_addr;	// Boot PROM Base Address.
	u32	pp_bprom_addr_mask;	// Boot PROM Address Mask.
	u8	pp_res2[8];			// reserved.
	u16	pp_eeprom_cmd;		// EEPROM Command.
	u16	pp_eeprom_data;		// EEPROM Data.
	u8	pp_res3[12];			// reserved.
	u16	pp_rxf_byte_cnt;		// Received Frame Byte Counter.
	u8	pp_res4[174];			// reserved.
	// Status and Control Registers.
	u8	pp_conf_ctrl[32];		// Configuration & Control Registers.
	u8	pp_state_event[32];		// Status & Event Registers.
	u8	pp_res5[4];
	// Initiate Transmit Registers.
	u16	pp_txcmd;			// TxCMD (transmit command).
	u16	pp_txlen;			// TxLENGTH (transmit length).
	u8	pp_res6[8];			// reserved.
	// Address Filter Registers.
	u8	pp_addr_filter[8];		// Logical Address Filter.
	u8	pp_iaddr[6];			// Individual Address.
	u8	pp_res7[674];
	// Frame Location.
	u16	pp_rxstatus;			// RxStatus.
	u16	pp_rxlen;			// RxLength.
	u8	pp_rxfloc[1532];		// Rx Frame Location.
	u8	pp_txfloc[1536];		// Tx Frame Location.
} PACKET_PAGE;				// cs8900a.


// Ethernet Header.
// ethernet header를 사용하면 et_protlen까지만 사용하며,
// et_protlen은 protocol type으로 사용한다.
typedef struct {
	u8		et_dest[6];		// Destination Mac Address.
	u8		et_src[6];		// Source Mac Address.
	u16		et_protlen;		// if ethernet header protocol, else length.
	u8		et_dsap;		// DSAP.
	u8		et_ssap;		// SSAP.
	u8		et_ctl;			// contorl.
	u8		et_snap1;		// SNAP.
	u8		et_snap2;
	u8		et_snap3;
	u16		et_prot;		// protocol.
} ETH_HEADER;
#define	ETHER_HDR_SIZE		14
#define	E802_HDR_SIZE		22
#define PROT_IP				0x0800
#define	PROT_ARP			0x0806
#define PROT_RARP				0x8035


// IP Header.
typedef struct {
	char	ip_hl_v;	// version and header length(version은 현재 4, length는 4 byte 단위).
	char	ip_tos;		// type of service.
	short	ip_len;		// total length (IP Header를 포함한 길이).
	short	ip_id;		// identification.
	short	ip_off;		// fragment offset field.
	char	ip_ttl;		// time to live (원하는 값, 0x40 or 0xff).
	char	ip_p;		// protocol (UDP:17).
	short	ip_chksum;	// checksum.
	long	ip_src;		// source ip address.
	long	ip_dest;	// destination ip address.
} IP_HEADER;
#define IP_HDR_SIZE		20


// udp header.
typedef struct {
	u16	udp_src;	// udp source port.
	u16	udp_dest;	// udp destination port.
	u16	udp_len;	// length of udp packet.
	u16	udp_chksum;	// checksum.
} UDP_HEADER;
#define UDP_HDR_SIZE	8


// Address Resolution Protocol (ARP) header.
typedef struct {
	u16		ar_hrd;		// Format of hardware address.
	u16		ar_pro;		// Format of protocol address.
	u8		ar_hln;		// Length of hardware address.
	u8		ar_pln;		// Length of protocol address.
	u16		ar_op;		// Operation.
	u8		ar_data[0];
} ARP_HEADER;

#define ARP_HDR_SIZE			(8+20)	// Size assuming ethernet.
// for ar_hrd of ARP_HEADER.
#define ARP_ETHER				1		// Ethernet hardware address.
// for ar_op of ARP_HEADER.
#define ARPOP_REQUEST			1		// Request to resolve address.
#define ARPOP_REPLY			2		// Response to previous request.
#define RARPOP_REQUEST			3		// Request to resolve address.
#define RARPOP_REPLY			4		// Response to previous request.
#define ETH_P_IP       			0x0800  	// Internet Protocol packet.   raoudi 

// BOOTP header.
typedef struct {
	u8		bh_opcode;		// Operation.
	u8		bh_htype;		// Hardware type.
	u8		bh_hlen;		// Hardware address length.
	u8		bh_hops;		// Hop count (gateway thing).
	u32		bh_tid;			// Transaction ID.
	u16		bh_secs;		// Seconds since boot.
	u16		bh_res1;		// Reserved(alignment).
	u32		bh_ciaddr;		// Client IP address.
	u32		bh_yiaddr;		// Your (client) IP address.
	u32		bh_siaddr;		// Server IP address(Server가 setting).
	u32		bh_giaddr;		// Gateway IP address.
	u8		bh_chaddr[16];	// Client hardware address(Mac Address).
	char		bh_sname[64];	// Server host name.
	char		bh_file[128];	// Boot file name.
	char		bh_vend[64];	// Vendor information.
} BOOTP_HEADER;

#define BOOTP_HDR_SIZE			300
#define BOOTP_SIZE				(ETHER_HDR_SIZE + IP_HDR_SIZE + UDP_HDR_SIZE + BOOTP_HDR_SIZE)
#define OP_BOOTREQUEST			1
#define OP_BOOTREPLY	 		2
#define HWT_ETHER	 			1
#define HWL_ETHER				0x06

struct ethhdr {
        u8  et_dest[6];              // destination mac address
        u8  et_src[6];               // source mac address.
        u16 et_protlen;              // if ethernet header protocol, else length.
/*
        uint8  et_dsap;                 // DSAP.
        uint8  et_ssap;                 // SSAP.
        uint8  et_ctl;                  // contorl.
        uint8  et_snap1;                // SNAP.
        uint8  et_snap2;
        uint8  et_snap3;
        uint16 et_prot;                 // protocol.
*/
};

#endif
