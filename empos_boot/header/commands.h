#ident "$Id: commands.h$"

#ifndef SMART_COMMANDS_H
#define SMART_COMMANDS_H

// bootp, tftp��
// ��Ʈ��ũ ���� ��ɾ����  �ش� ���Ͽ� ���� �Ѵ�

// ������ ��Ʈ�δ� ��ɾ� ��
bool DoTest (int argc, char** argv);	//����� �½�Ʈ�� ���� ����� ���� �Լ�
bool DoBoot (int argc, char **argv);	//Ŀ�� ������ �����Ѵ�.
bool DoReset (int argc, char **argv);	//��Ʈ�δ��� �ٽ� �����Ѵ�.
bool DoDownLoad (int argc, char **argv);	//uuencode�� ���� �̹����� �ٿ�޴´�.
bool DoFlash (int argc, char **argv);	//�ٿ���� �̹����� �÷����� ���� �Ѵ�.
bool DoErase (int argc, char **argv);	//�÷����� ����� ������ �����.
bool DoHelp (int argc, char **argv);	//��Ʈ�δ� ����� ���� help�� �����ش�.
bool DoRead (int argc, char **argv); 	//�޸�, ���������� ���� �д´�
bool DoWrite (int argc, char **argv);	//�޸�, �������Ϳ� ���� �� �ִ´�.
bool DoStatus (int argc, char **argv);	//�޸�, �������Ϳ� ���� �� �ִ´�.
bool DoSetIP( int argc, char **argv);	//ȣ��Ʈ PC�� Ŭ���̾�Ʈ ������ IP�� ���������� �����Ҽ� �ִ�.
bool DoMacWrite( int argc, char **argv);  //��Ʈ��ũĨ�� MAC�ּҸ� �����Ҽ� �ִ�.
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

