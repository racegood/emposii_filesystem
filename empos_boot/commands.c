#include <config.h>
#include <serial.h>
#include <net.h>
#include <time.h>
#include <lib.h>
#include <reg.h>
#include <main.h>
#include <commands.h>
#include <flash.h>
#include <smc91111.h>
/////////////////////////////////////////////////////////
// test function
/////////////////////////////////////////////////////////

#define first 	0
#define second  1

extern u8 *dmac;

bool DoTest(int argc, char **argv)
{
	if(StrCmp(argv[1], "led") ==0){
		u16 val;
		if (!HexToInt(argv[2], &val, 8)){
				return false;
		}

		__REG8(0x10600000) = val;
	}

  return true;
}

bool DoBoot(int argc, char **argv)
{
	if(argc !=1)
		return false;

	boot_kernel();
	return true;
}

bool DoReset(int argc, char **argv)
{
 	void (*reset)(void) = (void*)BOOTLOADER_ROM_BASE;

	if(argc !=1) return false;

	reset();
	printf("\nreset failed...\n\n");

	return true;
}

bool DoDownLoad(int argc, char **argv)
{
	if(argc !=2)
		return false;
	else if(StrCmp(argv[1], "kernel") ==0)
		Download(KERNEL_RAM_BASE, KERNEL_LENGTH);
	else if(StrCmp(argv[1], "ramdisk") ==0)
		Download(RAMDISK_RAM_BASE, RAMDISK_LENGTH);
	else if(StrCmp(argv[1], "booter") ==0)
		Download(BOOTLOADER_RAM_BASE, BOOTLOADER_LENGTH);
	else if(StrCmp(argv[1], "usr") ==0)
		Download(USR_RAM_BASE, USR_LENGTH);
	else
		return false;

	return true;
}

bool DoFlash(int argc, char **argv)
{
	long	dest=0, src=0, len, wsize;

	if(argc !=2)
		return false;

	if (!StrCmp(argv[1], "kernel")){
		dest = (long)KERNEL_ROM_BASE;
		src  = (long)KERNEL_RAM_BASE;
		len  = (long)KERNEL_LENGTH;
		printf("Saving kernel to flash...\n");
	}
	else if (!StrCmp(argv[1], "ramdisk")){
		dest = (long)RAMDISK_ROM_BASE;
		src  = (long)RAMDISK_RAM_BASE;
		len  = (long)RAMDISK_LENGTH;
		printf("Saving ramdisk to flash...\n");
	}
 	else if (!StrCmp(argv[1], "booter")){
		dest = (long)BOOTLOADER_ROM_BASE;
		src  = (long)BOOTLOADER_RAM_BASE;
		len  = (long)BOOTLOADER_LENGTH;
		printf("Saving bootloader to flash...\n");
	}
	else if (!StrCmp(argv[1], "usr")){
		dest = (long)USR_ROM_BASE;
		src  = (long)USR_RAM_BASE;
		len  = (long)USR_LENGTH;
		printf("Saving usr image to flash...\n");
	}else
		return false;

	// erase flash blocks.
	printf("\tErased block range\t: 0x%08lx ~ 0x%08lx\n", dest, (long)dest+len-1);
	if (!Erase((u32 *)dest, len)) return false;

	printf("\tDone.\n\n");

	// write to flash.
	printf("Write to flash...\n");

	wsize = FLASH_BLOCK_SIZE;

	do{
		printf("\tCurrent block address\t: 0x%08lx", (u32)dest);
		if (!FlashWrite((u32 *)dest, (u32 *)src)) break;

		dest += FLASH_BLOCK_SIZE;
		src += FLASH_BLOCK_SIZE;
			ClearLine();
	} while((len -= FLASH_BLOCK_SIZE) >0);

	printf("\tCurrent block address\t: 0x%08lx", (u32)dest);
	printf("\n\tDone.\n");

	__REG(dest) = (u32)(READ_ARRAY);

	return true;
}	// DoWriteToFlashBlocks.

