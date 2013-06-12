/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ���α׷� ���� : �� ���α׷��� EMPOS_X255 (EMPOS II) ������ ��Ʈ�δ��� ����ϱ� ���� ����� ����.
//		   ���� �ڵ��� �κа�, ���� ���� �ҽ��� ����/����/��� �Ͽ�����, �� ���α׷� ���� ����(GNU, GPL)�Ǿ� �ִ�.
//
// �� �� ��       : �ѹ� ���� ��� ������
// �� �� ��       : GNU/GPL
// ���� �丮      : 2003 �� 7�� 10�� �ۼ�
//                : 2005 �� 9�� 20�� ����
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <config.h>
#include <serial.h>
#include <time.h>
#include <lib.h>
#include <reg.h>
#include <commands.h>
#include <main.h>
#include <net.h>
#include <bootp.h>
#include <tftp.h>
#include <smc91111.h>

void c_main(void)
{
	time_t now;
	int i;
	int autoboot=1;
	int timeout;
	char commandline[200];		//�ø����� ���� �ԷµǴ� ���ڿ��� �̰��� �����Ͽ��ٰ� �Ľ��Ѵ�.

	SerialInit(); 				//serial�� �ʱ�ȭ �Ѵ�.
	printf("\n\nThis is EMPOS_boot version 1.0\n");  // ��Ʈ�δ� ���� ���
	printf("This is free software, and undered GNU GPL license\n");

	__REG(0x10100000) =0x9000;	//nport reset
	user_wait(100*556);
	__REG(0x10100000) =0x1f00;	//nport reset 
	user_wait(250*556);

	IoTest();				//Board�� ���� IO�� �½�Ʈ �Ѵ�,
	eth_init();				//�̴����� �ʱ�ȭ �Ѵ�.

	/* �Ÿ� ī�� */
	printf("\nCopy kernel from ROM to RAM ");
	MemCpy32((u32 *)KERNEL_RAM_BASE, (u32 *)KERNEL_ROM_BASE, KERNEL_LENGTH / 4);
	printf("\nCopy ramdisk from ROM to RAM ");
	MemCpy32((u32 *)RAMDISK_RAM_BASE, (u32 *)RAMDISK_ROM_BASE, RAMDISK_LENGTH / 4);

	/*ADD - 2005.08.25 : get ip addr from flash memory  */
	GetIPAddr();
	/* ���� ������ ����  */
	printf("\nAutoboot in progress, press any key to stop.  ");

	for (i=0; i<BOOT_DELAY; i++){
		timeout = GetTime(&now)+1;	// 1�ʰ� Delay
		printf("\b");
		printf("%d",BOOT_DELAY-i);
		while (GetTime(&now)<timeout){
			if ((__REG(FFUART_BASE + FFLSR) & 0x00000001)){
			__REG(FFUART_BASE + FFRBR); //�Էµ� ���� ���ۿ��� ���������ν� ó���� ������Ʈ ����� ���´�.
				autoboot = false;		//�Էµ� ���� ������� �����Ʈ FALSE
				break;
			}
		}
		if (autoboot==false) break;
	}

	if(autoboot)
		boot_kernel();	   // Ŀ�� �ε�


	//���� ���� FALSE�� ���
	printf("\n\n\t* Type 'help' to get a list of commands *");
	while(1){
		printf("\nEMPOS # ");
		if(GetCommand(&commandline[0]))	//��ɾ �Է¹޴´�.
		   RunCommand(&commandline[0]);	//�Է¹��� ��ɾ ������ ����� �����Ѵ�. Ŀ�� ���ø��� �̺κ��� ���������� �ִ�.
	}
}

int GetCommand(char* commandline)
{
	char ch;
	int charnum=0;
	commandline[0]='\0';

	//2����Ʈ�� ���۵Ǵ� Ư��Ű ���� �ڵ尪 ó���� �������� �ʾҴ�.
	while(1)
	{
		if (GetChar(&ch) ==1){

			/* ȭ�鿡 �Է��� ������ ���, ��ɾ� ���� */
			switch(ch){
			case 13 :	//Enter �� �Է� �Ǿ��� ���
				if(charnum>0){
					commandline[charnum]='\0';
					charnum=0;
					return 1;
				}else			//Enter�� �������� ������ �Էµ� ���ڰ�
					return 0;	//�����Ƿ� 0�� �����Ͽ�  ������Ʈ�� ����Ѵ�.
				break;

			case 8 :  	//�� �����̽��� �Էµ� ��� �ձ��� �ϳ��� �����.
				if (charnum>0){
					PutChar(ch);PutChar(' ');PutChar(ch);
					charnum--;
					commandline[charnum] ='\0';
				}
				break;
			default :
				PutChar(ch); //�Ϲ� ���ڸ� �޾��� ��� �׺��ڸ� �ٽ� ȭ�鿡 ����Ѵ�.
				commandline[charnum] =ch;
				charnum++;
			}
		}
	}
}

