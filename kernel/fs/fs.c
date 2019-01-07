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

// uint8_t block_bitmap[BLOCK_BITMAP_SIZE] = {0};
// uint8_t inode_bitmap[INODE_BITMAP_SIZE] = {0};

// inode_t inode_table[INODE_NUM] = {0};
// uint8_t inode_table[INODE_TABLE_SIZE] = {0};

// BitMap_t block_bitmap_ptr = block_bitmap;
// BitMap_t inode_bitmap_ptr = inode_bitmap;

uint8_t superblock_buffer[BLOCK_SIZE] = {0};
// uint8_t blockbmp_block_buffer[BLOCK_SIZE] = {0};
uint8_t blockbmp_buffer[BLOCK_BITMAP_SIZE] = {0};
uint8_t inodebmp_block_buffer[BLOCK_SIZE] = {0};
uint8_t inodetable_block_buffer[BLOCK_SIZE] = {0};

uint8_t data_block_buffer[BLOCK_SIZE] = {0};
uint8_t dentry_block_buffer[BLOCK_SIZE] = {0};

BitMap_t blockbmp_buffer_ptr = (BitMap_t)blockbmp_buffer;
BitMap_t inodebmp_block_buffer_ptr = (BitMap_t)inodebmp_block_buffer;

// superblock_t superblock;

file_descriptor_t file_descriptor_table[MAX_FILE_DESCRIPTOR_NUM];

superblock_t *superblock_ptr = (superblock_t *)superblock_buffer;

inode_t root_inode;
inode_t *root_inode_ptr = &root_inode;

static void set_block_bmp(uint32_t block_index)
{
    set_bitmap(blockbmp_buffer_ptr, block_index);        
    return;
}

static void unset_block_bmp(uint32_t block_index)
{
    unset_bitmap(blockbmp_buffer_ptr, block_index);
    return;
}

static bool_t check_block_bmp(uint32_t block_index)
{
    return check_bitmap(blockbmp_buffer_ptr, block_index);
}

static void set_inode_bmp(uint32_t inum)
{
    set_bitmap(inodebmp_block_buffer_ptr, inum);        
    return;   
}

static void unset_inode_bmp(uint32_t inum)
{
    unset_bitmap(inodebmp_block_buffer_ptr, inum);
    return;   
}

static bool_t check_inode_bmp(uint32_t inum)
{
    return check_bitmap(inodebmp_block_buffer_ptr, inum);
}

//-------------------------------------------------------------------------------

void sd_card_read(void *dest, uint32_t sd_offset, uint32_t size)
{
    sdread((char *)dest, sd_offset, size);
}

void sd_card_write(void *dest, uint32_t sd_offset, uint32_t size)
{
    sdwrite((char *)dest, sd_offset, size);
}

//only write_block(), and read_block() interact with SD card
//other func interact with buffer in memory
static void write_block(uint32_t block_index, uint8_t *block_buffer)
{
    uint32_t sd_offset = block_index*BLOCK_SIZE + FS_START_SD_OFFSET;
    sd_card_write(block_buffer, sd_offset, BLOCK_SIZE);
}

static void read_block(uint32_t block_index, uint8_t *block_buffer)
{
    uint32_t sd_offset = block_index*BLOCK_SIZE + FS_START_SD_OFFSET;
    sd_card_read(block_buffer, sd_offset, BLOCK_SIZE);
}

//sync from memory to disk
static void sync_to_disk_inode_bmp()
{
    write_block(INODE_BMP_BLOCK_INDEX, inodebmp_block_buffer);
}

static void sync_to_disk_block_bmp()
{
    int i = 0;
    for(; i < BLOCK_BMP_BLOCKS_NUM; i++){
        write_block(BLOCK_BMP_BLOCK_INDEX + i, blockbmp_buffer + BLOCK_SIZE * i);
    }
    return;
}

static void sync_to_disk_superblock()
{
    write_block(SUPERBLOCK_BLOCK_INDEX, superblock_buffer);
}

static void sync_to_disk_inode_table(uint32_t inode_table_offset)
{
    write_block(INODE_TABLE_BLOCK_INDEX + inode_table_offset, inodetable_block_buffer);
}

static void sync_to_disk_file_data(uint32_t file_data_offset)
{
    write_block(DATA_BLOCK_INDEX + file_data_offset, data_block_buffer);
}

//sync from disk to memory
static void sync_from_disk_inode_bmp()
{
    read_block(INODE_BMP_BLOCK_INDEX, inodebmp_block_buffer);
}

