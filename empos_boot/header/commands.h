#ident "$Id: commands.h$"

#ifndef SMART_COMMANDS_H
#define SMART_COMMANDS_H

// bootp, tftp등
// 네트워크 관련 명령어들은  해당 파일에 존재 한다

// 나머지 부트로더 명령어 들
bool DoTest (int argc, char** argv);	//사용자 태스트를 위해 만들어 놓은 함수
bool DoBoot (int argc, char **argv);	//커널 번지로 점프한다.
bool DoReset (int argc, char **argv);	//부트로더를 다시 시작한다.
bool DoDownLoad (int argc, char **argv);	//uuencode를 통해 이미지를 다운받는다.
bool DoFlash (int argc, char **argv);	//다운받은 이미지를 플래쉬에 저장 한다.
bool DoErase (int argc, char **argv);	//플래쉬에 저장된 내용을 지운다.
bool DoHelp (int argc, char **argv);	//부트로더 사용을 위한 help를 보여준다.
bool DoRead (int argc, char **argv); 	//메모리, 래지스터의 값을 읽는다
bool DoWrite (int argc, char **argv);	//메모리, 래지스터에 값을 써 넣는다.
bool DoStatus (int argc, char **argv);	//메모리, 래지스터에 값을 써 넣는다.
bool DoSetIP( int argc, char **argv);	//호스트 PC와 클라이언트 보드의 IP를 인위적으로 설정할수 있다.
bool DoMacWrite( int argc, char **argv);  //네트워크칩의 MAC주소를 저장할수 있다.
bool DoUpgrade (int argc, char **argv);
bool DoBootp (int argc, char **argv);
bool DoTftp (int argc, char **argv);



extern u8		broadcastEther[6];
extern u32		broadcastIP;
extern u8		clientEther[6];
extern u32		clientIP;
extern u8		hostEther[6];
extern u32		hostIP;			
extern short		protocol;

#endif

