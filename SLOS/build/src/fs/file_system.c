/*******************************************
 *                                         *
 *        2013. 06. 03.                    *
 *         File System Proto Type          *
 *                                         * 
 *******************************************/
/***********************
 *   Include File      *
 ***********************/

// User Define Library
#include "FileStructure.h"
#include "iNode.h"
#include "system_kernel.h"
#include "cmd_program.h"
#include "all.h"

/***********************
 *    Type Define      *
 ***********************/

/**************************
 *  User Define Structure *
 **************************/

/***********************
 *    Static Value     *
 ***********************/
/**********************
 *  Function List     *
 **********************/

boolean Initialize (  ) ;

char * directoryPath () ;

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
int file_system_start ()
{
	char buf[1024];//{0};
	int count=0;

	// Initialize
	if ( !Initialize() ) {
		// 프로그램 초기화 실패 
		printf ( "\n\n *** [error] Program Initialize failed...\n\n" );
		return -1;
	}


	do {
		count=0;
		// Input Instruction;
		printf ( "MFSystem_%s/>", directoryPath() );
//		while ( (buf[count++] = GetChar()) != '\n' ) ;
		

		buf[count-1]='\0';
		// 명령어 처리 루틴 
		if (parse_cmd(buf) == -1) break;
	} while ( 1 );

	printf ( " ...Program terminate... \n");
	return 0;
}


boolean Initialize ( ) 
{
	printf ( " *************************** \n" ) ;
	printf ( " *  Welcome to File System * \n" ) ;
	printf ( " *************************** \n\n" ) ;

	// 0. Debug Mode Enabled 
	if ( debug ) printf ( " - [warning!!] debug mode enabled \n\n" );

	// 1. FileManager_Initialize
	if ( !FileManager_Initialize () ) return false;

	// 2. Set Root Node;
	if ( !setHeader () ) return false;

	return true;
}
char * directoryPath () 
{
	return iNode_getName((struct iNode *)getTopDirectoryStack() );
}

