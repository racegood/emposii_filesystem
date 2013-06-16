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
char			nameSet[][8] = {
		"name1", 	"name2",	"name3",	"name4",	"name5",
		"name6",	"name7",	"name8",	"name9",	"name10",
		NULL
};


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

boolean seperate_cmd ( char * cmd_, unsigned int * arr_ ) 
{	// arr_ 's Size - 4 
	int	cmd_count = 0;
	printf ( "Seperate_cmd Start = [%s] \n\n", cmd_ );
	while ( (*cmd_) != '\0' )
	{
		while((*cmd_) == ' ') cmd_++;
		arr_[cmd_count] = (unsigned int)cmd_;

		while((*cmd_) >= '0' && (*cmd_) <= 'z') cmd_++;
		(*cmd_) = '\0';

		cmd_count++;
		cmd_++;

	}
	return true;
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
	char *cmd_array[4];

	seperate_cmd ( msg_, cmd_array );


	if ( !StrCmp (cmd_array[0], "cd")) {
		cmd_cd ( cmd_array[1] );
		return true;
	}
	else if ( !StrCmp ( cmd_array[0], "fc" ) ) {
		return cmd_fc ( cmd_array[1], cmd_array[2]  );
	}
	else if ( !StrCmp ( cmd_array[0], "tree") ) {
		return cmd_tree( cmd_array[1] );
	}
	else if ( !StrCmp ( cmd_array[0], "tree") ) {
		return cmd_tree ( cmd_array[1] );
	}
	else if ( !StrCmp ( cmd_array[0], "ls") ) {
		return cmd_ls ( cmd_array[1] );
	}
	else if ( !StrCmp ( cmd_array[0], "rmdir") ) {
		return cmd_rmdir ( cmd_array[1] );
	}
	else if ( !StrCmp( cmd_array[0], "exit")) {		// exit - 프로그램을 종료한다. 
		return -1;
	}
	else if ( !StrCmp(cmd_array[0], "mkdir")) {
		return cmd_mkdir (cmd_array[1]);
	}
	else if ( !StrCmp ( cmd_array[0], "mv" )) {
		return cmd_mv	( cmd_array[1], cmd_array[2] );
	}
	else {
		if ( debug ) printf( " - Command not Found [%s]\n",cmd_array[0] );
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
			if ( curNode->flag == iNODE_FLAG_DIRECTORY )
				printf ( " %s/\t", curNode->File_Struct.name );
			else if ( curNode->flag == iNODE_FLAG_NON_DIRECTORY ) 
				printf ( " %s\t", curNode->File_Struct.name );
			
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

boolean cmd_fc	( char * path_, char * count_ ) 
{
	int count = StrToInt ( count_ ), i=0;
	char * filePath = "/";
	// Make File 
	printf ( "%s, %s\n", path_, count_ );

	// Path Check 
	if ( !StrLen(path_) ) 
	{
		printf ( "\n Required Path, Path Not Commandded \n\n"  );
		return false;
	}
	
	if ( count < 1 || count > 10 ) 
	{
		printf ( "\n Count Value Between 1 ~ 10 : %d\n\n", count ) ;
		return false;
	}

	printf ( " -- File Creation Start \n\n" ); 
	for ( i = 0; i < count; i++ ) 
	{
		printf("%s , %d , %s\n", nameSet[i], i, filePath);
		if ( ! CreateFile ( 0x01, 		/* Physical Address */
							nameSet[i],	/* File Name */
							1000*(i+1),			/* File Size */
							filePath	/* File Path */ 
							) ) 
		{
			printf ( " -- [Error] File Creation is Failed -- [%s] \n\n", nameSet[i] );
		}
							
	}

	return true;
}

boolean MoveDirectory ( char * msg_ ) { 
	struct iNode * curNode=nil,*aNode = (struct iNode*)getTopDirectoryStack ();
	int index =0;
	char aDir[32];
	printf ( "Move Directory [%s]\n", msg_);
	if ( StrLen ( msg_ ) > 1 ) {
		// Set directoryStack With Recursive
		while ( StrLen(msg_) > 0 ) 
		{
			index=0;
			MemSet ( aDir, '\0', 32 );
			while ( (*msg_) != '/') {
				if ((*msg_) == '\0');break;
				aDir[index++] = (*msg_++);
			}
			aDir[index] = '\0';
			printf ( "aDir [%s] \n", aDir );	
			msg_++;
			
			curNode = SearchNameWithCurrentiNode ( aNode, aDir );
			if ( curNode && curNode->flag == iNODE_FLAG_DIRECTORY ) 
			{
				directoryStack[stackIndex++] = (unsigned int)curNode;
				aNode = curNode;
			} else {
				return true;
			}
		}			
	}
	return false;
}

boolean cmd_cd (   char * msg_ ) 
{
	int index=0;

	if ( !StrLen(msg_) || msg_[0] == '/' ) 
	{	// 아무것도 쓰지 않으면 루트로 이동
		directoryStack_Clear();
		return MoveDirectory ( msg_+1 );
	}

	if ( !StrNCmp (msg_, "..", 2) ) 
	{	// .. 의 케이스
		if ( stackIndex > 0 )
			directoryStack[stackIndex--] = nil;
		return true;
	}

	index =	MoveDirectory ( msg_ );
	// Can't Find Folder 
	if ( !index ) printf(" [%s] Cannot found folder \n", msg_);
	return true;
}

boolean cmd_mv	( char * name, char * destPath ) 
{
	struct iNode * aNode	= RemoveiNodeWithName ( (struct iNode *)directoryStack[stackIndex], name );
	struct iNode * destNode	= FindFolderWithPath ( destPath );
	
	if ( !aNode ) 
	{
		printf ( " [%s] Directory is Not Found \n", name );
		return false;
	}

	if ( !destNode ) 
	{
		printf ( " [%s] Destination Folder is Not Found \n", destPath );
	}

	return iNode_AddShibling ( destNode, aNode );
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
