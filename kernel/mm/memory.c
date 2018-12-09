#include "mm.h"
#include "string.h"
#include "sched.h"

//TODO:Finish memory management functions here refer to mm.h and add any functions you need.


//global pointer to the level one page table base address
// page_directory_entry_t page_dir[PAGE_DIR_ENTRIES];

// Keep track of all page frames (in memory and in swap division): their vaddr, status, and other properties
static page_map_entry_t page_map[PAGEABLE_PAGES];

// Keep track of all units in swap divsion: their status, and other properties
static swap_map_entry_t swap_table[SD_SWAP_UNIT_NUM];

//global page table pointer
uint32_t page_table_base_ptr = 0;

//global page frame pointer
uint32_t page_frame_base_ptr = PAGE_FRAME_START;

//global vitual memory pointer
uint32_t vm_base_ptr = 0;

tlb_entry_t tlb_table[TLB_ENTRIES_NUM];

static int tlb_entry_index_ptr = 0;

/* get the physical address from virtual address (in kernel) */
uint32_t va_2_pa(uint32_t va) 
{
    return (uint32_t) va - 0xa0000000;
}

/* get the virtual address (in kernel) from physical address */
uint32_t pa_2_va(uint32_t pa) 
{
    return (uint32_t) pa + 0xa0000000;
}

/* TODO: Returns virtual address (in kernel) of page frame number index */
uint32_t page_frame_vaddr(uint32_t index)
{
    return PAGE_FRAME_START + index * PAGE_SIZE;
}   

uint32_t page_frame_paddr(uint32_t index)
{
    return va_2_pa(page_frame_vaddr(index));
}

uint32_t get_swap_in_index()
{
    static int i = 0;
    while(i < SD_SWAP_UNIT_NUM && swap_table[i].avail == 0){
        i = (i + 1) % SD_SWAP_UNIT_NUM;
    }
    swap_table[i].avail = 0;
    return i;
}

void do_page_swap(uint32_t swap_out_index, uint32_t swap_in_index)
{

}

//Page Fault handler
uint32_t page_alloc(bool_t pinned)
{
    int free_index;
    int swap_out_index, swap_in_index;
    bool_t flag_find_free_page = 0;
    static int clock_ptr = 0;
    int i;
    for(i = 0; i < PAGEABLE_PAGES; i++){
        if(page_map[i].avail == 1){
            flag_find_free_page = 1;
            free_index = i;
            // return free_index;
        }
    }
/*
    if(flag_find_free_page == 0){
        while(page_map[clock_ptr].pinned == 1 || page_map[clock_ptr].R == 1){
            if(page_map[clock_ptr].R == 1){
                page_map[clock_ptr].R = 0;
            }
            clock_ptr = (clock_ptr + 1) % PAGEABLE_PAGES;
        }
        swap_out_index = clock_ptr;
        swap_in_index = get_swap_in_index();

        //revised PTE accordingly
        int pid = page_map[clock_ptr].pid;
        int VPN2 = page_map[clock_ptr].vaddr & 0xffffe000;
        uint32_t *page_table = (uint32_t *)page_table_base_ptr;
        page_table[page_map[clock_ptr].vaddr >> 12] = 

        //flush TLB accordingly
        // int EntryHi = VPN2 | pid;
        // tlb_flush(EntryHi);

        do_page_swap(swap_out_index, swap_in_index);

        page_map[swap_out_index].swaped = 1;
        page_map[swap_in_index].swaped = 0;

        free_index = swap_in_index;
    }
*/
    bzero((uint32_t *)pa_2_va(page_map[free_index].paddr), PAGE_SIZE);

    if(page_table_base_ptr == 0){
        page_map[free_index].avail = 0;
        page_map[free_index].pinned = pinned;
        page_map[free_index].R = 1;
    }
    else{
        page_map[free_index].vaddr = current_running->user_context.cp0_badvaddr;
        page_map[free_index].VPN = page_map[free_index].vaddr >> 12;
        page_map[free_index].pid = current_running->pid;
        page_map[free_index].avail = 0;
        page_map[free_index].pinned = pinned;
        page_map[free_index].R = 1;
    }

    return free_index;
}

void set_up_page_table()
{
    //page map index for a page table  
    bool_t pinned = TRUE;
    int i = 0;
    while(i < PAGE_TABLE_SIZE / PAGE_SIZE){
        page_alloc(pinned);
        i++;
    }
}