bool DoErase(int argc, char **argv){

	u32	addr=0, len;

	if(argc == 1)
	{
		printf("\n\tErase Command need a Position \n\tex)erase {booter/kernel/ramdisk/usr}\n");
		return false;
	}

	if(argc ==2)
	{
	    if (!StrCmp(argv[1], "kernel")){
                addr = (long)KERNEL_ROM_BASE;
                len  = (long)KERNEL_LENGTH;
            }
            else if (!StrCmp(argv[1], "ramdisk")){
                addr = (long)RAMDISK_ROM_BASE;
                len  = (long)RAMDISK_LENGTH;
            }
            else if (!StrCmp(argv[1], "booter")){
                addr = (long)BOOTLOADER_ROM_BASE;
                len  = (long)BOOTLOADER_LENGTH;
            }
	    else if (!StrCmp(argv[1], "usr")){
                addr = (long)USR_ROM_BASE;
                len  = (long)USR_LENGTH;
            }
	    else
        	return false;
	}else if(argc ==3){
		if (!HexToInt(argv[1], &addr, 32)){
				return false;
		}
		if (!HexToInt(argv[2], &len, 32)){
				return false;
		}
	}

	if ((long)addr%FLASH_BLOCK_SIZE){
		printf("address is not flash block base.\n");
		return false;
	}
	// erase flash blocks.
	printf("\tErased block range\t: 0x%08lx ~ 0x%08lx\n", addr, (long)addr+len-1);
	if (!Erase((u32 *)addr, len)) return false;

	printf("\tDone.\n");
	return true;

}	
bool DoHelp(int argc, char **argv)
{
	//사용자에게 보여줄 도움말을 이곳에서 모두 편집한다.
	if(argc==1){
	printf("\n\t* The following commands are supported *\n");
	printf("\t 1. boot \t: Boot the kernel at ram\n");
	printf("\t 2. reset\t: Restart this bootloader at rom\n");
	printf("\t 3. bootp\t: Get IP by bootp\n");
	printf("\t 4. setip\t: Set IP on host and client\n");
	printf("\t 5. macwrite\t: Set MAC address on client\n");
	printf("\t 6. tftp  \t: Download images by tftp\n");
	printf("\t 7. download\t: Download images by uuencode\n");
	printf("\t 8. read  \t: Read value on a specific address\n");
	printf("\t 9. write \t: Write value on a specific address\n");
	printf("\t10. flash \t: Write images that are previously downloaded\n");
	printf("\t11. erase\t: Erase flash area \n");
	printf("\t12. status \t: Print S/W, H/W and network status\n");
	printf("\t13. help \t: Print help message for users to operate this program\n");
	printf("\t14. test \t: Add your code in this function for S/W, H/W test\n");

	printf("\n\t* Use 'help command' to get help on a specific command *\n");

	}else if(argc ==2){ //각각 명령어 들에 대한 자세한 설명
		printf("\n\t* Help for the '%s' *\n", argv[1]);
		if (!StrCmp(argv[1], "boot")){
			printf("\tboot : Boot the kernel at ram\n");
		}else if (!StrCmp(argv[1], "reset")){
			printf("\treset : Restart this bootloader at rom\n");
		}else if (!StrCmp(argv[1], "bootp")){
   			printf("\tbootp : Get IP by bootp\n");
		}else if (!StrCmp(argv[1], "setip")){
			printf("\tsetip [host ip] [client ip] : Set IP on host and client\n\n");

			printf("\t* Arguments explain *\n");
			printf("\t1. host ip : Host pc IP address in using\n");
			printf("\t2. client ip : Board IP address to set\n");

			printf("\n\t* Example use *\n");
			printf("\t1. setip 192.168.123.200 192.168.123.201\n");

		}else if (!StrCmp(argv[1], "macwrite")){
			printf("\tmacwrite {pri/sec} [MAC addr] : Set MAC addr on client\n\n");
			printf("\t* Arguments explain *\n");
			printf("\t1. target client : target to change MAC\n");
			printf("\t2. MAC addr :  Client MAC address in using\n");

			printf("\n\t* Example use *\n");
			printf("\t1. macwrite pri 000ba8000001\n");
		}else if (!StrCmp(argv[1], "tftp")){
			printf("\ttftp [file] [position] : Download images by tftp\n\n");

			printf("\t* Arguments explain *\n");
			printf("\t1. file : File name in tftp service directory\n");
			printf("\t2. position {booter/kernel/ramdisk/usr} : Saving position at ram\n");

			printf("\n\t* Example use *\n");
			printf("\t1. tftp zImage kernel\n");

		}else if (!StrCmp(argv[1], "download")){
			printf("\tdownload [position] : Download images by uuencode\n\n");

			printf("\t* Arguments explain *\n");
			printf("\t1. position {booter/kernel/ramdisk/usr} : Saving position at ram\n");

			printf("\n\t* Example use *\n");
			printf("\t1. <client> download kernel\n");
			printf("\t   <host> uuencode zImage zImage >/dev/ttyS0\n");

		}else if (!StrCmp(argv[1], "read")){
			printf("\tread {b8/b16/b32} [address] [len] : Read value on a specific address\n\n");

			printf("\t* Arguments explain *\n");
			printf("\t1. bit : read bit number per excute\n");
			printf("\t2. address : Specific address to read\n");
			printf("\t3. len : length to read, default is '0x1'\n");

			printf("\n\t* Example use *\n");
			printf("\t1. read b32 0x48000000 0xa\n");
			printf("\t2. read b8 0x48000004\n");

		}else if (!StrCmp(argv[1], "write")){
			printf("\twrite {b8/b16/b32} [address] [value] : Write value on a specific address\n\n");

			printf("\t* Arguments explain *\n");
			printf("\t1. bit : read bit number per excute\n");
			printf("\t2. address : Specific address to write\n");
			printf("\t3. value : Value to write\n");

			printf("\n\t* Example use *\n");
			printf("\t1. write b32 0x4800000C 0x12BC5554\n");

		}else if (!StrCmp(argv[1], "flash")){
			printf("\tflash [position] : Write images that are previously downloaded\n\n");

			printf("\t* Arguments explain *\n");
			printf("\t1. position {booter/kernel/ramdisk/usr} : Specific position to write\n");

			printf("\n\t* Example use *\n");
			printf("\t1. flash kernel\n");


		}else if (!StrCmp(argv[1], "erase")){
			printf("\terase [position], [start addr] [len] : Erase flash area\n\n");

			printf("\t* Arguments explain *\n");
			printf("\t1. position {booter/kernel/ramdisk/usr} : Specific position to erase\n");
			printf("\t2. stat addr : Start address to erase\n");
			printf("\t3. len : Erase length\n");

			printf("\n\t* Example use *\n");
			printf("\t1. erase kernel\n");
			printf("\t2. erase 0x0 0xc0000\n");

		}else if (!StrCmp(argv[1], "status")){
			printf("\tstatus : Print S/W, H/W and network status\n");
		}else if (!StrCmp(argv[1], "help")){
			printf("\thelp [command] : Print help message for users to operate this program\n\n");

			printf("\t* Arguments explain *\n");
			printf("\t1. command : Specific command to help, default is NULL\n");

			printf("\n\t* Example use *\n");
			printf("\t1. help\n");
			printf("\t2. help reset\n");
		}else if (!StrCmp(argv[1], "test")){
			printf("\ttest [..][..]... : Add your code in this function for S/W, H/W test\n\n");
		}else
			printf("\tThis command is not supported...\n\n");
	}else
	  return false;


	return true;
}
bool DoRead (int argc, char **argv)
{
	u32 addr;
	u32 len;

	int i;
	if (argc == 3)
		len=1;

	else if(argc == 4)
	{
		if (!HexToInt(argv[3], &len, 32)){
			return false;
		}
	}else
		return false;

	if (!HexToInt(argv[2], &addr, 32)){
		return false;
	}

	if (!StrCmp(argv[1], "b8")){ //8bit read
		for(i=0; i<len; i++)
		{
			printf("\taddress : 0x%08lx    value : 0x%02x\n",addr, __REG8(addr));
			addr +=1;
		}
	}else if (!StrCmp(argv[1], "b16")){ //16bit read
		for(i=0; i<len; i++)
		{
			printf("\taddress : 0x%08lx    value : 0x%04x\n",addr, __REG16(addr));
			addr +=2;
		}
	}else if (!StrCmp(argv[1], "b32")){ //16bit read
		for(i=0; i<len; i++)
		{
			printf("\taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG32(addr));
			addr +=4;
		}
	}else {
		return false;
	}

	return true;
}
bool DoWrite (int argc, char **argv)
{
	//4byte 단위로 메모리, 래지스터에 값을 넣는다.
	//ex> write b8|b16|b32 0xa0000000 data

	u32 addr;
	u32 data;
	if (argc != 4)
		return false;

	if (!HexToInt(argv[2], &addr, 32)){
		return false;
	}

	if (!HexToInt(argv[3], &data, 32)){
		return false;
	}

	if (!StrCmp(argv[1], "b8")){ //8bit read
		printf("\tprevious value\n\taddress : 0x%08lx    value : 0x%02x\n\n",addr, __REG8(addr));
		__REG8(addr) = (u8)data;
		printf("\tchanged value\n\taddress : 0x%08lx    value : 0x%02x\n",addr, __REG8(addr));

	}else if (!StrCmp(argv[1], "b16")){ //16bit read
		printf("\tprevious value\n\taddress : 0x%08lx    value : 0x%04x\n\n",addr, __REG16(addr));
		__REG16(addr) = (u16)data;
		printf("\tchanged value\n\taddress : 0x%08lx    value : 0x%04x\n",addr, __REG16(addr));

	}else if (!StrCmp(argv[1], "b32")){ //16bit read
		printf("\tprevious value\n\taddress : 0x%08lx    value : 0x%08lx\n\n",addr, __REG32(addr));
		__REG32(addr) = (u32)data;
		printf("\tchanged value\n\taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG32(addr));
	}else
		return false;

	return true;

}
bool DoMacWrite( int argc, char **argv){

//	u8 mac[6];
	u8 macaddr[12] , addr_info[20];

	int i=0;
	int off=0;
	if (argc != 3){
		printf("Command error \n");
		return false;
	}

	for(i=0;i<20;i++)
		addr_info[i] =  __REG8(CONFIG_ROM_BASE + i);

	for(i=0;i<12;i++)
		macaddr[i] =  __REG8(CONFIG_ROM_BASE + i);

	printf("\tPrimary MAC Address\t: 0x");
	PrintEthAddr(&macaddr[0]);
	printf("\n");

	printf("\tSecondary MAC Address\t: 0x");
	PrintEthAddr(&macaddr[6]);
	printf("\n");

	if (!StrCmp(argv[1], "pri"))
		off = 0;
 	else if (!StrCmp(argv[1], "sec"))
		off = 6;
	else {
		printf("argv2 is not correct\n");
		return 1;
	}

	if ((argv[2][0] == '0') && (argv[2][1] == 'x'))   i=2;

	HexToInt(&argv[2][0], &macaddr[off+0], 8);
	HexToInt(&argv[2][2], &macaddr[off+1], 8);
	HexToInt(&argv[2][4], &macaddr[off+2], 8);
	HexToInt(&argv[2][6], &macaddr[off+3], 8);
	HexToInt(&argv[2][8], &macaddr[off+4], 8);
	HexToInt(&argv[2][10], &macaddr[off+5], 8);

	for(i=0;i<12;i++)
		addr_info[i]=macaddr[i];

	if (!EraseBlock((u32 *)CONFIG_ROM_BASE)){
		printf(" Erase Failed\n");
		return false;
	}

	if (!FlashWrite((u32 *)CONFIG_ROM_BASE,addr_info)){
		printf(" Write Failed\n");
		return false;
	}

	for(i=0;i<12;i++)
		macaddr[i] =  __REG8(CONFIG_ROM_BASE + i);

	printf("\tNew pri MAC Address\t: 0x");
	PrintEthAddr(&macaddr[0]);
	printf("\n");

	printf("\tNew sec MAC Address\t: 0x");
	PrintEthAddr(&macaddr[6]);
	printf("\n");
	
	eth_init();

	return true;
}
bool DoSetIP( int argc, char **argv){
	u8 clip[4];
	u8 hsip[4];
	u8 addr_info[20];
	u32  clientfullip, hostfullip;

	int i, class=0;

	if(argc !=3){
		return false;
	}

	for(i=0;i<12;i++)
		addr_info[i] =  __REG8(CONFIG_ROM_BASE + i);

	for(i=0;i<4;i++){
		clip[i] = 0; hsip[i]=0;
	}

	for(i=0;i<16;i++){
		if(argv[1][i] =='.'){
			class++;
			continue;
		}else if(argv[1][i] == '\0'){
			break;
		}

		hsip[class] *=10;
		hsip[class] += (argv[1][i] - '0');
	}

	class =0;
	for(i=0;i<16;i++){
		if(argv[2][i] =='.'){
			class++;
			continue;
		}else if(argv[2][i] == '\0'){
			break;
		}
		clip[class] *=10;
		clip[class] += (argv[2][i] - '0');
	}

	hostfullip &=0x00000000;
	hostfullip |= hsip[0] << 24;
	addr_info[12] = hsip[0];
	hostfullip |= hsip[1] << 16;
	addr_info[13] = hsip[1];
	hostfullip |= hsip[2] << 8;
	addr_info[14] = hsip[2];
	hostfullip |= hsip[3] ;
	addr_info[15] = hsip[3];
	hostfullip = SWAP32(hostfullip);
	hostIP=hostfullip;

	clientfullip &=0x00000000;
	clientfullip |= clip[0] << 24;
	addr_info[16] = clip[0];
	clientfullip |= clip[1] << 16;
	addr_info[17] = clip[1];
	clientfullip |= clip[2] << 8;
	addr_info[18] = clip[2];
	clientfullip |= clip[3] ;
	addr_info[19] = clip[3];
	clientfullip = SWAP32(clientfullip);
	clientIP=clientfullip;

	if (!EraseBlock((u32 *)CONFIG_ROM_BASE)){
		printf(" Erase Failed\n");
		return false;
	}

	if (!FlashWrite((u32 *)CONFIG_ROM_BASE,addr_info)){
		printf(" Write Failed\n");
		return false;
	}

	printf("\tHost   (server) IP       : ");
	PrintIPAddr(hostIP);
	printf("\n");

	printf("\tClient (target) IP       : ");
	PrintIPAddr(clientIP);
	printf("\n\n");

	return true;

}

bool DoStatus (int argc, char **argv)
{
	//현재의 H/W, S/W상태를 표시해 준다.
	u32 addr;
	u8 macaddr[12] , addr_info[20];
//	u8 *printaddr;
	int i;
	//메모리 맵을 표시해 준다.
	printf("\n\t* Memory Map *\n");
	printf("\tbootloader) ROM : 0x%08lx, RAM : 0x%08lx, LENGTH : 0x%08lx\n",(u32)BOOTLOADER_ROM_BASE, (u32)BOOTLOADER_RAM_BASE,(u32)BOOTLOADER_LENGTH);
	printf("\tkernel)     ROM : 0x%08lx, RAM : 0x%08lx, LENGTH : 0x%08lx\n",(u32)KERNEL_ROM_BASE, (u32)KERNEL_RAM_BASE, (u32)KERNEL_LENGTH);
	printf("\tramdisk)    ROM : 0x%08lx, RAM : 0x%08lx, LENGTH : 0x%08lx\n",(u32)RAMDISK_ROM_BASE, (u32)RAMDISK_RAM_BASE, (u32)RAMDISK_LENGTH);
	printf("\tusr)        ROM : 0x%08lx, RAM : 0x%08lx, LENGTH : 0x%08lx\n",(u32)USR_ROM_BASE, (u32)USR_RAM_BASE, (u32)USR_LENGTH);


	printf("\n\t* Network Status *\n");
	printf("\tHost      (server) Ethernet : 0x");
	PrintEthAddr(dmac);
//	PrintEthAddr(hostEther);
	printf("\n");

	for(i=0;i<20;i++)
		addr_info[i] =  __REG8(CONFIG_ROM_BASE + i);

	printf("\tHost      (server) IP       : ");
	PrintIPAddr(hostIP);
	printf("\n");

	printf("\tClient    (target) Ethernet : 0x");
	PrintEthAddr(clientEther);
	printf("\n");

	printf("\tClient    (target) IP       : ");
	PrintIPAddr(clientIP);
	printf("\n");

	for(i=0;i<12;i++)
		macaddr[i] =  __REG8(CONFIG_ROM_BASE + i);

	printf("\tPrimary   (target) Ethernet : 0x");
	PrintEthAddr(&macaddr[0]);
	printf("\n");

	printf("\tSecondary (target) Ethernet : 0x");
	PrintEthAddr(&macaddr[6]);
	printf("\n");

	printf("\n\t* Registers Value *\n");
	addr = MEM_CTL_BASE + MDCNFG;	printf("\tMDCNFG\taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MDREFR;	printf("\tMDREFR\taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MSC0;		printf("\tMSC0  \taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MSC1;		printf("\tMSC1  \taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MSC2;		printf("\tMSC2  \taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MECR;		printf("\tMECR  \taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + SXCNFG;	printf("\tSXCNFG\taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MCMEM0;	printf("\tMCMEM0\taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MCMEM1;	printf("\tMCMEM1\taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MCATT0;	printf("\tMCATT0\taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MCATT1;	printf("\tMCATT1\taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MCIO0;		printf("\tMCIO0  \taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MCIO1;		printf("\tMCIO1  \taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));
	addr = MEM_CTL_BASE + MDMRS;	printf("\tMDMRS \taddress : 0x%08lx    value : 0x%08lx\n",addr, __REG(addr));

	return true;
}

/* for fast s/w upgrade : by yhsong */
struct g_upgrade{
	int kernel;
	int ramdisk;
	int usr;
};
struct g_status{
	bool ip_st:1;
	bool kr_dn:1;
	bool rd_dn:1;
	bool ur_dn:1;
	bool kr_fl:1;
	bool rd_fl:1;
	bool ur_fl:1;
	bool mac_wr:1;
};
void print_banner(void);
void print_complete(void);
void print_ncomplete(void);
void print_usage(void);
char *print_ox(bool val);
void print_status(struct g_status status);
bool Cmd_Kernel(int xy);
bool Cmd_Ramdisk(int xy);
bool Cmd_Usr(int xy);
bool Cmd_Mac(int th, char *mac_ad);

bool DoUpgrade (int argc, char **argv)
{
	struct g_upgrade upg;
	struct g_status status;
	int i=0;
	int go = 1;
	char ch;

	upg.kernel  = 0;
	upg.ramdisk = 0;
	upg.usr     = 0;
	status.ip_st = 0;
	status.kr_dn = 0;
	status.rd_dn = 0;
	status.ur_dn = 0;
	status.kr_fl = 0;
	status.rd_fl = 0;
	status.ur_fl = 0;
	status.mac_wr= 0;
	
	print_banner();
	
	while((argv[1][0] == '-')){
		for(i = 0; i < StrLen(argv[1]) -1; i++){
			switch (argv[1][i+1]){
				case 'k':
					upg.kernel = 1;
					break;
				case 'r':
					upg.ramdisk = 1;
					break;
				case 'u':
					upg.usr	= 1;
					break;
				case 'a':
					upg.kernel = 1;
					upg.ramdisk = 1;
					upg.usr	= 1;
					break;
				default:
					printf("\n	Bad option [%s]\n",argv[1]);
					i+=100;
					break;
			}	
		}
		++argv;
		--argc;
	}

	if((argc < 3) || ( i>100)){
		print_usage();
		return false;
	}

	printf("	Selected Info	: kernel  [%d]\n",upg.kernel);
	printf("			: ramdisk [%d]\n",upg.ramdisk);
	printf("			: usr     [%d]\n",upg.usr);
	printf("	continue? Enter any key\n");

	while(go){
	if(GetChar(&ch))
		go = 0;
	}

	Cmd_Mac(0, "0000");

	DoBootp(1, 0);
	if(clientIP !=0){
		status.ip_st = 1;
	}

	if(status.ip_st){
		if(upg.kernel){
			status.kr_dn = Cmd_Kernel(1);
			user_wait(0x100000);
		}
		if(upg.ramdisk){
	       		status.rd_dn = Cmd_Ramdisk(1);
			user_wait(0x100000);
		}
		if(upg.usr){
			status.ur_dn = Cmd_Usr(1);
			user_wait(0x100000);
		}
	} else {
		print_status(status);
		print_ncomplete();
		return false;
	}

	if(upg.kernel && status.kr_dn){
		status.kr_fl = Cmd_Kernel(0);
		user_wait(0x100000);
	}
	if(upg.ramdisk && status.rd_dn){
        	status.rd_fl = Cmd_Ramdisk(0);
		user_wait(0x100000);
	}
	if(upg.usr && status.ur_dn){
		status.ur_fl = Cmd_Usr(0);
		user_wait(0x100000);
	}
	
	status.mac_wr = Cmd_Mac(0, argv[1]);
	status.mac_wr = Cmd_Mac(1, argv[2]);
	
	if(status.mac_wr){
		print_status(status);
		print_complete();
	}else {
		print_status(status);
		print_ncomplete();
		return false;
	}
	printf("\nPlease reset or boot !!\n");
	return true;
}

void print_banner()
{
	printf("	===============================\n");
	printf("	= HANBACK EMPOS2 S/W DOWNLOAD =\n");
	printf("	===============================\n");
}

void print_complete()
{
	printf("	===============================\n");
	printf("	= S/W UPGRAGE COMPLETED !!    =\n");
	printf("	===============================\n");
}

void print_ncomplete()
{
	printf("	===============================\n");
	printf("	= NOT COMPLETED !! SEE STATUS =\n");
	printf("	===============================\n");
}

void print_usage()
{
	printf("	===============================\n");
	printf("	= Usage	 :up [opt] p_mac s_mac=\n");
	printf("	= Options:		      =\n");
	printf("	=   -k kernel  (zImage)       =\n");
	printf("	=   -r ramdisk (ramdisk16.gz) =\n");
	printf("	=   -u usr     (qtopia.jffs)  =\n");
	printf("	=   -a all	    	      =\n");
	printf("	= Example:up  -rk   00a7 00a8 =\n");
	printf("	===============================\n");
}

char *print_ox(bool val){
	char *ox;

	if(val){
		ox ="OK ";
	} else{
		ox ="NOK";	
	}

	return (ox);
}

void print_status(struct g_status status)
{
	
	printf("	===============================\n");	
	printf("	=  S/W DOWNLOAD INFORMATION   =\n");
	printf("	===============================\n");
	printf("	= BOOTP			: %s =\n",print_ox(status.ip_st));
	printf("	= TFTP KERNEL		: %s =\n",print_ox(status.kr_dn));
	printf("	= TFTP RAMDISk		: %s =\n",print_ox(status.rd_dn));
	printf("	= TFTP USR		: %s =\n",print_ox(status.ur_dn));
	printf("	= FLASH KERNEL		: %s =\n",print_ox(status.kr_fl));
	printf("	= FLASH RAMDISK		: %s =\n",print_ox(status.rd_fl));
	printf("	= FLASH USR		: %s =\n",print_ox(status.ur_fl));
	printf("	= MAC WRITE		: %s =\n",print_ox(status.mac_wr));
	printf("	===============================\n");
}

bool Cmd_Kernel(int xy)
{
	char *cmd_kernel_dn[3];
	char *cmd_kernel_fl[2];
	
	cmd_kernel_dn[0] = "tftp";
	cmd_kernel_dn[1] = "zImage";
	cmd_kernel_dn[2] = "kernel";
	cmd_kernel_fl[0] = "flash";
	cmd_kernel_fl[1] = "kernel";

	if(xy){
		return(DoTftp(3, cmd_kernel_dn));
	}else {
		return(DoFlash(2, cmd_kernel_fl));	
	}
}

bool Cmd_Ramdisk(int xy)
{
	char *cmd_ramdisk_dn[3];
	char *cmd_ramdisk_fl[2];

	cmd_ramdisk_dn[0] = "tftp";
	cmd_ramdisk_dn[1] = "ramdisk16.gz";
	cmd_ramdisk_dn[2] = "ramdisk";
	cmd_ramdisk_fl[0] = "flash";
	cmd_ramdisk_fl[1] = "ramdisk";

	if(xy){
		return(DoTftp(3, cmd_ramdisk_dn));
	}else {
		return(DoFlash(2, cmd_ramdisk_fl));	
	}
}

bool Cmd_Usr(int xy)
{
	char *cmd_usr_dn[3];
	char *cmd_usr_fl[2];
	
	cmd_usr_dn[0] = "tftp";
	cmd_usr_dn[1] = "qtopia.jffs";
	cmd_usr_dn[2] = "usr";
	cmd_usr_fl[0] = "flash";
	cmd_usr_fl[1] = "usr";

	if(xy){
		return(DoTftp(3, cmd_usr_dn));
	}else {
		return(DoFlash(2, cmd_usr_fl));	
	}
}

bool Cmd_Mac(int th, char *mac_ad)
{
	char *cmd_mac_all[3];
	char mac_temp[20];
	
	cmd_mac_all[0] = "macwrite";
	StrCpy(mac_temp,"000ba803");

	if(th == 0){
		cmd_mac_all[1] = "pri";
	}else {
		cmd_mac_all[1] = "sec";
	}
	cmd_mac_all[2] = mac_temp;
	cmd_mac_all[2][8] = mac_ad[0];
	cmd_mac_all[2][9] = mac_ad[1];
	cmd_mac_all[2][10] = mac_ad[2];
	cmd_mac_all[2][11] = mac_ad[3];
	cmd_mac_all[2][12] = '\0';

	return(DoMacWrite(3,cmd_mac_all));
}
