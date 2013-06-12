#include "system_kernel.h"
extern int stackIndex;
extern int directoryStack[];

unsigned int	header=nil;					/* System */
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

