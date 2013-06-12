#include "file.h"

static int fd=0;
static file_str file_temp;

int openfile(char *name)
{
	int ret=0;

	if(name[0] != '/')
	{
		printf("filename error");
		return 0;
	}

	if(findfile(name))
	{
		//open
		//return fd
		ret=1;
	}
	else // create new file
	{

	}

	return ret;
}

int closefile(int fd)
{
	int ret=0;

	return ret;
}

int writefile(int fd, char *data)
{
	int ret=0;

	return ret;
}

char *readfile(int fd)
{
	char *data;

	return data;
}

int findfile(char *name)
{
	int i,ret=1;
	char dir[MAXNUM_FILENAME];
	file_str *parent=&file[0];

	for(i=0;i<MAXNUM_FILENAME; i++)
		dir[i] = 0;

	parent = parent->child;

	/*
	if(name[i] == '\0') // directory
	{
		while(parent == NULL)
		{
			if(StrCmp(dir, parent) == 0)
			{
				parent = parent->child;
				break;
			}
			parent = parent->sibling;
		}
	}

	*/
	return ret;
}

char *getFilename(char *name)
{
	char dir[MAXNUM_FILENAME];
	int i=1;

	for(i=0;i<MAXNUM_FILENAME; i++)
		dir[i] = 0;

	// get directory or file name
	while(name[i] != '/' && name[i] !='\0')
	{
		dir[i-1] = name[i];
		i++;
	}
	dir[i-1] = '\0';
	return dir;
}