static void sync_from_disk_block_bmp(uint32_t block_bmp_offset)
{
    int i = 0;
    for(; i < BLOCK_BMP_BLOCKS_NUM; i++){
        read_block(BLOCK_BMP_BLOCK_INDEX + i, blockbmp_buffer + BLOCK_SIZE * i);
    }
    return;
}

static void sync_from_disk_superblock()
{
    read_block(SUPERBLOCK_BLOCK_INDEX, superblock_buffer);
}

static void sync_from_disk_inode_table(uint32_t inode_table_offset)
{
    read_block(INODE_TABLE_BLOCK_INDEX + inode_table_offset, inodetable_block_buffer);
}

static void sync_from_disk_file_data(uint32_t file_data_offset)
{
    read_block(DATA_BLOCK_INDEX + file_data_offset, data_block_buffer);
}

//-------------------------------------------------------------------------------

void init_fs()
{
    // sd_card_read(superblock_buffer, FS_START_SD_OFFSET, BLOCK_SIZE);
    // superblock_ptr = (superblock_t *)superblock_buffer;
    // read_block(SUPERBLOCK_BLOCK_INDEX, superblock_buffer);
    sync_from_disk_superblock();

    if(superblock_ptr->s_magic == FS_MAGIC_NUMBER){
        
        vt100_move_cursor(1, 1);    
        printk("[FS] File system exists in the disk!\n");
        printk("[FS] File system current informatin:\n");
        printk("     magic number : 0x%x\n", superblock_ptr->s_magic);
        printk("     file system size : 0x%x\n", superblock_ptr->s_disk_size);
        printk("     block size : 0x%x\n", superblock_ptr->s_block_size);
        printk("     total blocks : %d\n", superblock_ptr->s_total_blocks_cnt);
        printk("     free blocks : %d\n", superblock_ptr->s_free_blocks_cnt);
        printk("     total inodes : %d\n", superblock_ptr->s_total_inodes_cnt);
        printk("     free inodes : %d\n", superblock_ptr->s_free_inode_cnt);
        printk("     block bitmap start-block index : %d\n", superblock_ptr->s_blockbmp_block_index);
        printk("                  disk offset : %d B\n", superblock_ptr->s_blockbmp_block_index * BLOCK_SIZE);
        printk("     inode bitmap start-block index : %d\n", superblock_ptr->s_inodebmp_block_index);
        printk("                  disk offset : %d B\n", superblock_ptr->s_inodebmp_block_index * BLOCK_SIZE);
        printk("     inode table start-block index : %d\n", superblock_ptr->s_inodetable_block_index);
        printk("                 disk offset : %d B\n", superblock_ptr->s_inodetable_block_index * BLOCK_SIZE);
        printk("     file data start-block index : %d\n", superblock_ptr->s_data_block_index);
        printk("     inode entry size : %d\n", superblock_ptr->s_inode_size);
        printk("     dir entry size : %d\n", superblock_ptr->s_dentry_size);
        // printk("[FS] inode bitmap...\n");
        // printk("[FS] block bitmap...\n");
        // printk("[FS] inode table...\n");
        // printk("[FS] Initializing file system finished!\n");
    }
    else{
        do_mkfs();
    }
}

