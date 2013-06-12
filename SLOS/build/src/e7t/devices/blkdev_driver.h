void blkdev_init(void);

UID blkdev_open(unsigned int major);
int blkdev_close(UID id);

block_datastr *blkdev_write_block(UID id,char *data);

block_datastr *blkdev_read_block(UID id);


