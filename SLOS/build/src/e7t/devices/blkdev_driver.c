
#include "../../devices/ddf_frame.h"
#include "blkdev_driver.h"
#include "../headers/macros.h"
#include "../../serial/serial.h"

#define DEVICE_BLOCK_EMPOSII	55100
/*****************************************************************************
 * MACROS
 *****************************************************************************/

/*****************************************************************************
 * DATATYPES
 *****************************************************************************/
#define BLOCK_SIZE ((1<<10) - 4)
#define BLOCK_NUM (58 * 1000)
#define BLOCK_INIT_PTR 0xA0600000
#define BLOCK_END_PTR 0xA4000000

/*****************************************************************************
 * STATICS
 *****************************************************************************/
static char tempblock[1048576];

void blkdev_init(void)
{
	block_datastr *blk = (block_datastr*)BLOCK_INIT_PTR;

	unsigned int i;
	
	bcb.n_blocks = BLOCK_NUM;
	bcb.blocksleft = BLOCK_NUM;
	bcb.blk = blk;
	bcb.uid = NONE;
}

UID blkdev_open(unsigned int major) 
{
	int i;
	if(major == DEVICE_BLOCK_EMPOSII){
		if(bcb.uid == NONE){
			bcb.uid = uid_generate();
			return bcb.uid;
		}
		else{
			return DEVICE_IN_USE;
		}
	}
	return DEVICE_NEXT;
}

int blkdev_close(UID id) 
{
	if(bcb.uid == id){
		bcb.uid = NONE;
		return DEVICE_SUCCESS;
	}
	return DEVICE_UNKNOWN;
}

block_datastr *find_freeblock(void)
{
	block_datastr *ret = bcb.blk;
	while(((unsigned int)(ret->next) & 0x1) != 0){
		ret++;
	}
	return ret;
}

block_datastr *blkdev_write_block(UID id,char *data)		
{
	int size;
	char *copy_ptr;
	block_datastr *ret;
	block_datastr *bp;

	char *temp;

	int i=0;

	ret = bp = find_freeblock();	 // get free block pointer

	size = get_datasize(data); // get size of data
	printf("size : %d \n", size);

	while(size > 0){
		if(bcb.blocksleft <= 0)
		{
			printf("No have empty block");
			return NULL;
		}
		bp->next = (unsigned int)(bp->next)|(0x1);	// check block to be used
		if(size > BLOCK_SIZE)
		{
			MemCpy(bp->data, data, BLOCK_SIZE);
			bp->next = (unsigned int)(bp->next)|(unsigned int)find_freeblock();
			printf("bp - bp->next : %x - %x [%d]\n", bp, bp->next, size);
			bp = (unsigned int)(bp->next)&0xFFFFFFFE;
		}
		else
		{
			MemCpy(bp->data, data, size);
			printf(" bp - bp->next : %x - %x [%d]\n", bp, bp->next, size);
		}
		size = size - BLOCK_SIZE;
		bcb.blocksleft--;
	}


	return ret;
}

int free_block(block_datastr *ptr)
{
	if(ptr == 0)
	{
		return 0;
	}
	else
	{
		free_block((unsigned int)(ptr->next)&0xFFFFFFFE);
		printf(" block free - %x %x\n", ptr, ptr->next);
		ptr->next = NULL;
		bcb.blocksleft++;
		return 1;
	}
}

void blkdev_free_block(UID id, block_datastr *ptr)
{
	int ret;
	ret = free_block(ptr);
	if(ret = 1)
		printf("block free complete\n");
	else
		printf("[error] block free\n");
}

char *blkdev_read_block(UID id, block_datastr *ptr) 
{
	int i,j, size;
	size = get_blocksize(ptr);

	j=0;
	while(size > 0){
		if(size > BLOCK_SIZE)
		{
			for(i=0; i<BLOCK_SIZE; i++)
			{
				tempblock[i] = ptr->data[i];
			}
			ptr = (unsigned int)(ptr->next)&0xFFFFFFFE;
			j+=i;
		}
		else
		{
			for(i=0; i<size; i++)
			{
				tempblock[i+j] = ptr->data[i];
			}
		}
		size = size - BLOCK_SIZE;
	}

	tempblock[i+j] = '\0';
	return tempblock;
}

int get_datasize(char *data)
{
	int i=0;
	char *ptr = data;

	while(*ptr != EOF)
	{
		i++;
		ptr++;
	}
	return i;
}

int get_blockdatasize(char *data)
{
	int i=0;
	char *ptr =data;

	while(*ptr != EOF)
	{
		i++;
		ptr++;
		if(i == BLOCK_SIZE)
		{
			return i;
		}
	}
	return i;
}

int get_blocksize(block_datastr *ptr)
{
	int cnt;

	cnt = get_blockdatasize(ptr->data);
	ptr = (unsigned int)(ptr->next)&0xFFFFFFFE;
	while(ptr!= 0)
	{
		cnt += get_blockdatasize(ptr->data);
		ptr = (unsigned int)(ptr->next)&0xFFFFFFFE;
	}
	return cnt;
}
