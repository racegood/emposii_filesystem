#include "system_kernel.h"
extern int stackIndex;
extern int directoryStack[];

unsigned int	header=nil;			/* System */
struct iNode	FileManager[Max_Size];		/* System */ 

boolean FileManager_Initialize ( ) 
{
	int i=0;

	for ( i = 0; i < Max_Size; i++ ) {
		if ( !iNode_Initialize( &FileManager[i] ) ) return false ;
	}

	if ( debug ) printf ( " - FileManager_Initialize () \n\n" );
	return true;
}
struct iNode * SearchEmptyFileManager ( ) 
{
	int i = 0;
	for ( i = 0; i < Max_Size; i++ ) {
		/* 사용하지 않은 iNode를 찾아서 반환해 준다. */
		if ( FileManager[i].flag == iNODE_FLAG_NOT_USED ) return &(FileManager[i]);
	}

	return nil;		/* Failed.. */
}
unsigned int CreateDirectory ( char * name_ ) 
{
	struct iNode * newNode = nil, *sbNode = nil;

	printf("CreateDirectory call !\n");
	if (!StrLen(name_)) {
		printf(" ** [Error] argument error \n");
		return nil;
	}

	if ( header && SearchWithName(name_) ) {
		printf(" ** [Error] already existed \n");
		return nil;
	}

	if ( (newNode = SearchEmptyFileManager()) == nil ) return nil;

	
	printf ( " Set Directory iNode \n" );
	if ( !iNode_SetDirectory(newNode) ) return nil;

	printf ( "SetDirectoryWithName\n" );
	if ( !FileStructure_SetDirectoryWithName( &(newNode->File_Struct), name_) ) return nil;

	printf ( "Set Directory Stack \n" );
	if ( !header ) {	// Root 디렉터리 설정
		header = (unsigned int)newNode;
		stackIndex = 0;
		directoryStack[stackIndex] = header;
	} else {			
		if ( !iNode_AddShibling((struct iNode*)directoryStack[stackIndex], newNode) ) return nil;
	}

	// Super Block Init;
	if ( (sbNode = SearchEmptyFileManager()) == nil ) return nil;

	// Cunnect Directory & Super Block 
	if ( !iNode_SetDirectoryWithSuperBlock( newNode,sbNode )) return nil;


	return (unsigned int)newNode;
}

unsigned int RemoveDirectory ( char * name )
{
	struct iNode * aNode=nil;
	struct iNode * tempNode =nil;

	if (!strlen(name)) {
		printf(" ** [Error] argument error \n");
		return nil;
	}

	if ( header && !(aNode = SearchWithName(name)) ) {
		printf(" ** [Error] Not existed this folder\n");
		return nil;
	}

	aNode = (struct iNode*)directoryStack[stackIndex];
	tempNode = aNode = (struct iNode*)aNode->child;
	for ( ; aNode;
		tempNode = aNode,
		aNode = (struct iNode*)aNode->shibling ) {
		if ( aNode->flag == iNODE_FLAG_DIRECTORY 
			&& !StrCmp(aNode->File_Struct.name,name) ) {

			// 해당 노드의 폴더 내 검사
				// 실패 시 동작안함
			if ( FindInsideDirectory(aNode) ) {
				return nil;
			}

			// 링크 이어주기 
			// tempNode : 이전 노드 
			// aNode : 현제 노드 
			tempNode->shibling = aNode->shibling;

			// 메모리 반환 
			return iNode_Release(aNode);
		}
	}
			


	return nil;
}
boolean setHeader  ( ) 
{
	if ( !LoadDirectoryStructure () ) {
		header = CreateDirectory ( "root" );
	}
	return true;
}

struct iNode * SearchWithName ( char *name ) 
{
	struct iNode * curNode = nil;
	curNode = (struct iNode*)directoryStack[stackIndex];
	curNode = (struct iNode*)curNode->child;

	printf("SearchWithName call!!, curNode : %x\n", curNode);

	for ( ; curNode; curNode = (struct iNode *)curNode->shibling ){
		if ( curNode->flag != iNODE_FLAG_NOT_USED 
			&& curNode->flag != iNODE_FLAG_MASTER_BLOCK ) {
			if ( !StrCmp (name,curNode->File_Struct.name) ) {
				return curNode;
			}
		}
	}

	printf("return nil\n");
	return nil;
}

struct iNode * SearchNameWithCurrentiNode ( struct iNode * aNode, char *name )
{
	struct iNode * curNode = nil;
	curNode = (struct iNode *)aNode->child;
	printf ( "Search Name With CurrentiNode \n\n;" ) ;
	for ( ; curNode; curNode = (struct iNode *)curNode->shibling ) 
	{
		if ( curNode->flag != iNODE_FLAG_NOT_USED
			&& curNode->flag != iNODE_FLAG_MASTER_BLOCK ) 
		{
			if ( !StrCmp ( name, curNode->File_Struct.name ) )
				return curNode;
		}
	}
	return nil;
}

struct iNode * RemoveiNodeWithName ( struct iNode * aNode, char * name_ )
{
	struct iNode * curNode = nil, *tempNode=nil;
	curNode = (struct iNode *)aNode->child;