void RunCommand(char* commandline)
{

	char *argv[10];	//�ִ� argc�� 10���� ������ �ִ�.
	char malloc[10][50];  //���� ���۵Ǵ� argv�� ���� ������ �մ�.
	int argc=0;
	int command_i=0, arg_i=0; //command_i = ��ü command line�� index, arg_i = �Ľ̵� argv�� ���� index �̹Ƿ�
	int i=0;		       //command_i�� �� ���� �Ľ̸��� ��� ����, arg_i�� argv �и��� 0���� �ʱ�ȭ �ȴ�.

	for(i=0;i<10;i++)
		argv[i] = &malloc[i][0]; //2��° �ּ� �ε����� Pasing�� �����ϵ��� ó�� �����Ϳ� �ּҰ��� �־��ش�.
		 			  //�̺κ��� �����ϸ� �����Ͻ� ������ ���� ������ ����� ���ϴ� �ּҸ� ã�ư�������.

	while(1)			  //Ű����� �Է¹��� commandline�� argv�� �и��ϴ� ����
	{
		if(commandline[command_i]=='\0') //��ɾ� ������ �Ľ��� �Ϸ�Ǹ� , break�Ͽ� �������� ��ɾ �����Ѵ�.
		{
			argv[argc][arg_i]='\0';	//�и��� argv�� ���������� '\n'�� �����Ͽ� argv�� ������ ��Ÿ����.
			argc++;

			break;
		}
		if(commandline[command_i] ==' '){ //���� �����ϰ�� argv�� �и�, argc����
			if(arg_i ==0){
				command_i++;
				continue;
			}
			if (argv[argc][arg_i-1] ==' '){ //���� ���ڰ� �������� ������ �ԷµǾ��� ��� �ϳ����� �����ϰ� ��� ����.
				command_i++;
				continue;
			}else{
				argv[argc][arg_i] = '\0'; //�ϳ��� argv�и��� �Ϸ�.
				argc++;
				arg_i=0;
			}
		}else{
			argv[argc][arg_i] = commandline[command_i]; //���� ���ڰ� �ƴҰ�� argv�Է°��� �ش�commandline���� ����.

			command_i++;
			arg_i++;
		}

	}

	if(argc > 0)	//�Էµ� argv�� 1�� �̻��� ��� �ش� ��ɾ� ����.
	{
		bool suc =0;
		PutChar('\n');

		//�Ʒ��� ��� ��ɾ ���� �����ǰ� �ִ�. ��ɾ ���� �����Ƿ� �ܼ� �񱳸� ����Ѵ�.
		//���� ��� ��ɾ���� commands.c�� �����Ǿ� �ִ�.
		if(!StrCmp(argv[0], "boot")){
			suc = DoBoot(argc, argv);		//Ŀ���� �����Ѵ�.

		}else if(!StrCmp(argv[0], "reset")){
			suc = DoReset(argc, argv);		//��Ʈ�δ��� ó�� 0�������� �ٽ� �����Ѵ�.

		}else if(!StrCmp(argv[0], "bootp")){
			suc = DoBootp(argc, argv);		//bootp�� ���� ip�� �Ҵ� �޴´�. bootp.c�� ����

		}else if(!StrCmp(argv[0], "setip")){
			suc = DoSetIP(argc, argv);		//ȣ��Ʈ pc�� ������ ip�� �����Ҽ� �ִ�. ���� �ֹ߼��̴�.

		}else if(!StrCmp(argv[0], "macwrite")){
			suc = DoMacWrite(argc, argv);	//��Ʈ��ũ Ĩ�� mac�ּҸ� �����Ҽ� �ִ�.

		}else if(!StrCmp(argv[0], "tftp")){
			suc = DoTftp(argc, argv);		//tftp�� ���� �����͸� ���۹޴´�. tftp.c�� ����

		}else if(!StrCmp(argv[0], "download")){
			suc = DoDownLoad(argc, argv);	//Ŀ���� zmodem�� ���� �ٿ�ε� �Ѵ�.

		}else if(!StrCmp(argv[0], "flash")){
			suc = DoFlash(argc, argv);		//�ٿ�ε� ���� �̹����� �÷����� �����Ѵ�.

		}else if(!StrCmp(argv[0], "erase")){
			suc = DoErase(argc, argv);		//�÷��� ������ �����

		}else if(!StrCmp(argv[0], "help")){
			suc = DoHelp(argc, argv);		//������ ����Ѵ�.

		}else if(!StrCmp(argv[0], "read")){
			suc = DoRead(argc, argv);		//Ư�� �޸��� ���� ����Ʈ�ϴ�.

		}else if(!StrCmp(argv[0], "write")){
			suc = DoWrite(argc, argv);		//Ư�� �޸��� ���� �ٲܼ� �ִ�.

		}else if(!StrCmp(argv[0], "status")){
			suc = DoStatus(argc, argv);		//S/W, H/W ���¸� ����Ѵ�.

		}else if(!StrCmp(argv[0], "test")){
			suc = DoTest(argc, argv);		//test ����� ������ ��� DoTest�� ���� �Ѵ�.
		}else if((!StrCmp(argv[0], "upgrade")||(!StrCmp(argv[0],"up")))){
			suc = DoUpgrade(argc, argv);		//test ����� ������ ��� DoTest�� ���� �Ѵ�.
		}else{
 			printf("\tBad command line...\n");   //��ġ�ϴ� ��ɾ ����.
			return;
		}
		/////////////////////////////////////////
		if(!suc)						//����� �����Ͽ����� �������� ������ �̷������ �ʾ��� ���.
 			printf("\tCommand failed...\n");
	}
}


void boot_kernel(void)
{
	void (*theKernel)(int zero, int arch) = (void (*)(int, int))KERNEL_RAM_BASE; //�Լ� �����ͷ� �ɽ���

	printf("\nStarting kernel...\n\n");

	theKernel(0, ARCH_NUMBER); 		// start kernel
	printf("\nKernel failed...\n\n"); 		//���������� Ŀ���� �����Ͽ������ �� ���ڴ� ��µ��� �ʴ´�.

}
