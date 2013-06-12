/*******************************************
 *                                         *
 *        2013. 06. 03.                    *
 *         File System Proto Type          *
 *                                         * 
 *******************************************/
/***********************
 *   Include File      *
 ***********************/

/* Standard Library */

/* User Define Library */
#include "FileStructure.h"

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
boolean FileStruct_Initialize ( struct FileStructure * aFile_ )
{
	if ( !aFile_ ) return false;

	aFile_->name[0]	= '\0';
	aFile_->fs_size = 0;
	aFile_->fs_type = 0;

	return true;
}

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
boolean FileStructure_SetDirectoryWithName( struct FileStructure * fs_, char * name_ )
{
	// 인자를 검사해주자 
	if ( !StrLen(name_) ) return false;

	// 이름을 설정해줍니다.
	StrCpy (fs_->name, (char*)name_);

	// Flag Setting 
	fs_->fs_type		= FILE_STRUCTURE_DIRECTORY;
	fs_->fs_size		= 0;

	return true;
}

boolean FileStructure_SetFile( struct FileStructure * fs, char * name_, int size_) 
{
	StrCpy ( fs->name, name_ );
	fs->fs_size = size_ ;
	fs->fs_type = FILE_STRUCTURE_NON_DIRECTORY;
	return true;
}
