#ifndef __SYSTEM_KERNEL_H__
#define __SYSTEM_KERNEL_H__

/*******************************************
 *                                         *
 *        2013. 06. 03.                    *
 *        File System Proto Type           *
 *        FileName : System_Kernel         * 
 *  Note :                                 *
 *                                         *
 *                                         *
 *    - End of Note -                      *
 *******************************************/
/***********************
 *   Include File      *
 ***********************/
#include "all.h"
#include "iNode.h"
/* Standard Library */

/* User Define Library */

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

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
boolean FileManager_Initialize (  ) ;
unsigned int CreateDirectory ( char * name_ ) ;
struct iNode * SearchEmptyFileManager ( ) ;
boolean setHeader  ( ) ;
struct iNode * SearchWithName ( char *name ) ;
unsigned int RemoveDirectory ( char * name ) ;
boolean FindInsideDirectory ( struct iNode * ) ;
boolean LoadDirectoryStructure ( );
struct iNode * SearchNameWithCurrentiNode ( struct iNode *, char * );
boolean SearchFileWithName ( char *, char* );
struct iNode * FindFolderWithPath ( char * );
#endif
