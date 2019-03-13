# UCAS-OS

## Project 6 : File System

### Features:

* Implement a simple file system
    * Disk and File system metadata management
    * Hierarchical directory structure
    * Common file system operations and unix-like commands:
        * FILE SYSTEM OPERATIONS :
            - [x] **mkfs** : create a file system
            - [x] **statfs** : print the information related to the file system including : size of the file system, number of inodes, start address of inodes, etc.
        * DIRECTORY OPERATIONS :
            - [x] **cd [directory name] or cd ./[directory name] or cd ./[directory name]/[directory name]** : enter a directory
            - [x] **mkdir [directory name] or mkdir ./[directory name]** : create a directory
            - [x] **rmdir [directory name] or rmdir ./[directory name]** : delete a directory
            - [x] **ls** : print the directory entries in the current directory
        * FILE OPERATIONS :
            - [x] **touch [file name]** : create a file
            - [x] **cat [file name]** : print the content of the file in shell
        * FILE FUNCTIONS :
            - [x] int fopen(char *name, uint32_t mode)
            - [x] int fwrite(int fd, char *buffer, int length)
            - [x] int fread(int fd, char *buffer, int length)
            - [x] void fclose(int fd)
        * ADDITIONAL OPERATIONS :
            - [x] **find [path] [name]** : find out whether the file exists in the directory 
            - [x] **rename [old name] [new name]** : rename a file or directory
            - [x] **ln [src path] [new path]** : create a link
            - [x] **ln -s [src path] [new path]** : create a symble link
            - [] **pwd**
            - [] **dump**
            - [] **du**
            - [] **df**
            - [] **diff [file name] [file name]**
            - [] **wc [file name]**
            - [] **rm [file name]**
            - [] **cp [file name] [file name]**
            - [] **mv [file name] [file name]**
            - [] **chmod [file name]**
            - [] **man [command name]**



### Disk Layout:

```c
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
```

### Structure :



### Features Test :

![6-0](/resources/disk.jpg)

![6-1](/resources/1.gif)

![6-2](/resources/2.gif)

![6-3](/resources/3.gif)

![6-4](/resources/4.gif)

