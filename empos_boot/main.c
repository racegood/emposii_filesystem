/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 프로그램 설명 : 본 프로그램은 EMPOS_X255 (EMPOS II) 보드의 부트로더로 사용하기 위해 만들어 졌다.
//		   직접 코딩한 부분과, 여러 오픈 소스를 참조/수정/사용 하였으며, 본 프로그램 또한 오픈(GNU, GPL)되어 있다.
//
// 작 성 자       : 한백 전자 기술 연구소
// 저 작 권       : GNU/GPL
// 히스 토리      : 2003 년 7월 10일 작성
//                : 2005 년 9월 20일 수정
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
	char commandline[200];		//시리얼을 통해 입력되는 문자열을 이곳에 저장하였다가 파싱한다.

	SerialInit(); 				//serial을 초기화 한다.
	printf("\n\nThis is EMPOS_boot version 1.0\n");  // 부트로더 정보 출력
	printf("This is free software, and undered GNU GPL license\n");

	__REG(0x10100000) =0x9000;	//nport reset
	user_wait(100*556);
	__REG(0x10100000) =0x1f00;	//nport reset 
	user_wait(250*556);

	IoTest();				//Board의 각종 IO를 태스트 한다,
	eth_init();				//이더넷을 초기화 한다.

	/* 매모리 카피 */
	printf("\nCopy kernel from ROM to RAM ");
	MemCpy32((u32 *)KERNEL_RAM_BASE, (u32 *)KERNEL_ROM_BASE, KERNEL_LENGTH / 4);
	printf("\nCopy ramdisk from ROM to RAM ");
	MemCpy32((u32 *)RAMDISK_RAM_BASE, (u32 *)RAMDISK_ROM_BASE, RAMDISK_LENGTH / 4);

	/*ADD - 2005.08.25 : get ip addr from flash memory  */
	GetIPAddr();
	/* 오토 부팅을 결정  */
	printf("\nAutoboot in progress, press any key to stop.  ");

	for (i=0; i<BOOT_DELAY; i++){
		timeout = GetTime(&now)+1;	// 1초간 Delay
		printf("\b");
		printf("%d",BOOT_DELAY-i);
		while (GetTime(&now)<timeout){
			if ((__REG(FFUART_BASE + FFLSR) & 0x00000001)){
			__REG(FFUART_BASE + FFRBR); //입력된 값을 버퍼에서 가저옴으로써 처음에 프롭프트 출력을 막는다.
				autoboot = false;		//입력된 값이 있을경우 오토부트 FALSE
				break;
			}
		}
		if (autoboot==false) break;
	}

	if(autoboot)
		boot_kernel();	   // 커널 로딩


	//오토 부팅 FALSE일 경우
	printf("\n\n\t* Type 'help' to get a list of commands *");
	while(1){
		printf("\nEMPOS # ");
		if(GetCommand(&commandline[0]))	//명령어를 입력받는다.
		   RunCommand(&commandline[0]);	//입력받은 명령어가 있으면 명령을 실행한다. 커널 부팅만이 이부분을 빠져나갈수 있다.
	}
}

int GetCommand(char* commandline)
{
	char ch;
	int charnum=0;
	commandline[0]='\0';

	//2바이트로 전송되는 특수키 스켄 코드값 처리가 구현되지 않았다.
	while(1)
	{
		if (GetChar(&ch) ==1){

			/* 화면에 입력한 글자의 출력, 명령어 저장 */
			switch(ch){
			case 13 :	//Enter 가 입력 되었을 경우
				if(charnum>0){
					commandline[charnum]='\0';
					charnum=0;
					return 1;
				}else			//Enter가 들어왔으나 기존에 입력된 문자가
					return 0;	//없으므로 0을 리턴하여  프롭프트를 출력한다.
				break;

			case 8 :  	//백 스패이스가 입력될 경우 앞글자 하나를 지운다.
				if (charnum>0){
					PutChar(ch);PutChar(' ');PutChar(ch);
					charnum--;
					commandline[charnum] ='\0';
				}
				break;
			default :
				PutChar(ch); //일반 문자를 받았을 경우 그분자를 다시 화면에 출력한다.
				commandline[charnum] =ch;
				charnum++;
			}
		}
	}
}

