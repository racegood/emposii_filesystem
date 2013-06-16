/*
description : initialize block control block
return value : void
   */
void blkdev_init(void);

/*
description : open block device
return value : uid
   */
UID blkdev_open(unsigned int major);

/*
description : close block device
return value : true or false
   */
int blkdev_close(UID id);

/*
description : write data to block device
return value : start pointer of block device
   */
block_datastr *blkdev_write_block(UID id,char *data);

/*
description : read data from block device
return value : start pointer of data
   */
char *blkdev_read_block(UID id, block_datastr *ptr);

/*
description : get data size in block
return value : size of written block
   */
int get_blocksize(block_datastr *ptr);

/*
description : get data size from data
return value : size of data
   */
int get_datasize(char *data);

