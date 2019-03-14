# UCAS-OS

## Part 6 : File System

### Features:

* Implement a simple file system
    * Disk and File system metadata management
    * Hierarchical directory structure
    * Use the SD card as the disk for your file system
    * Design and implement mkfs and statfs
    * Design and implement directory operations
    * Design and implement file operations
* Support common file system operations and unix-like commands:
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
        - [ ] **pwd**
        - [ ] **dump**
        - [ ] **du**
        - [ ] **df**
        - [ ] **diff [file name] [file name]**
        - [ ] **wc [file name]**
        - [ ] **rm [file name]**
        - [ ] **cp [file name] [file name]**
        - [ ] **mv [file name] [file name]**
        - [ ] **chmod [file name]**
        - [ ] **man [command name]**
* Design file system structure:
    * Superblock : Metadata to describe the structure of the file system
    * Inodes : Metadata to describe file/directory
    * File descriptor table : Keeping information of opening files
    * Directories : A special file containing list of files and directories
    * Block bitmap : Allocation bitmap of all the blocks of the whole file system
    * Inode bitmap ; Allocation bitmap of the inodes of all the inodes of the whole file system
    * Inode table : Table of all the inodes


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



### Features Test on Hardware:

![6-0](/resources/disk.jpg)

![6-1](/resources/1.gif)

![6-2](/resources/2.gif)

![6-3](/resources/3.gif)

![6-4](/resources/4.gif)

## Part 5 : Device Driver

### Core Features 

* Implement driver for network card
    * Setup MAC controller registers to allow network card initialization, and sending/receiving data without interrupt
    * Implement sending/receiving data with blocking
    * Implement network interrupt handler to serve receiving data
* Design Device Driver Structure
    * MAC controller : Include two parts of registers
        * MAC registers
        * DMA registers
    * Direct Memory Access
    * DMA descriptor
    * MAC interrupt handler

    ![6-0](/resources/driver.png)

## Part 4 : Virtual Memory

### Core Features

* Implement virtual memory management for user process
    * Setup page table and TLB entries
    * Handle TLB exception to support TLB refill and invalid TLB exceptions
    * Handle page fault to support on demand paging assuming physical memory is enough
* Design virtual memory structure
    * Page tables
        * The data structure to store the mapping between virtual addresses and physical addresses
        * Each mapping is a page table entry
    * MMU and TLB
        * MMU stores a cache of recently used mappings from the page table, which is called translation lookaside buffer (TLB)
* Implement page fault handler with TLB miss and page fault, capable of isolating different processes
* Implement the page replacement when the required memory size exceeds the physical memory size. Choose an algorithm for the page replacement algorithm other than FIFO (my choice is clock algorithm)

## Part 3 : Interactive OS and Process Management

### Core Features

* Support interactive operation and basic process management
    * Implement a simple terminal and basic user commands
    * Implement four system calls: spawn, kill, wait, and exit
    * Implement three synchronization primitives
    * Implement inter-process communication mechanism: mailbox and use it to solve Producer-consumer problem
* Design Simple terminal
    * Screen
        * Use the provided printf to show input command
    * Shell
        * A user level process
        * Parse input command and invoke corresponding syscalls
        * Show the input command
* Implement synchronization primitives
    * condition variable
    * semaphores
    * barriers

## Part 2 : A Simple Kernel

### Core Features

* Write a simple kernel (non-preemptive)
    * Start a set of user processes and kernel threads
    * Perform context switches between processes and threads
    * Provide non-preemptive kernel support with context switch
    * Support basic mutex to allow BLOCK state of processes/threads
* Write a simple kernel (preemptive)
    * Provide preemptive kernel support with clock interrupt handler and priority based scheduling
    * Support system calls


## Part 1 : Bootloader

### Core Features

* Write a bootloader to start a simple kernel based on Openloongson SoC board
    * BIOS
        * Basic Input/Output System
        * Firmware used to perform hardware initialization after power-on
        * Load bootloader
    * Bootblock
        * Loaded by BIOS
        * Hard disk
    * Bootloader
        * A small program to enable operating system
        * Load the kernel
        * Switch control to the kernel
