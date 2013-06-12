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
#include "cmd_program.h"

/***********************
 *    Type Define      *
 ***********************/

/**************************
 *  User Define Structure *
 **************************/


/***********************
 *    Static Value     *
 ***********************/
unsigned int	directoryStack[32];	/* Shall */
int				stackIndex = -1;			/* Shall */

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
void DIRCountValueSet_init ( struct DIRCountValueSet * aSet)
{
	aSet->dir	= 0;
	aSet->other = 0;
}

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
boolean parse_cmd (   char * msg_ ) 
{
	char cmd[Max_Length*4];// {'\n'};
	char * cur_cmd_PTR = cmd;
	char* buf = (char*)msg_;
	int i=0,cmd_Count=1;

	// msg를 분리합시다. 
	while ( (*buf) != '\0' ) {

		// 공백처리
		while ( (*buf) == ' ' ) {buf++;}

		for (;i<31 && (*buf) != '\0' && (*buf) != ' '; i++,buf++) {
			cur_cmd_PTR[i]=(*buf);
		}
		cur_cmd_PTR[i]='\0';
		
		cur_cmd_PTR = cmd + ( cmd_Count * Max_Length ) - StrLen(cmd);
		cmd_Count++;
	}
	if ( !StrCmp (cmd, "cd")) {
		cur_cmd_PTR = cmd+Max_Length;
		if ( debug ) printf ( "  - Directory is ==> [ %s ]\n", cur_cmd_PTR );
		cmd_cd ( cmd+Max_Length );
		return true;
	}
	else if ( !StrCmp ( cmd, "tree") ) {
		return cmd_tree( cmd+Max_Length );
	}
	else if ( !StrCmp ( cmd, "tree") ) {
		return cmd_tree ( cmd+Max_Length );
	}
	else if ( !StrCmp(cmd, "ls") ) {
		return cmd_ls ( cmd+Max_Length );
	}
	else if ( !StrCmp (cmd, "rmdir") ) {
		return cmd_rmdir ( cmd+Max_Length );
	}
	else if ( !StrCmp(cmd, "exit")) {		// exit - 프로그램을 종료한다. 
		return -1;
	}
	else if ( !StrCmp(cmd, "mkdir")) {
		return cmd_mkdir (cmd+Max_Length);
	}
	else if ( !StrCmp ( cmd, "mv" )) {
		return cmd_mv	( cmd+Max_Length, cmd+(Max_Length*2) );
	}
	else {
		if ( debug ) printf( " - Command not Found [%s]\n",cmd );
		return true;
	}
	return true;
}

void PrintFiles ()
{
	struct iNode * curNode = (struct iNode*)directoryStack[stackIndex];
	curNode = (struct iNode*)curNode->child;/* 현재 폴더의 SuperBlock을 가리키게 합니다. */

	for (;curNode;curNode = (struct iNode*)curNode->shibling) {
		if (curNode->flag != iNODE_FLAG_MASTER_BLOCK) {
			if ( curNode->flag == iNODE_FLAG_DIRECTORY ) {
				printf ( " %s/\t", curNode->File_Struct.name );
			}
		}
	}

	printf ( "\n" );
}
unsigned int getTopDirectoryStack ()
{
	return directoryStack[stackIndex];
}

unsigned int getDirectoryStack ( int index )
{
	return directoryStack[index];
}

boolean directoryStack_Clear ()
{
	int i = 1;

	for ( ; i <= stackIndex; i++ ) {
		directoryStack[i] = nil;
	}
	stackIndex=0;
	return true;
}

boolean cmd_mkdir (   char * msg_ )
{
	if (debug) printf ( " - cmd_mkdir :: %s\n", msg_ );
	if ( !CreateDirectory(msg_) ) return false;
	return true;
}

boolean cmd_ls (   char * msg_ ) 
{
	PrintFiles ();
	return true;
}

boolean cmd_rmdir	(   char * msg_ ) 
{
	RemoveDirectory(msg_);
	return true;
}

boolean cmd_cd (   char * msg_ ) 
{
	struct iNode * curNode = nil;
	int index=0;
	char aDir[32];
	MemSet ( aDir, '\0', 32 );

	if ( !StrLen(msg_) || msg_[0] == '/' ) {	// 아무것도 쓰지 않으면 루트로 이동
		directoryStack_Clear();
		if ( StrLen ( msg_ ) > 1 ) {
			msg_+=1;
			// Set directoryStack With Recursive
			while ( StrLen(msg_) ) 
			{
				while ( (*msg_) != '/' )
					aDir[index++] = (*msg_++);
				
				aDir[index] = '\0';
				msg_++;
				
				curNode = SearchNameWithCurrentiNode ( (struct iNode*)getTopDirectoryStack (), aDir );
				directoryStack[stackIndex++] = curNode;
			}			
		}
		return true;
	}

	if ( !StrNCmp (msg_, "..", 2) ) {	// .. 의 케이스
		directoryStack[stackIndex--] = nil;
		return true;
	}

	// Find Folder by Name 
	curNode = (struct iNode *)directoryStack[stackIndex];
	curNode = (struct iNode *)curNode->child;

	for ( ; curNode; curNode = (struct iNode *)curNode->shibling ) {
		if ( curNode->flag == iNODE_FLAG_DIRECTORY 
			&& !StrCmp(curNode->File_Struct.name,msg_ ) ) {
			// Add Stack
			directoryStack[++stackIndex] = (unsigned int) curNode;
			return true;
		} 
	}

	// Can't Find Folder 
	printf(" [%s] Cannot found folder \n", msg_);
	return true;
}

boolean cmd_mv	( char * name, char * destPath ) 
{
	return true;
}

boolean cmd_tree	(   char * msg_ ) 
{
	struct iNode * curNode = (struct iNode *)getTopDirectoryStack();
	struct DIRCountValueSet aSet;

	// Initialize aSet
	DIRCountValueSet_init(&aSet);

	// Get Super Block;
	curNode = (struct iNode *)curNode->child;


	printf ( ".\n" );


	// Trace Tree
	SuperBlock_PrintTree(curNode, 1, &aSet);

	// Print Count 
	printf ( "\n %d Directories, %d Files \n\n", aSet.dir, aSet.other );
	
	return true;
}

void SuperBlock_PrintTree ( struct iNode * aNode, int level, struct DIRCountValueSet * aSet_ ) 
{
	int i=0;
	int count = 0;
	for ( ;aNode; aNode = (struct iNode *)aNode->shibling ) {

		if ( aNode->flag == iNODE_FLAG_NON_DIRECTORY ) {
			aSet_->other++;
			for ( i = 1; i < level; i++ ) printf ( "    " );
			printf ( "---- %s\n", aNode->File_Struct.name );
		}
		else if ( aNode->flag == iNODE_FLAG_DIRECTORY ) {
			aSet_->dir++;
			for ( i = 1; i < level; i++ ) printf ( "    " );
			printf ( "---- %s\n", aNode->File_Struct.name );
		}
		if ( aNode->child)
			SuperBlock_PrintTree((struct iNode *)aNode->child,level+1, aSet_);
	}
	
}
