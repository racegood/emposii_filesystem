#include "../core/all.h"
#define MAXNUM_FILENAME 128
#define MAXNUM_FILE		256

typedef struct file_struct{
	block_datastr *ptr;	// 0 bit : 0 - directory, 1 - file
	char name[MAXNUM_FILENAME];
	int size;
	struct file_struct *child;
	struct file_struct *sibling;
}file_str;

static file_str file[MAXNUM_FILE]; 

int openfile(char *name);
int closefile(int fd);
int writefile(int fd, char *data);
char* readfile(int fd);

