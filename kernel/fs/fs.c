#include "fs.h"
#include "time.h"
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

uint8_t superblock_buffer[BLOCK_SIZE] = {0};
// uint8_t blockbmp_block_buffer[BLOCK_SIZE] = {0};
uint8_t blockbmp_buffer[BLOCK_BITMAP_SIZE] = {0};
uint8_t inodebmp_block_buffer[BLOCK_SIZE] = {0};
uint8_t inodetable_block_buffer[BLOCK_SIZE] = {0};

uint8_t data_block_buffer[BLOCK_SIZE] = {0};
uint8_t dentry_block_buffer[BLOCK_SIZE] = {0};

BitMap_t blockbmp_buffer_ptr = (BitMap_t)blockbmp_buffer;
BitMap_t inodebmp_block_buffer_ptr = (BitMap_t)inodebmp_block_buffer;

file_descriptor_t file_descriptor_table[MAX_FILE_DESCRIPTOR_NUM];

superblock_t *superblock_ptr = (superblock_t *)superblock_buffer;

uint8_t find_file_buffer[BLOCK_SIZE] = {0};
uint8_t parse_file_buffer[MAX_PATH_LENGTH] = {0};

uint32_t buffer1[POINTER_PER_BLOCK] = {0};
uint32_t buffer2[POINTER_PER_BLOCK] = {0};
uint32_t buffer3[POINTER_PER_BLOCK] = {0};

inode_t root_inode;
inode_t *root_inode_ptr = &root_inode;

char parent[MAX_PATH_LENGTH];
char name[MAX_NAME_LENGTH];

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

static void sync_to_disk_dentry(uint32_t dentry_offset)
{
    write_block(DATA_BLOCK_INDEX + dentry_offset, dentry_block_buffer);
}

//sync from disk to memory
static void sync_from_disk_inode_bmp()
{
    read_block(INODE_BMP_BLOCK_INDEX, inodebmp_block_buffer);
}

static void sync_from_disk_block_bmp()
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

static void sync_from_disk_dentry(uint32_t dentry_offset)
{
    read_block(DATA_BLOCK_INDEX + dentry_offset, dentry_block_buffer);
}

//------------------------------------------------------------------------------------------

static void write_to_buffer_inode(inode_t *inode_ptr)
{
    memcpy((inodetable_block_buffer + (inode_ptr->i_num % INODE_NUM_PER_BLOCK)*INODE_SIZE), 
           (uint8_t *)inode_ptr, INODE_SIZE);
}

static void read_inode(uint32_t inum, inode_t *inode_ptr)
{
    uint32_t inode_table_offset = inum / INODE_NUM_PER_BLOCK;
    sync_from_disk_inode_table(inode_table_offset);
    memcpy((uint8_t *)inode_ptr,inodetable_block_buffer+(inum%INODE_NUM_PER_BLOCK)*INODE_SIZE, INODE_SIZE);
}

static void clear_disk()
{
    int i = 0;
    for(; i < BLOCK_NUM; i++){
        write_block(i, data_block_buffer);
    }
}

static int write_dentry(inode_t* inode_ptr, uint32_t dnum, dentry_t* dentry)
{
    if(dnum < 2){
        vt100_move_cursor(1, 45);
        printk("[FS ERROR] ERROR_DIR_NUM_INCORRECT\n");
        return ERROR_DIR_NUM_INCORRECT;
    }
    if(dnum > inode_ptr->i_fnum + 2){
        vt100_move_cursor(1, 45);
        printk("[FS ERROR] ERROR_DENTRY_SETTING_INCORRECT\n");
        return ERROR_DENTRY_SETTING_INCORRECT;
    }

    uint32_t major_index, minor_index;
    major_index = dnum / DENTRY_NUM_PER_BLOCK;
    minor_index = dnum % DENTRY_NUM_PER_BLOCK;

    bzero(dentry_block_buffer, BLOCK_SIZE);
    dentry_t *dentry_table = (dentry_t *)dentry_block_buffer;

    if(get_block_index_in_dir(inode_ptr, major_index) == 0){

    }
    else{
        
    }
}

//-------------------------------------------------------------------------------

