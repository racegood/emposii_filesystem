#include "iNode.h"


boolean iNode_Initialize ( struct iNode * aNode ) 
{
	if ( !aNode ) return false;

	aNode->head			= nil;
	aNode->shibling		= nil;
	aNode->child		= nil;
	//aNode->File_Struct	= nil;
	if ( !FileStruct_Initialize( &(aNode->File_Struct) ) ) return false;
	aNode->flag			= iNODE_FLAG_NOT_USED;
	aNode->not_used		= nil;
	aNode->P_addr		= nil;
	return true;
}

boolean iNode_SetDirectoryWithSuperBlock ( struct iNode * dNode, struct iNode * sbNode )
{
	if ( !dNode || !sbNode ) return false;
	if ( dNode->flag != iNODE_FLAG_DIRECTORY ) return false;

	// Set Directory Node 
	dNode->child	= (unsigned int)sbNode;

	// Set Super Block Node 
	sbNode->head	= (unsigned int)dNode;
	sbNode->flag	= iNODE_FLAG_MASTER_BLOCK;

	return true;
}

boolean iNode_SetDirectory ( struct iNode * aNode )
{
	if ( !aNode ) return false;

	// set Directory Node
	aNode->flag		= iNODE_FLAG_DIRECTORY;

	return true;
}

boolean iNode_SetFile	( struct iNode * aNode )
{
	if ( !aNode ) return false;
	// Set File Node 
	aNode->flag		= iNODE_FLAG_NON_DIRECTORY;

	return true;
}

boolean iNode_SetSuperBlock ( struct iNode * aNode )
{
	return true;
}

char * iNode_getName ( struct iNode * aNode )
{
	return (!StrNCmp(aNode->File_Struct.name,"root",4)? "": aNode->File_Struct.name)  ;
}

struct iNode * iNode_FindHeadNode ( struct iNode * aNode_ )
{
	return (struct iNode*)aNode_->child;
}

boolean iNode_AddShibling ( struct iNode * aNode, struct iNode * sNode )
{
	struct iNode * sbNode = iNode_FindHeadNode (aNode);
	
	// Set Shibling Node's Attribute
	sNode->shibling = sbNode->shibling;
	sNode->head		= (unsigned int)sbNode;

	// Set Super Block's Attribute
	sbNode->shibling	= (unsigned int)sNode;

	return true;
}

boolean iNode_Release	( struct iNode * aNode_)
{
	block_datastr * del = aNode_->P_addr;

	printf("hi~\n");
	blkdev_free_block(ublkdev, del);
	printf("bye~\n");

	aNode_->P_addr = NULL;
	return iNode_Initialize(aNode_);
}

boolean iNode_SetPhysicalAddress ( struct iNode * aNode, unsigned int addr_ ) 
{
	// 메모리 주소 검사
	if ( !addr_ ) return false;

	// 메모리 주소 대입
	aNode->P_addr	= addr_;
	return true;
}
