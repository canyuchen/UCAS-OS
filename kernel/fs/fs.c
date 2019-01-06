#include "fs.h"

/*
* SD card file system for OS seminar
* This filesystem looks like this:
* FS size : 1GB
* 1 Block : 4KB, total Blocks : 256K
* 1 Inode : 128B
* Inode Bitmap size : 8K / 8 * 1B= 1KB
* Block Bitmap size : 256K / 8 * 1B= 32KB
* --------------------------------------------------------------------------------
* | Superblock   | Block Bitmap  | Inode Bitmap  | Inode Table    |  Blocks    |
* | 1 Block 4KB  | 8 Blocks 32KB | 1 Block  4KB  | 256 Blocks 1MB |  Others    |
* --------------------------------------------------------------------------------
*/

uint8_t block_bitmap[BLOCK_BITMAP_SIZE] = {0};
uint8_t inode_bitmap[INODE_BITMAP_SIZE] = {0};

BitMap_t block_bitmap_ptr = block_bitmap;
BitMap_t inode_bitmap_ptr = inode_bitmap;

void sd_card_read(void *dest, uint32_t sd_offset, uint32_t size)
{
    sdread((char *)dest, sd_offset, size);
}

void sd_card_write(void *dest, uint32_t sd_offset, uint32_t size)
{
    sdwrite((char *)dest, sd_offset, size);
}

int do_fopen(char *name, uint32_t mode)
{

}

void do_fwrite(int fd, char *content, int length)
{

}

void do_fread(int fd, char *buffer, int length)
{

}

void do_fclose(int fd)
{

}

void do_fexit()
{

}




