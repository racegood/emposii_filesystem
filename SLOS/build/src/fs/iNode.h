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
	Addr head;			/* �ش� ������ Super_Block�� ����Ŵ */
	Addr child;			/* ������ ��� �ڽ��� Super_Block�� ����Ŵ */
	Addr shibling;		/* ���� ���� ���� �ٸ� ����/������ ����Ŵ */
	struct FileStructure File_Struct;	/* ���� ���Ͽ� ���� ������ ����ִ� ������ �ּ� */
	int	 flag;			/* iNode�� ������ */
	Addr not_used;		/* ������� ������ ���߿� �� ���� */
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
