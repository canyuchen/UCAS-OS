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
uint8_t blockbmp_block_buffer[BLOCK_SIZE] = {0};
uint8_t inodebmp_block_buffer[BLOCK_SIZE] = {0};
uint8_t inodetable_block_buffer[BLOCK_SIZE] = {0};
uint8_t data_block_buffer[BLOCK_SIZE] = {0};
uint8_t dentry_block_buffer[BLOCK_SIZE] = {0};

BitMap_t blockbmp_block_buffer_ptr = blockbmp_block_buffer;
BitMap_t inodebmp_block_buffer_ptr = inodebmp_block_buffer;

// superblock_t superblock;

file_descriptor_t file_descriptor_table[MAX_FILE_DESCRIPTOR_NUM];

superblock_t *superblock_ptr = (superblock_t *)superblock_buffer;

inode_t root;
inode_t *current_root_ptr = &root;


void sd_card_read(void *dest, uint32_t sd_offset, uint32_t size)
{
    sdread((char *)dest, sd_offset, size);
}

void sd_card_write(void *dest, uint32_t sd_offset, uint32_t size)
{
    sdwrite((char *)dest, sd_offset, size);
}

static void write_block_bmp(uint32_t block_bmp_bit, bool_t value)
{
    if(value == 1){
        set_bitmap(blockbmp_block_buffer_ptr, block_bmp_bit);        
    }
    else{
        unset_bitmap(blockbmp_block_buffer_ptr, block_bmp_bit);
    }
    return;
}

static bool_t read_block_bmp(uint32_t block_bmp_bit)
{
    return check_bitmap(blockbmp_block_buffer_ptr, block_bmp_bit);
}

static void write_inode_bmp(uint32_t inode_bmp_bit, bool_t value)
{
    if(value == 1){
        set_bitmap(inodebmp_block_buffer_ptr, inode_bmp_bit);        
    }
    else{
        unset_bitmap(inodebmp_block_buffer_ptr, inode_bmp_bit);
    }
    return;   
}

static bool_t read_inode_bmp(uint32_t inode_bmp_bit)
{
    return check_bitmap(inodebmp_block_buffer_ptr, inode_bmp_bit);
}

/*
static void write_inode_table(uint32_t inum, uint8_t *inodetable_block_buffer)
{
    uint32_t inum %= INODE_NUM_PER_BLOCK;

}

static void read_inode_table(uint32_t inum, uint8_t *inodetable_block_buffer)
{

}
*/

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

//-------------------------------------------------------------------------------

void init_fs()
{
    // sd_card_read(superblock_buffer, FS_START_SD_OFFSET, BLOCK_SIZE);
    // superblock_ptr = (superblock_t *)superblock_buffer;
    read_block(SUPERBLOCK_BLOCK_INDEX, superblock_buffer);
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
    bzero(superblock_buffer, BLOCK_SIZE);
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
        write_block_bmp(i, 1);
        superblock_ptr->s_free_blocks_cnt--;
    }
    write_block(BLOCK_BMP_BLOCK_INDEX, blockbmp_block_buffer);

    write_block(SUPERBLOCK_BLOCK_INDEX, superblock_buffer);

    //init root dir
    

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