void RunCommand(char* commandline)
{

	char *argv[10];	//최대 argc를 10까지 가질수 있다.
	char malloc[10][50];  //실제 전송되는 argv의 값을 가지고 잇다.
	int argc=0;
	int command_i=0, arg_i=0; //command_i = 전체 command line의 index, arg_i = 파싱된 argv에 관한 index 이므로
	int i=0;		       //command_i는 한 문자 파싱마다 계속 증가, arg_i는 argv 분리후 0으로 초기화 된다.

	for(i=0;i<10;i++)
		argv[i] = &malloc[i][0]; //2번째 주소 인덱스로 Pasing이 가능하도록 처음 포인터에 주소값을 넣어준다.
		 			  //이부분을 생략하면 컴파일시 에러는 나지 않으나 실행시 원하는 주소를 찾아갈수없다.

	while(1)			  //키보드로 입력받은 commandline을 argv로 분리하는 과정
	{
		if(commandline[command_i]=='\0') //명령어 끝까지 파싱이 완료되면 , break하여 실질적인 명령어를 실행한다.
		{
			argv[argc][arg_i]='\0';	//분리된 argv의 마지막에는 '\n'을 삽입하여 argv의 끝임을 나타낸다.
			argc++;

			break;
		}
		if(commandline[command_i] ==' '){ //공백 문자일경우 argv를 분리, argc증가
			if(arg_i ==0){
				command_i++;
				continue;
			}
			if (argv[argc][arg_i-1] ==' '){ //공백 문자가 연속으로 여러개 입력되었을 경우 하나만을 제외하고 모두 무시.
				command_i++;
				continue;
			}else{
				argv[argc][arg_i] = '\0'; //하나의 argv분리가 완료.
				argc++;
				arg_i=0;
			}
		}else{
			argv[argc][arg_i] = commandline[command_i]; //공백 문자가 아닐경우 argv입력값은 해당commandline값과 같다.

			command_i++;
			arg_i++;
		}

	}

	if(argc > 0)	//입력된 argv가 1나 이상일 경우 해당 명령어 실행.
	{
		bool suc =0;
		PutChar('\n');

		//아래의 모든 명령어가 현재 지원되고 있다. 명령어가 많지 않으므로 단순 비교를 사용한다.
		//거의 모든 명령어들은 commands.c에 구현되어 있다.
		if(!StrCmp(argv[0], "boot")){
			suc = DoBoot(argc, argv);		//커널을 부팅한다.

		}else if(!StrCmp(argv[0], "reset")){
			suc = DoReset(argc, argv);		//부트로더를 처음 0번지부터 다시 시작한다.

		}else if(!StrCmp(argv[0], "bootp")){
			suc = DoBootp(argc, argv);		//bootp를 통해 ip를 할당 받는다. bootp.c에 존재

		}else if(!StrCmp(argv[0], "setip")){
			suc = DoSetIP(argc, argv);		//호스트 pc와 보드의 ip를 설정할수 있다. 전원 휘발성이다.

		}else if(!StrCmp(argv[0], "macwrite")){
			suc = DoMacWrite(argc, argv);	//네트워크 칩의 mac주소를 변경할수 있다.

		}else if(!StrCmp(argv[0], "tftp")){
			suc = DoTftp(argc, argv);		//tftp를 통해 데이터를 전송받는다. tftp.c에 존재

		}else if(!StrCmp(argv[0], "download")){
			suc = DoDownLoad(argc, argv);	//커널을 zmodem을 통해 다운로드 한다.

		}else if(!StrCmp(argv[0], "flash")){
			suc = DoFlash(argc, argv);		//다운로드 받은 이미지를 플래쉬에 저장한다.

		}else if(!StrCmp(argv[0], "erase")){
			suc = DoErase(argc, argv);		//플래쉬 영역을 지운다

		}else if(!StrCmp(argv[0], "help")){
			suc = DoHelp(argc, argv);		//도움말을 출력한다.

		}else if(!StrCmp(argv[0], "read")){
			suc = DoRead(argc, argv);		//특정 메모리의 값을 프린트하다.

		}else if(!StrCmp(argv[0], "write")){
			suc = DoWrite(argc, argv);		//특정 메모리의 값을 바꿀수 있다.

		}else if(!StrCmp(argv[0], "status")){
			suc = DoStatus(argc, argv);		//S/W, H/W 상태를 출력한다.

		}else if(!StrCmp(argv[0], "test")){
			suc = DoTest(argc, argv);		//test 명령이 들어왔을 경우 DoTest를 실행 한다.
		}else if((!StrCmp(argv[0], "upgrade")||(!StrCmp(argv[0],"up")))){
			suc = DoUpgrade(argc, argv);		//test 명령이 들어왔을 경우 DoTest를 실행 한다.
		}else{
 			printf("\tBad command line...\n");   //일치하는 명령어가 없다.
			return;
		}
		/////////////////////////////////////////
		if(!suc)						//명령을 실행하였지만 정상적인 리턴이 이루어지지 않았을 경우.
 			printf("\tCommand failed...\n");
	}
}


void boot_kernel(void)
{
	void (*theKernel)(int zero, int arch) = (void (*)(int, int))KERNEL_RAM_BASE; //함수 포인터로 케스팅

	printf("\nStarting kernel...\n\n");

	theKernel(0, ARCH_NUMBER); 		// start kernel
	printf("\nKernel failed...\n\n"); 		//성공적으로 커널이 부팅하였을경우 이 문자는 출력되지 않는다.

}