//operations on file system
void init_fs()
{
    // sd_card_read(superblock_buffer, FS_START_SD_OFFSET, BLOCK_SIZE);
    // superblock_ptr = (superblock_t *)superblock_buffer;
    // read_block(SUPERBLOCK_BLOCK_INDEX, superblock_buffer);
    sync_from_disk_superblock();

    if(superblock_ptr->s_magic == FS_MAGIC_NUMBER){

        sync_from_disk_block_bmp();
        sync_from_disk_inode_bmp();

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
    bzero(data_block_buffer, BLOCK_SIZE);
    bzero(superblock_buffer, BLOCK_SIZE);
    bzero(blockbmp_buffer, BLOCK_BITMAP_SIZE);
    bzero(inodebmp_block_buffer, BLOCK_SIZE);
    bzero(dentry_block_buffer, BLOCK_SIZE);
    bzero(inodetable_block_buffer, BLOCK_SIZE);

    bzero(parent, MAX_PATH_LENGTH);
    bzero(name, MAX_NAME_LENGTH);

    clear_disk();

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
    superblock_ptr->s_free_inode_cnt--;
    sync_to_disk_superblock();

    uint32_t root_block_index = DATA_BLOCK_INDEX;
    set_block_bmp(root_block_index);
    sync_to_disk_block_bmp();
    superblock_ptr->s_free_blocks_cnt--;
    sync_to_disk_superblock();

    root_inode_ptr->i_fmode = S_IFDIR | 0755;
    //???
    root_inode_ptr->i_links_cnt = 1;
    root_inode_ptr->i_fsize = BLOCK_SIZE;
    root_inode_ptr->i_fnum = 0;
    root_inode_ptr->i_atime = get_ticks();
    root_inode_ptr->i_ctime = get_ticks();
    root_inode_ptr->i_mtime = get_ticks();
    bzero(root_inode_ptr->i_direct_table, MAX_DIRECT_NUM*sizeof(uint32_t));
    root_inode_ptr->i_indirect_block_1_ptr = NULL;
    root_inode_ptr->i_indirect_block_2_ptr = NULL;
    root_inode_ptr->i_indirect_block_3_ptr = NULL;
    root_inode_ptr->i_num = 0;
    bzero(root_inode_ptr->padding, 10*sizeof(uint32_t));

    write_to_buffer_inode(root_inode_ptr);
    uint32_t inode_table_offset = root_inum / INODE_NUM_PER_BLOCK;
    sync_to_disk_inode_table(inode_table_offset);

    dentry_t *root_dentry_table = (dentry_t *)dentry_block_buffer;
    root_dentry_table[0].d_inum = 0;
    strcpy(root_dentry_table[0].d_name, ".");
    root_dentry_table[1].d_inum = 0;
    strcpy(root_dentry_table[1].d_name, "..");
    uint32_t dentry_offset = 0;
    sync_to_disk_dentry(dentry_offset);

    //print FS info
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

void separate_path(const char *path, char *parent, char *name)
{
    strcpy(parent, (char *)path);
    char* loc = strrchr(parent, '/');
    strcpy(name, loc + 1);
    name[MAX_NAME_LENGTH - 1] = '\0';
    if(loc == parent){
        loc++;
    } 
    *loc = '\0';
    return;
}

int get_block_index_in_dir(inode_t *inode_ptr, uint32_t idx)
{
    bzero(buffer1, POINTER_PER_BLOCK*sizeof(uint32_t));
    bzero(buffer2, POINTER_PER_BLOCK*sizeof(uint32_t));
    bzero(buffer3, POINTER_PER_BLOCK*sizeof(uint32_t));
    
    if(idx < FIRST_POINTER){
        return inode_ptr->i_direct_table[idx];
    }
    if(idx < SECOND_POINTER){
        if(inode_ptr->i_indirect_block_1_ptr == 0){
            return -1;
        }
        read_block(inode_ptr->i_indirect_block_1_ptr, (uint8_t *)buffer1);
        return buffer1[idx - FIRST_POINTER];
    }
    if(idx < THIRD_POINTER){
        if(inode_ptr->i_indirect_block_1_ptr == 0){
            return -1;
        }
        read_block(inode_ptr->i_indirect_block_1_ptr, (uint8_t *)buffer1);
        if(buffer1[(idx - SECOND_POINTER) / POINTER_PER_BLOCK] == 0){
            return -1;
        }
        read_block(buffer1[(idx - SECOND_POINTER) / POINTER_PER_BLOCK], (uint8_t *)buffer2);
        return buffer2[(idx - SECOND_POINTER) % POINTER_PER_BLOCK];
    }
    if(idx < MAX_BLOCK_INDEX){
        if(inode_ptr->i_indirect_block_1_ptr == 0){
            return -1;
        }
        read_block(inode_ptr->i_indirect_block_1_ptr, (uint8_t *)buffer1);
        if(buffer1[(idx - THIRD_POINTER) / (POINTER_PER_BLOCK * POINTER_PER_BLOCK)] == 0){
            return -1;
        } 
        read_block(buffer1[(idx - THIRD_POINTER) / (POINTER_PER_BLOCK * POINTER_PER_BLOCK)], (uint8_t *)buffer2);
        if(buffer2[((idx - THIRD_POINTER) % (POINTER_PER_BLOCK * POINTER_PER_BLOCK)) / POINTER_PER_BLOCK] == 0){
            return -1;
        } 
        read_block(buffer2[((idx - THIRD_POINTER) % (POINTER_PER_BLOCK * POINTER_PER_BLOCK)) / POINTER_PER_BLOCK], (uint8_t *)buffer3);
        return buffer3[(idx - THIRD_POINTER) % POINTER_PER_BLOCK];
    }   
}

uint32_t find_file(inode_t *inode_ptr, const char *name)
{
    bzero(find_file_buffer, BLOCK_SIZE);
    dentry_t *p = (dentry_t *)find_file_buffer;
    int i = 0, j = 0;
    for(; i < MAX_BLOCK_INDEX; i++){
        uint32_t block_index = get_block_index_in_dir(inode_ptr, i);
        read_block(block_index, find_file_buffer);
        for(; j < POINTER_PER_BLOCK; j++){
            if(strcmp((char *)name, p[j].d_name) == 0){
                return p[j].d_inum;
            }
        }
    }
    return -1;
}

uint32_t parse_path(const char *path)
{
    bzero(parse_file_buffer, MAX_PATH_LENGTH);
    char *p;
    strcpy(parse_file_buffer, (char *)path);
    p = strtok(parse_file_buffer, "/");
    if(p == NULL){
        return 0; //root
    }
    uint32_t result = find_file(root_inode_ptr, p); //root dentry

    inode_t inode;
    while((p = strtok(NULL, "/")) != NULL){
        read_inode(result, &inode);
        result = find_file(&inode, p);
    }
    return result;
}

int find_free_inode()
{
    int i = 0, j = 0;
    for(; i < INODE_BITMAP_SIZE; i++){
        if(inodebmp_block_buffer[i] != 0xff){
            while(check_inode_bmp(j) == 1){
                j++;
            }
            return j;
        }
        else{
            j += BYTE_SIZE;         
        }
    }
    vt100_move_cursor(1, 45);
    printk("[FS ERROR] ERROR_NO_FREE_INODE\n");
    return ERROR_NO_FREE_INODE;
}

int find_free_block()
{
    int i = 0, j = 0;
    for(; i < BLOCK_BITMAP_SIZE; i++){
        if(blockbmp_buffer[i] != 0xff){
            while(check_block_bmp(j) == 1){
                j++;
            }
            return j;
        }
        else{
            j += BYTE_SIZE;         
        }
    }
    vt100_move_cursor(1, 45);
    printk("[FS ERROR] ERROR_NO_FREE_BLOCK\n");
    return ERROR_NO_FREE_BLOCK;
}

//operations on directory
uint32_t do_mkdir(const char *path, mode_t mode)
{
    bzero(parent, MAX_PATH_LENGTH);
    bzero(name, MAX_NAME_LENGTH);

    separate_path(path, parent, name);

    uint32_t parent_inum, free_inum, free_block_index;
    parent_inum = parse_path(parent);
    
    inode_t parent_inode, new_inode;
    read_inode(parent_inum, &parent_inode);

    if(find_file(&parent_inode, name) != -1){
        vt100_move_cursor(1, 45);
        printk("[FS ERROR] ERROR_DUP_DIR_NAME\n");
        return ERROR_DUP_DIR_NAME;
    }

    free_inum = find_free_inode();
    set_inode_bmp(free_inum);
    sync_to_disk_inode_bmp();

    superblock_ptr->s_free_inode_cnt--;
    sync_to_disk_superblock();

    free_block_index = find_free_block();
    set_block_bmp(free_block_index);
    sync_to_disk_block_bmp();

    superblock_ptr->s_free_blocks_cnt--;
    sync_to_disk_superblock();    

    new_inode.i_fmode = S_IFDIR | mode;
    new_inode.i_links_cnt = 1;
    new_inode.i_fsize = BLOCK_SIZE;
    new_inode.i_fnum = 0;
    new_inode.i_atime = get_ticks();
    new_inode.i_ctime = get_ticks();
    new_inode.i_mtime = get_ticks();
    bzero(new_inode.i_direct_table, MAX_DIRECT_NUM*sizeof(uint32_t));
    new_inode.i_indirect_block_1_ptr = NULL;
    new_inode.i_indirect_block_2_ptr = NULL;
    new_inode.i_indirect_block_3_ptr = NULL;
    new_inode.i_num = free_inum;
    bzero(new_inode.padding, 10*sizeof(uint32_t));

    bzero(inodetable_block_buffer, BLOCK_SIZE);
    uint32_t inode_table_offset = free_inum / INODE_NUM_PER_BLOCK;
    sync_from_disk_inode_table(inode_table_offset);
    write_to_buffer_inode(&new_inode);
    sync_to_disk_inode_table(inode_table_offset);

    bzero(dentry_block_buffer, BLOCK_SIZE);
    dentry_t *new_dentry_table = (dentry_t *)dentry_block_buffer;
    new_dentry_table[0].d_inum = free_inum;
    strcpy(new_dentry_table[0].d_name, ".");
    new_dentry_table[1].d_inum = parent_inum;
    strcpy(new_dentry_table[1].d_name, "..");
    uint32_t dentry_offset = 0;
    sync_to_disk_dentry(dentry_offset);

    dentry_t parent_dentry;
    parent_dentry.d_inum = free_inum;
    strcpy(parent_dentry.d_name, name);


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




