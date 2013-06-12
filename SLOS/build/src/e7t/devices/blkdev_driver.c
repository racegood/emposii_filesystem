
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

typedef struct block_control_block{
	int n_blocks;
	int blocksleft;
	block_datastr *blk;
} block_cb;

unsigned int uid;
/*****************************************************************************
 * STATICS
 *****************************************************************************/
static block_cb bcb;

void blkdev_init(void)
{
	block_datastr *blk = (block_datastr*)BLOCK_INIT_PTR;

	int i;
	for(i=0; i<BLOCK_NUM; i++)
	{
		blk->next = NULL;
		blk++;
	}

	bcb.n_blocks = BLOCK_NUM;
	bcb.blocksleft = BLOCK_NUM;
	bcb.blk = blk;

	uid = NONE;
}

UID blkdev_open(unsigned int major) 
{
	int i;
	if(major == DEVICE_BLOCK_EMPOSII){
		if(uid != NONE){
			return DEVICE_IN_USE;
		}
		else{
			uid = uid_generate();
			return uid;
		}
	}
	return DEVICE_NEXT;
}

int blkdev_close(UID id) 
{
	if(uid == id){
		uid = NONE;
		return DEVICE_SUCCESS;
	}
	return DEVICE_UNKNOWN;
}

block_datastr *find_freeblock(void)
{
	block_datastr *ret = (block_datastr*)BLOCK_INIT_PTR;
	while(ret->next){
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
	ret = bp = find_freeblock();	 // get free block pointer

	size = get_datasize(data); // get size of data

	while(size > 0){
		if(size > BLOCK_SIZE)
		{
			MemCpy(bp, data, BLOCK_SIZE);
			//bp->next = find_freeblock();
	//		bp = bp->next;
		}
		else
			MemCpy(bp, data, size);
		size = size - BLOCK_SIZE;
		bcb.blocksleft--;
	}
	return ret;
}

/* do nothing */
block_datastr *blkdev_read_block(UID id) 
{
	block_datastr *ret = NULL;
	return ret;
}

int get_datasize(char *data)
{
	int i=0;
	char *ptr = data;
	while(ptr != NULL)
	{
		i++;
		ptr++;
	}
	return i;
}