//TASK 1 initialization
void fill_page_table()
{
    int i = 0;
    uint32_t *page_table = (uint32_t *)page_table_base_ptr;
    for(; i < VM_SIZE / PAGE_SIZE; i++){
        page_table[i] = (page_map[i].paddr & 0xfffff000) 
                      | PTE_G | PTE_V | PTE_D | PTE_C;
        page_map[i].VPN = i;
        page_map[i].vaddr = i * PAGE_SIZE;
    }
}

//TASK 1 initialization
void fill_tlb()
{
    int i = 0;
    uint32_t cp0_entryhi;
    uint32_t cp0_pagemask;
    uint32_t cp0_entrylo0;
    uint32_t cp0_entrylo1;
    uint32_t cp0_index;
    uint32_t *page_table = (uint32_t *)page_table_base_ptr;
    uint32_t VPN2;
    uint32_t PFN0;
    uint32_t PFN1;

    for(; i < TLB_ENTRIES_NUM; i++){
        tlb_table[i].empty = 1;
        tlb_table[i].index = i;

        // VPN2 = (2 * i * PAGE_SIZE) & 0xffffe000 >> 13;
        // VPN2 = ((2 * i * PAGE_SIZE) & 0xffffe000) >> 13;
        // BUG!!!
        VPN2 = (((2 * i * PAGE_SIZE) & 0xffffe000) >> 13);
        cp0_entryhi = (VPN2 << 13);
        cp0_pagemask = 0;
        PFN0 = ((page_table[VPN2 << 1] & 0xfffff000) >> 12);
        PFN1 = ((page_table[(VPN2 << 1) + 1] & 0xfffff000) >> 12);
        cp0_entrylo0 = (PFN0 << 6) | PTE_G | PTE_V | PTE_D | PTE_C;
        cp0_entrylo1 = (PFN1 << 6) | PTE_G | PTE_V | PTE_D | PTE_C;
        cp0_index = i;

        tlb_table[i].VPN2 = VPN2;
        tlb_table[i].PFN0 = PFN0;
        tlb_table[i].PFN1 = PFN1;

        set_cp0_entryhi(cp0_entryhi);
        set_cp0_pagemask(cp0_pagemask);
        set_cp0_entrylo0(cp0_entrylo0);
        set_cp0_entrylo1(cp0_entrylo1);
        set_cp0_index(cp0_index);

        asm volatile("tlbwi");
    }
    
}

//for debug
void read_tlb_1()
{
    int i = 0;

    for(; i < TLB_ENTRIES_NUM; i++){
        printf("\n%d ", tlb_table[i].empty);
        printf("%d ", tlb_table[i].index);
        printf("%d ", tlb_table[i].VPN2);
        printf("%d ", tlb_table[i].PFN0);
        printf("%d ", tlb_table[i].PFN1);
    }
}

//for debug
void read_page_map()
{
    int i = 0;

    for(; i < TLB_ENTRIES_NUM; i++){
        printf("\n%d ", tlb_table[i].empty);
        printf("%d ", tlb_table[i].index);
        printf("%d ", tlb_table[i].VPN2);
        printf("%d ", tlb_table[i].PFN0);
        printf("%d ", tlb_table[i].PFN1);
    }  
}

void init_memory()
{
    //init page frames 
    int i;
    for(i = 0; i < PAGEABLE_PAGES; i++){
        page_map[i].paddr = page_frame_paddr(i);
        page_map[i].vaddr = 0;        
        page_map[i].VPN = 0;
        page_map[i].PFN = page_map[i].paddr >> 12;
        page_map[i].pid = 0;
        page_map[i].index = i;
        page_map[i].avail = 1;
        page_map[i].dirty = 1;
        page_map[i].pinned = 0;
        page_map[i].swaped = 0;
        page_map[i].swap_index = -1;
        page_map[i].R = 0;
    }

    //arrange n pinned page frame for global page table
    set_up_page_table();
    page_table_base_ptr = PAGE_FRAME_START;

    bzero((uint32_t *)(PAGE_FRAME_START + PAGE_TABLE_SIZE), VM_SIZE);

    fill_page_table();

    fill_tlb();

}

/*
void handle_tlb_exception_helper()
{
    static int tlb_refill_count = 0;
    static int tlb_invalid_count = 0;

    uint32_t EntryHi = get_cp0_entryhi();
    uint32_t badvpn2 = EntryHi >> 13;
    uint32_t badvaddr = get_cp0_badvaddr();
    uint32_t EntryLo_is_odd_offset = (badvaddr & 0x1000) >> 12;

    uint32_t 
    
}
*/
