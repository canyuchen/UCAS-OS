#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#include "type.h"
#include "bitmap.h"
#include "string.h"

#define O_RDONLY    00000000    //只读打开。
#define O_WRONLY    00000001    //只写打开。
#define O_RDWR      00000002    //读、写打开。

#define S_IFMT      0170000     //文件类型的位遮罩
#define S_IFSOCK    0140000     //socket
#define S_IFLNK     0120000     //符号链接(symbolic link)
#define S_IFREG     0100000     //一般文件
#define S_IFBLK     0060000     //区块装置(block device)
#define S_IFDIR     0040000     //目录
#define S_IFCHR     0020000     //字符装置(character device)
#define S_IFIFO     0010000     //先进先出(fifo)

#define S_ISUID     0004000     //文件的(set user-id on execution)位
#define S_ISGID     0002000     //文件的(set group-id on execution)位
#define S_ISVTX     0001000     //文件的sticky位
#define S_IRWXU     00700       //文件所有者的遮罩值(即所有权限值)
#define S_IRUSR     00400       //文件所有者具可读取权限
#define S_IWUSR     00200       //文件所有者具可写入权限
#define S_IXUSR     00100       //文件所有者具可执行权限
#define S_IRWXG     00070       //用户组的遮罩值(即所有权限值)
#define S_IRGRP     00040       //用户组具可读取权限
#define S_IWGRP     00020       //用户组具可写入权限
#define S_IXGRP     00010       //用户组具可执行权限
#define S_IRWXO     00007       //其他用户的遮罩值(即所有权限值)
#define S_IROTH     00004       //其他用户具可读取权限
#define S_IWOTH     00002       //其他用户具可写入权限
#define S_IXOTH     00001       //其他用户具可执行权限

#define S_ISREG(m)   (((m) & S_IFMT) == S_IFREG)
#define S_ISBLK(m)   (((m) & S_IFMT) == S_IFBLK)
#define S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)   (((m) & S_IFMT) == S_IFCHR)
#define S_ISFIFO(m)  (((m) & S_IFMT) == S_IFIFO)
#define S_ISNAM(m)   (((m) & S_IFMT) == S_IFNAM)
#define S_ISLNK(m)   (((m) & S_IFMT) == S_IFLNK)
// 摘自《Linux C 函数库参考手册》
// st_mode是用特征位来表示文件类型的，特征位的定义如上：
// 判断文件类型时，用对文件的st_mode的值与上面给出的值相与，再比较。

//#define pprintf(x) printf(""#x": 0x%x\n", x);

#define ERROR_DUP_DIR_NAME -1
#define ERROR_NO_FREE_INODE -2
#define ERROR_NO_FREE_BLOCK -3
#define ERROR_DIR_NUM_INCORRECT -4
#define ERROR_DENTRY_SETTING_INCORRECT -5




enum {
    BYTE_SIZE = 0x8,

    //FS INFO
    FS_SIZE = 0x40000000, //1GB
    FS_START_SD_OFFSET = 0x20000000, //512MB
    FS_MAGIC_NUMBER = 0x2e575159,

    //SUPERBLOCK INFO
    SUPERBLOCK_BLOCKS_NUM = 1,
    BLOCK_BMP_BLOCKS_NUM = 8,
    INODE_BMP_BLOCKS_NUM = 1,
    INODE_TABLE_BLOCKS_NUM = 256,

    SUPERBLOCK_BLOCK_INDEX = 0,
    BLOCK_BMP_BLOCK_INDEX = SUPERBLOCK_BLOCKS_NUM,
    INODE_BMP_BLOCK_INDEX = SUPERBLOCK_BLOCKS_NUM
                          + BLOCK_BMP_BLOCKS_NUM,
    INODE_TABLE_BLOCK_INDEX = SUPERBLOCK_BLOCKS_NUM 
                            + BLOCK_BMP_BLOCKS_NUM
                            + INODE_BMP_BLOCKS_NUM,
    DATA_BLOCK_INDEX = SUPERBLOCK_BLOCKS_NUM 
                     + BLOCK_BMP_BLOCKS_NUM
                     + INODE_BMP_BLOCKS_NUM
                     + INODE_TABLE_BLOCKS_NUM,

    //BLOCK INFO
    BLOCK_SIZE = 0x1000, //4KB
    BLOCK_NUM = FS_SIZE / BLOCK_SIZE, //256K
    BLOCK_BITMAP_SIZE = BLOCK_NUM / BYTE_SIZE, //32KB

    BLOCK_BMP_NUM_PER_BLOCK = BLOCK_SIZE * BYTE_SIZE,

    //INODE INFO
    INODE_SIZE = 0x80, //128B
    // INODE_SIZE = sizeof(inode_t), //128B
    INODE_TABLE_SIZE = 0x100000, //1MB
    INODE_NUM = INODE_TABLE_SIZE / INODE_SIZE, //8K
    INODE_BITMAP_SIZE = INODE_NUM / BYTE_SIZE, //1KB

    INODE_NUM_PER_BLOCK = BLOCK_SIZE / INODE_SIZE,

    MAX_DIRECT_NUM = 12,

    //SD INFO

    //dentry
    MAX_NAME_LENGTH = 64,
    DENTRY_SIZE = 128,
    MAX_PATH_LENGTH = 512,

    DENTRY_NUM_PER_BLOCK = BLOCK_SIZE / DENTRY_SIZE,

    //file descriptor
    MAX_FILE_DESCRIPTOR_NUM = 32,