	for ( ; curNode; curNode = (struct iNode*)curNode->shibling )
	{
		if ( curNode->flag != iNODE_FLAG_NOT_USED
			&& curNode->flag != iNODE_FLAG_MASTER_BLOCK ) 
		{
			if ( !StrCmp ( name_, curNode->File_Struct.name ) )
			{
				if ( tempNode ) 
				{
					tempNode->shibling = curNode->shibling;
				}	
				curNode->shibling = nil;
				return curNode;
			}
		}
	}
	return nil;
}

boolean SearchFileWithName ( char * filePath_, char * fileName_ )
{
	struct iNode * currentNode=nil;	

	// Devide File Path 
	// Go File Path
	currentNode = FindFolderWithPath ( filePath_ );

	// Get A File 
	currentNode = SearchNameWithCurrentiNode ( currentNode, fileName_ );

	if ( !currentNode ) true;
	else false;
}

struct iNode * FindFolderWithPath ( char * filePath_ )
{
	char curFolder[32];
	int  index=0;
	int  curStackIndex =0;
	struct iNode * curStack=getTopDirectoryStack();

	printf ( "  Initialize To Buffer \n" );
	MemSet ( curFolder, '\0', 32 );

	if ( !StrLen ( filePath_ ) ) 
	{
		printf ( " filePath_ is not ..[%s]\n",filePath_ ) ;
		return nil;
	}
	
	if ( !StrNCmp ( filePath_, ".", 1 ) ) return curStack;
	printf("filepath: %s\n", filePath_);
	while ( (*filePath_) != '\0' ) 
	{
		// Seperate Path
		while ( (*filePath_) != '/' && (*filePath_) != '\0' ) 
		{
			curFolder[index++] = (*filePath_++);
		}
		curFolder[index] = '\0';
		filePath_++;
		if ( !index ) curStack = &(FileManager[0]);
		else 
		{
			printf ( " FindFolderWithPath - index is safe \n" ) ;
			curStack = SearchNameWithCurrentiNode ( curStack, curFolder );	
			if ( !curStack ) return nil;
			switch ( curStack->flag ) 
			{
				case iNODE_FLAG_DIRECTORY:
					continue;
					break;
				case iNODE_FLAG_NON_DIRECTORY:
					return curStack;
					break;
				default:	;
			}
		}
	}
	return curStack;
}

boolean FindInsideDirectory ( struct iNode * aNode_ ) 
{
	struct iNode * sbNode = (struct iNode *)aNode_->child;
	return (boolean)(aNode_->shibling);
}
boolean LoadDirectoryStructure ( )
{
	// 특정 메모리를 검사해서 File Structure를 구축함
	return false;
}

boolean CreateFile ( Addr P_addr, char * fileName_, int size_, char * path_ )
{
	struct iNode * aNode=nil;
	struct iNode * newNode=nil;
	
	// Pointer Check
	printf("pointer check\n");
	aNode = FindFolderWithPath ( path_ );
	if ( !aNode ) 
	{
		printf ( " Not Found This Path -- [%s] \n", path_ );
		return false;
	}
	
	printf(" File Name is Exist Check \n");
	if ( SearchNameWithCurrentiNode ( aNode, fileName_ ) ) 
	{
		printf ( " [%s] is Exist This Folder -> [%s] \n\n", fileName_, iNode_getName (aNode) );
		return false;
	}

	printf(" Get a New iNode  \n");
	if ( !(newNode = SearchEmptyFileManager ( )) )
	{
		printf ( " FileManager is Full ! \n" );
		return false;
	}
	printf(" Set iNode by File  \n");
	if ( !iNode_SetFile ( newNode ) ) return false;
	
	printf(" Set File Structure \n");
	if ( !FileStructure_SetFile ( &(newNode->File_Struct), fileName_, size_ ) )
		return false;

	printf(" Set Physical Address to iNode \n");
	if ( !iNode_SetPhysicalAddress ( newNode, P_addr ) ) return false;

	printf(" Add Shibling\n");
	if ( !iNode_AddShibling ( aNode, newNode ) ) return false;	
		
	return true;
}

struct iNode * ParsePath ( char * path_, struct iNode * curDir_ )
{
	char curPath[32];
	int index=0;
	printf ( " - ParsePath \n" );
	MemSet ( curPath, '\0', 32 );

	if ( path_[0] == '/' ) return ParsePath ( path_++, (struct iNode *)directoryStack[0] );
	// Seperate path
	while ( (*path_) != '\0' ) 
	{
		curPath[index++] = (*path_++);
	}
	curPath[index] = '\0';	
	printf ( " -- ParsePath : curPath [%s] \n", curPath );
	path_++;

	if ( index == 0 ) return curDir_;

	if ( !StrNCmp ( curPath, "..", 2 ) ) {
		curDir_ = curDir_->head;
		return ParsePath ( path_, curDir_->head );
	}

	if ( !StrNCmp ( curPath, ".", 1 ) )
		return ParsePath ( path_, curDir_ );

	return nil;
}
