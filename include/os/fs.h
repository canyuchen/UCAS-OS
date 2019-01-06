#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#include "type.h"

#define O_RDONLY    00000000
#define O_WRONLY    00000001
#define O_RDWR      00000002

#define pprintf(x) printf(""#x": 0x%x\n", x);

enum {
    BYTE_SIZE = 0x8,

    //FS INFO
    FS_SIZE = 0x40000000, //1GB

    //SUPERBLOCK INFO

    //BLOCK INFO
    BLOCK_SIZE = 0x1000, //4KB
    BLOCK_NUM = FS_SIZE / BLOCK_SIZE, //256K
    BLOCK_BITMAP_SIZE = BLOCK_NUM / BYTE_SIZE, //32KB

    //INODE INFO
    INODE_SIZE = 0x80, //128B
    INODE_TABLE_SIZE = 0x100000, //1MB
    INODE_NUM = INODE_TABLE_SIZE / INODE_SIZE, //8K
    INODE_BITMAP_SIZE = INODE_NUM / BYTE_SIZE, //4KB

};

void sdread(unsigned char *buf, unsigned int base, int n);
void sdwrite(unsigned char *buf, unsigned int base, int n);

int do_fopen(char *name, uint32_t mode);
void do_fwrite(int fd, char *content, int length);
void do_fread(int fd, char *buffer, int length);
void do_fclose(int fd);
void do_fexit();

#endif