void do_mkfs()
{
    // sd_card_read(superblock_buffer, FS_START_SD_OFFSET, BLOCK_SIZE);

    //init superblock and write into sd card
    // bzero(superblock_buffer, BLOCK_SIZE);
    // superblock_ptr = (superblock_t *)superblock_buffer;

    superblock_ptr->s_magic = FS_MAGIC_NUMBER;
    superblock_ptr->s_disk_size = FS_SIZE;
    superblock_ptr->s_block_size = BLOCK_SIZE;
    superblock_ptr->s_total_blocks_cnt = BLOCK_NUM;
    superblock_ptr->s_total_inodes_cnt = INODE_NUM;

    superblock_ptr->s_blockbmp_block_index = BLOCK_BMP_BLOCK_INDEX;
    superblock_ptr->s_inodebmp_block_index = INODE_BMP_BLOCK_INDEX;
    superblock_ptr->s_inodetable_block_index = INODE_TABLE_BLOCK_INDEX;
    superblock_ptr->s_data_block_index = DATA_BLOCK_INDEX;

    superblock_ptr->s_free_blocks_cnt = BLOCK_NUM - superblock_ptr->s_inodetable_block_index;
    superblock_ptr->s_free_inode_cnt = INODE_NUM;

    superblock_ptr->s_inode_size = INODE_SIZE;
    superblock_ptr->s_dentry_size = DENTRY_SIZE;

    int i = 0;
    for(; i++; i < INODE_TABLE_BLOCK_INDEX){
        set_block_bmp(i);
    }
    sync_to_disk_block_bmp();
    sync_to_disk_superblock();

    //init root dir
    uint32_t root_inum = 0;
    set_inode_bmp(root_inum);
    sync_to_disk_inode_bmp();

    uint32_t root_block_index = DATA_BLOCK_INDEX;
    set_block_bmp(root_block_index);
    sync_to_disk_block_bmp();


    // root_inode_ptr->i_fmode = S_IFDIR | 0755;
    // root_inode_ptr->i_links_cnt = 1;
    // root_inode_ptr->i_fsize = 
    // root_inode_ptr->
    // root_inode_ptr->
    // root_inode_ptr->
    // root_inode_ptr->
    // root_inode_ptr->
    // root_inode_ptr->
    // root_inode_ptr->
    // root_inode_ptr->
    // root_inode_ptr->

    vt100_move_cursor(1, 1);    
    printk("[FS] Starting initialize file system!\n");
    printk("[FS] Setting superblock...\n");
    printk("     magic number : 0x%x\n", superblock_ptr->s_magic);
    printk("     file system size : 0x%x\n", superblock_ptr->s_disk_size);
    printk("     block size : 0x%x\n", superblock_ptr->s_block_size);
    printk("     total blocks : %d\n", superblock_ptr->s_total_blocks_cnt);
    // printk("     free blocks : %d\n", superblock_ptr->s_free_blocks_cnt);
    printk("     total inodes : %d\n", superblock_ptr->s_total_inodes_cnt);
    // printk("     free inodes : %d\n", superblock_ptr->s_free_inode_cnt);
    printk("     block bitmap start-block index : %d\n", superblock_ptr->s_blockbmp_block_index);
    printk("                  disk offset : %d B\n", superblock_ptr->s_blockbmp_block_index * BLOCK_SIZE);
    printk("     inode bitmap start-block index : %d\n", superblock_ptr->s_inodebmp_block_index);
    printk("                  disk offset : %d B\n", superblock_ptr->s_inodebmp_block_index * BLOCK_SIZE);
    printk("     inode table start-block index : %d\n", superblock_ptr->s_inodetable_block_index);
    printk("                 disk offset : %d B\n", superblock_ptr->s_inodetable_block_index * BLOCK_SIZE);
    printk("     file data start-block index : %d\n", superblock_ptr->s_data_block_index);
    printk("     inode entry size : %d\n", superblock_ptr->s_inode_size);
    printk("     dir entry size : %d\n", superblock_ptr->s_dentry_size);
    printk("[FS] inode bitmap...\n");
    printk("[FS] block bitmap...\n");
    printk("[FS] inode table...\n");
    printk("[FS] Initializing file system finished!\n");
}

void do_statfs()
{
    vt100_move_cursor(1, 23);
    printk("[FS] File system current informatin:\n");
    printk("     magic number : 0x%x\n", superblock_ptr->s_magic);
    printk("     file system size : 0x%x\n", superblock_ptr->s_disk_size);
    printk("     block size : 0x%x\n", superblock_ptr->s_block_size);
    printk("     total blocks : %d\n", superblock_ptr->s_total_blocks_cnt);
    printk("     free blocks : %d\n", superblock_ptr->s_free_blocks_cnt);
    printk("     total inodes : %d\n", superblock_ptr->s_total_inodes_cnt);
    printk("     free inodes : %d\n", superblock_ptr->s_free_inode_cnt);
    printk("     block bitmap start-block index : %d\n", superblock_ptr->s_blockbmp_block_index);
    printk("                  disk offset : %d B\n", superblock_ptr->s_blockbmp_block_index * BLOCK_SIZE);
    printk("     inode bitmap start-block index : %d\n", superblock_ptr->s_inodebmp_block_index);
    printk("                  disk offset : %d B\n", superblock_ptr->s_inodebmp_block_index * BLOCK_SIZE);
    printk("     inode table start-block index : %d\n", superblock_ptr->s_inodetable_block_index);
    printk("                 disk offset : %d B\n", superblock_ptr->s_inodetable_block_index * BLOCK_SIZE);
    printk("     file data start-block index : %d\n", superblock_ptr->s_data_block_index);
    printk("     inode entry size : %d\n", superblock_ptr->s_inode_size);
    printk("     dir entry size : %d\n", superblock_ptr->s_dentry_size);
}

/*
void do_cd()
{

}
*/
void do_mkdir()
{

}

void do_rmdir()
{

}
/*
void do_ls()
{

}
*/
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




