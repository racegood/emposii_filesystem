#ifndef __INODE_H__
#define __INODE_H__

/*******************************************
 *                                         *
 *        2013. 06. 03.                    *
 *        File System Proto Type           *
 *        FileName :                       * 
 *  Note :                                 *
 *                                         *
 *                                         *
 *    - End of Note -                      *
 *******************************************/
/***********************
 *   Include File      *
 ***********************/

/* Standard Library */

/* User Define Library */
#include "all.h"
#include "FileStructure.h"
/***********************
 *    Type Define      *
 ***********************/
#define iNODE_FLAG_NOT_USED			0
#define iNODE_FLAG_MASTER_BLOCK		1
#define iNODE_FLAG_DIRECTORY		2
#define iNODE_FLAG_NON_DIRECTORY	3

/**************************
 *  User Define Structure *
 **************************/
struct iNode{
	Addr head;			/* 해당 폴더의 Super_Block을 가리킴 */
	Addr child;			/* 폴더의 경우 자신의 Super_Block을 가리킴 */
	Addr shibling;		/* 현재 폴더 내의 다른 파일/폴더를 가리킴 */
	struct FileStructure File_Struct;	/* 실제 파일에 대한 정보가 들어있는 파일의 주소 */
	int	 flag;			/* iNode를 구분함 */
	Addr not_used;		/* 사용하지 않지만 나중에 쓸 공간 */
	Addr P_addr;
};

/***********************
 *    Static Value     *
 ***********************/

/**********************
 *  Function List     *
 **********************/

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
boolean iNode_Initialize ( struct iNode * aNode ) ;

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
struct iNode * SearchEmptyFileManager ( ) ;

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
boolean iNode_SetDirectory ( struct iNode * aNode );
boolean iNode_SetFile	 ( struct iNode * aNode );
/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
boolean iNode_SetSuperBlock ( struct iNode * aNode );

boolean iNode_SetDirectoryWithSuperBlock ( struct iNode * dNode, struct iNode * sbNode );

char * iNode_getName ( struct iNode * aNode );
boolean iNode_AddShibling ( struct iNode * , struct iNode * );
boolean iNode_Release	( struct iNode * );
boolean iNode_SetPhysicalAddress ( struct iNode * aNode, unsigned int addr_ ) ;
#endif
