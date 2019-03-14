#ifndef INCLUDE_MM_H_
#define INCLUDE_MM_H_

#include "type.h"
#include "sched.h"

enum {
    /* physical page facts */
    PAGE_SIZE = 0x1000, //4KB

    PAGE_TABLE_SIZE = 0x200000, //16M - 18M => 512 PAGES => 2MB => 0 - 2G VM 
    PAGE_TABLE_PAGES = (PAGE_TABLE_SIZE / PAGE_SIZE),
    PAGE_TABLE_ENTRIES_NUM = (PAGE_TABLE_SIZE / sizeof(uint32_t)), 

    // Global bit
    PTE_G = (0x40 >> 6),
    // Valid bit
    PTE_V = (0x80 >> 6),
    // Writable bit
    PTE_D = (0x100 >> 6),
    // Uncache bit
    PTE_C = (0x400 >> 6),
    // // swaped bit
    // PTE_S = (0x1000 >> 6),

    VM_SIZE = 0x80000000, //0 - 2GB

    /***********************P4-3 TEST***********************/
    /*test condition 0                                     */
    FRAME_SIZE = 0x800000, //16M - 24M => 2048 PAGES => 8MB 
    FRAME_PAGES = FRAME_SIZE / PAGE_SIZE,
    /*******************************************************/
    SD_SWAP_DIVISION_START = 0x2000000, //32M
    SD_SWAP_DIVISION = 0x1000,
    SD_SWAP_UNIT = 0X1000,
    SD_SWAP_UNIT_NUM = SD_SWAP_DIVISION / SD_SWAP_UNIT,
    /*******************************************************/

    // /********************SD SWAP TEST***********************/
    // /*test condition 1: invalid TLB, PTE empty, no free PFN*/
    // FRAME_SIZE = PAGE_TABLE_SIZE + PAGE_SIZE, 
    // FRAME_PAGES = (FRAME_SIZE / PAGE_SIZE), // == 512 + 1
    // /*******************************************************/
    // SD_SWAP_DIVISION_START = 0x2000000, //32M
    // SD_SWAP_DIVISION = 0x2000,
    // SD_SWAP_UNIT = 0X1000,
    // SD_SWAP_UNIT_NUM = (SD_SWAP_DIVISION / SD_SWAP_UNIT), // == 2
    // /*******************************************************/
    // /*******************************************************/
    // /*test condition 2: invalid TLB, PTE not empty, no free PFN*/
    // FRAME_SIZE = PAGE_TABLE_SIZE + PAGE_SIZE, 
    // FRAME_PAGES = FRAME_SIZE / PAGE_SIZE, // == 512 + 1
    // /*******************************************************/
    // SD_SWAP_DIVISION_START = 0x2000000, //32M
    // SD_SWAP_DIVISION = 0x2000,
    // SD_SWAP_UNIT = 0X1000,
    // SD_SWAP_UNIT_NUM = SD_SWAP_DIVISION / SD_SWAP_UNIT, // == 2
    // /*******************************************************/

    //describe the total number of the pages of the virtual memory
    VM_PAGES = (VM_SIZE / PAGE_SIZE),
    //VM_PAGES == PAGE_TABLE_ENTRIES_NUM

    /* Constants to simulate a very small physical memory. */
    // PAGEABLE_PAGES = PAGE_TABLE_PAGES + FRAME_PAGES + SD_SWAP_UNIT_NUM,
    PAGEABLE_PAGES = FRAME_PAGES + SD_SWAP_UNIT_NUM,

    //page frame
    PAGE_FRAME_START = 0xa1000000, //16M

    //tlb
    TLB_ENTRIES_NUM = 32, //0-31

};

extern int tlb_refill_count;
extern int tlb_invalid_count;

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
    bool_t   R;
} page_map_entry_t;

typedef struct tlb_entry {
    uint32_t index;
    bool_t   empty;
    uint32_t VPN2;
    uint32_t PFN0;
    uint32_t PFN1;
    uint32_t pid;
} tlb_entry_t;

typedef uint32_t page_directory_entry_t;

typedef uint32_t page_table_entry_t;

//global pointer to the level one page table base address
// extern page_directory_entry_t page_dir[PAGE_DIR_ENTRIES];

// extern page_table_entry_t page_table[PAGE_TABLE_ENTRIES];

void init_memory();
// void do_TLB_Refill();
// void do_page_fault();

// uint32_t page_frame_paddr(uint32_t index);
// uint32_t page_frame_vaddr(uint32_t index);
// uint32_t pa_2_va(uint32_t pa);
// uint32_t va_2_pa(uint32_t va);
// uint32_t page_alloc(bool_t pinned);
// uint32_t get_user_stack_top();

typedef struct swap_map_entry {
    uint32_t PFN;
    uint32_t paddr;
    uint32_t pid;
    bool_t   avail;
    uint32_t index;
} swap_map_entry_t;

// extern int page_alloc_ptr;

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

//for debug
extern uint32_t get_sp_reg();
extern void print_sp_reg();


extern void close_clock_interrupt();
extern void enable_clock_interrupt();

void handle_tlb_exception_helper();

//for debug
void read_tlb_1();
void read_page_map();

#endif
