#ifndef __CMD_PROGRAM_H__
#define __CMD_PROGRAM_H__

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
#include "iNode.h"
#include "system_kernel.h"
/***********************
 *    Type Define      *
 ***********************/

/**************************
 *  User Define Structure *
 **************************/
struct DIRCountValueSet {
	int dir;
	int other;
};

/***********************
 *    Static Value     *
 ***********************/

/**********************
 *  Function List     *
 **********************/
void DIRCountValueSet_init ( struct DIRCountValueSet * );
/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
boolean parse_cmd  (   char * msg_ ) ;
boolean cmd_mkdir	(   char * msg_ ) ;
boolean cmd_ls		(   char * msg_ ) ;
boolean cmd_cd		(   char * msg_ ) ;
boolean cmd_rmdir	(   char * msg_ ) ;
boolean cmd_tree	(   char * msg_ ) ;
boolean cmd_mv		( char *, char * );
boolean cmd_fc		( char *, char * );
void	PrintFiles ();
boolean directoryStack_Clear ();
unsigned int getTopDirectoryStack ();
unsigned int getDirectoryStack ( int index );
void SuperBlock_PrintTree ( struct iNode * aNode, int level, struct DIRCountValueSet * aSet_ ) ;
#endif
