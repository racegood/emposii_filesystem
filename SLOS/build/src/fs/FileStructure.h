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
	char name[16];		/* 파일의 이름 */
	int fs_type;		/* 파일의 타입 */
	int fs_size;		/* 파일의 크기 */
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
boolean FileStructure_SetFile ( struct FileStructure *, char *, int );
boolean FileStruct_Initialize ( struct FileStructure * aFile_ ) ;
#endif