    POINTER_PER_BLOCK = (BLOCK_SIZE / sizeof(int32_t)),
    FIRST_POINTER = MAX_DIRECT_NUM,
    SECOND_POINTER = (FIRST_POINTER + POINTER_PER_BLOCK),
    THIRD_POINTER = (SECOND_POINTER + POINTER_PER_BLOCK * POINTER_PER_BLOCK),
    MAX_BLOCK_INDEX = (THIRD_POINTER + POINTER_PER_BLOCK * POINTER_PER_BLOCK * POINTER_PER_BLOCK),

    MAX_LS_NUM = 10,

    MAX_DENTRY_BLOCK_NUM = FIRST_POINTER,

    FILE_READ_BLOCK_NUM = 5,
    FILE_READ_MAX_LENGTH = FILE_READ_BLOCK_NUM * BLOCK_SIZE,
    FILE_WRITE_BLOCK_NUM = 5,
    FILE_WRITE_MAX_LENGTH = FILE_READ_BLOCK_NUM * BLOCK_SIZE,

    CAT_BLOCK_NUM = 5,
    CAT_MAX_LENGTH = CAT_BLOCK_NUM * BLOCK_SIZE,

    CAT_LENGTH = 200,
};

typedef struct superblock {
    uint32_t s_disk_size;                  //磁盘总容量
    uint32_t s_block_size;                 //block大小
    //2
    uint32_t s_magic;                      //魔数(0x2e575159)
    //3
    uint32_t s_total_inodes_cnt;           //磁盘总 inode 数
    uint32_t s_total_blocks_cnt;           //磁盘总 block 数
    uint32_t s_free_inode_cnt;             //磁盘空闲 inode 数
    uint32_t s_free_blocks_cnt;            //磁盘空闲 block 数
    //7
    uint32_t s_blockbmp_block_index;       //Block Bitmap起始 block
    uint32_t s_inodebmp_block_index;       //Inode Bitmap起始 block
    uint32_t s_inodetable_block_index;     //Inode Table起始 block
    uint32_t s_data_block_index;           //data Blocks起始 block
    //11
    uint32_t s_checknum;                   //校验和
    //12
    uint32_t s_inode_size;
    uint32_t s_dentry_size;
    //14
    uint32_t padding[114];
    //128
} superblock_t; //size: 128*sizeof(int) -> 512Byte

typedef struct inode {
    uint16_t i_fmode;                        //文件类型和权限信息
    uint16_t i_links_cnt;                    //硬链接数量 
    //1
    uint32_t i_fsize;                        //文件大小
    uint32_t i_fnum;                         //目录内文件数(不含.和..)
    //3
    uint32_t i_atime;                        //最后访问时间
    uint32_t i_ctime;                        //元数据最后修改时间
    uint32_t i_mtime;                        //文件最后修改时间
    //6
    uint32_t i_direct_table[MAX_DIRECT_NUM]; //直接block指针
    uint32_t i_indirect_block_1_ptr;         //1级指针block指针
    uint32_t i_indirect_block_2_ptr;         //2级指针block指针
    uint32_t i_indirect_block_3_ptr;         //3级指针block指针
    //21
    uint32_t i_num;                          //inode number
    //22
    uint32_t padding[10];
    //32
} inode_t;  //size: 32*sizeof(int) -> 128Byte

typedef struct dentry {
    uint32_t d_inum;
    //1
    char     d_name[MAX_NAME_LENGTH];
    //17
    uint32_t padding[15];
    //32
} dentry_t; //size: 32*sizeof(int) -> 128Byte

typedef struct file_descriptor {
    uint32_t fd_inum;
    uint32_t fd_mode;
    uint32_t fd_r_offset;
    uint32_t fd_w_offset;
    //4
    uint32_t padding[4];
    //8
} file_descriptor_t; //size: 8*sizeof(int) -> 32Byte

typedef uint16_t mode_t;


extern dentry_t ls_buffer[MAX_LS_NUM];

extern uint8_t cat_buffer[CAT_MAX_LENGTH];

// extern uint8_t inode_table[INODE_TABLE_SIZE];

void sdread(unsigned char *buf, unsigned int base, int n);
void sdwrite(unsigned char *buf, unsigned int base, int n);

//use libepmon.a to read SD card
void sd_card_read(void *dest, uint32_t sd_offset, uint32_t size);
//use libepmon.a to write SD card
void sd_card_write(void *dest, uint32_t sd_offset, uint32_t size);

int do_fopen(char *name, uint32_t mode);
int do_fwrite(int fd, char *buffer, int length);
int do_fread(int fd, char *buffer, int length);
void do_fclose(int fd);
// void do_fexit();

void init_fs();
void do_mkfs();
void do_statfs();
void do_cd(char *name);
uint32_t do_mkdir(const char *path, mode_t mode);
// uint32_t do_mkdir(const char *path, mode_t mode);
void do_rmdir(const char *path);
// void do_rmdir(const char *path);
void do_ls();

int do_touch(char *name, mode_t mode);
void do_cat(char *name);

int do_find(char *path, char *name);
void do_rename(char *old_name, char *new_name);
//only for FS

void separate_path(const char *path, char *parent, char *name);
// void separate_path(char *path, char *parent, char *name);
int get_block_index_in_dir(inode_t *inode_ptr, uint32_t idx);
void write_block_index_in_dir(inode_t *inode_ptr, uint32_t idx, uint32_t block_index);
// int find_file(inode_t *inode_ptr, const char *name);
int find_file(inode_t *inode_ptr, char *name);
uint32_t parse_path(const char *path, inode_t *inode_ptr);
// uint32_t parse_path(char *path, inode_t *inode_ptr);
int find_free_inode();
int find_free_block();
void read_dentry(inode_t* inode_ptr, uint32_t dnum, dentry_t* dentry_ptr);

#endif
