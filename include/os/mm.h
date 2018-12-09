#ifndef INCLUDE_MM_H_
#define INCLUDE_MM_H_

#include "type.h"

enum {
    /* physical page facts */
    PAGE_SIZE = 0x1000,
    //Level 1
    // PAGE_DIR_ENTRIES = (PAGE_SIZE / sizeof(uint32_t)),
    //Level 2
    //FOR NOW only considering ONE LEVEL page table

    PAGE_TABLE_SIZE = 0x400000, //4MB
    PAGE_TABLE_ENTRIES_NUM = (PAGE_TABLE_SIZE / sizeof(uint32_t)), 

    //describe the total number of the pages of the virtual memory
    VM_PAGEABLE_PAGES_NUM = PAGE_TABLE_ENTRIES_NUM,

    // Global bit
    PTE_G = (0x40 >> 6),
    // Valid bit
    PTE_V = (0x80 >> 6),
    // Writable bit
    PTE_D = (0x100 >> 6),
    // Uncache bit
    PTE_C = (0x400 >> 6),
    // swaped bit
    PTE_S = (0x1000 >> 6),

    //swap division : 4K:32M - 32M+4K
    SD_SWAP_DIVISION = 0x1000,
    SD_SWAP_UNIT = 0X1000,
    SD_SWAP_UNIT_NUM = 0x1,

    /* Constants to simulate a very small physical memory. */
    PAGEABLE_PAGES = PAGE_TABLE_SIZE / PAGE_SIZE + 0x800 
                     + SD_SWAP_UNIT_NUM,
    //TASK 1 setting:
    //virtual addr: 0 - 8M 
    //physical addr: 20M - 28M + 4K
    VM_SIZE = 0x800000,

    //page frame
    PAGE_FRAME_START = 0xa1000000,

    //tlb
    TLB_ENTRIES_NUM = 32,

};

typedef struct page_map_entry {
    uint32_t paddr; 
    uint32_t vaddr;    
    uint32_t VPN;
    uint32_t PFN;    
    pid_t    pid;
    int      index;    
    bool_t   avail;
    bool_t   dirty;
    bool_t   pinned;
    bool_t   swaped;
    int      swap_index;
    uint32_t R;
} page_map_entry_t;

typedef struct tlb_entry {
    uint32_t index;
    bool_t   empty;
    uint32_t VPN2;
    uint32_t PFN0;
    uint32_t PFN1;
} tlb_entry_t;

typedef uint32_t page_directory_entry_t;

typedef uint32_t page_table_entry_t;

//global pointer to the level one page table base address
// extern page_directory_entry_t page_dir[PAGE_DIR_ENTRIES];

// extern page_table_entry_t page_table[PAGE_TABLE_ENTRIES];

void init_memory();
// void do_TLB_Refill();
// void do_page_fault();


typedef struct swap_map_entry {
    uint32_t PFN;
    uint32_t paddr;
    uint32_t pid;
    bool_t   avail;
    uint32_t index;
} swap_map_entry_t;


// extern swap_map_entry_t swap_table[SD_SWAP_UNIT_NUM];

void sdread(unsigned char *buf, unsigned int base, int n);
void sdwrite(unsigned char *buf, unsigned int base, int n);

extern void tlb_flush(int EntryHi);
extern void tlb_flush_no_check();

extern uint32_t get_cp0_status();
extern void set_cp0_status(uint32_t cp0_status);
extern uint32_t get_asid();
extern void set_entryhi_asid(uint32_t asid);
extern uint32_t get_cp0_index();
extern void set_cp0_index(uint32_t index);
extern uint32_t get_cp0_entryhi();
extern void set_cp0_entryhi(uint32_t cp0_entryhi);
extern uint32_t get_cp0_badvaddr();
extern uint32_t get_cp0_entrylo0();
extern void set_cp0_entrylo0(uint32_t cp0_entrylo0);
extern uint32_t get_cp0_entrylo1();
extern void set_cp0_entrylo1(uint32_t cp0_entrylo1);
extern uint32_t get_cp0_pagemask();
extern void set_cp0_pagemask(uint32_t cp0_pagemask);

void handle_tlb_exception_helper();

//for debug
void read_tlb_1();
void read_page_map();

#endif
