#ifndef __FILESTRUCTURE_H__
#define __FILESTRUCTURE_H__


/*******************************************
 *                                         *
 *        2013. 06. 03.                    *
 *        File System Proto Type           *
 *        FileName : FileStructure.h       * 
 *  Note :                                 *
 *     File Structure Class Define         *
 *                                         *
 *    - End of Note -                      *
 *******************************************/
/***********************
 *   Include File      *
 ***********************/

/* Standard Library */

/* User Define Library */
#include "all.h"

/***********************
 *    Type Define      *
 ***********************/

#define FILE_STRUCTURE_DIRECTORY	 0
#define FILE_STRUCTURE_NON_DIRECTORY 1

/**************************
 *  User Define Structure *
 **************************/

struct FileStructure {
	char name[16];		/* ������ �̸� */
	int fs_type;		/* ������ Ÿ�� */
	int fs_size;		/* ������ ũ�� */
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
boolean FileStructure_SetDirectoryWithName ( struct FileStructure * fs_, char * name_ );

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
boolean FileStructure_SetNonDirectoryWithName ( struct FileStructure * fs_, char * name_ );

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
boolean FileStruct_Initialize ( struct FileStructure * aFile_ ) ;
#endif
