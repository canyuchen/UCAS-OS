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

//global page frame pointer (only in memory!!!)
uint32_t page_frame_base_ptr = PAGE_FRAME_START;

static uint32_t free_page_frame_num = FRAME_PAGES;

//global vitual memory pointer, 0 - 2G VM
uint32_t vm_base_ptr = 0;

tlb_entry_t tlb_table[TLB_ENTRIES_NUM];

static int tlb_entry_index_ptr = 0;

// extern int page_alloc_ptr;
static int page_alloc_ptr = 0;

static uint32_t wrong_pid;

int tlb_refill_count = 0;
int tlb_invalid_count = 0;

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
    for(i = page_alloc_ptr; i < FRAME_PAGES; i++){
    // for(i = 0; i < FRAME_PAGES; i++){
        if(page_map[i].avail == 1){
            flag_find_free_page = 1;
            free_index = i;
        
            // vt100_move_cursor(1, 49);
            // printk("%d %d %d %d %d %d %d %d", page_table_base_ptr, free_index, page_map[0].avail, page_map[1].avail, page_map[2].avail, page_map[3].avail, page_map[4].avail, page_map[5].avail);

            page_alloc_ptr = (page_alloc_ptr + 1) % FRAME_PAGES;
            
            //BUG!!!!!!!!!!!!!!!!!!
            break;

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
        page_map[free_index].pid = PID;

        // vt100_move_cursor(1, 49);
        // printk("%d %d %d %d %d %d %d", free_index, page_map[0].avail, page_map[1].avail, page_map[2].avail, page_map[3].avail, page_map[4].avail, page_map[5].avail);


    }
    else{
        page_map[free_index].vaddr = current_running->user_context.cp0_badvaddr;
        page_map[free_index].VPN = ((page_map[free_index].vaddr & 0xfffff000) >> 12);
        page_map[free_index].pid = current_running->pid;
        page_map[free_index].avail = 0;
        page_map[free_index].pinned = pinned;
        page_map[free_index].R = 1;
    }

    if(free_page_frame_num > 0){
        free_page_frame_num--;
    }

    return free_index;
}

void set_up_page_table()
{
    //page map index for a page table  
    bool_t pinned = TRUE;
    int i = 0;
    while(i < PAGE_TABLE_PAGES){
        page_alloc(pinned);
        i++;
    }
}

/*
//TASK 1 initialization
void fill_page_table()
{
    int i = 0;
    uint32_t *page_table = (uint32_t *)page_table_base_ptr;
    for(; i < VM_SIZE / PAGE_SIZE; i++){
        page_table[i] = (page_map[i + PAGE_TABLE_SIZE / PAGE_SIZE].paddr & 0xfffff000) 
                      | PTE_G | PTE_V | PTE_D | PTE_C;
        page_map[i + PAGE_TABLE_SIZE / PAGE_SIZE].VPN = i;
        page_map[i + PAGE_TABLE_SIZE / PAGE_SIZE].vaddr = i * PAGE_SIZE;
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
*/

static void fill_tlb()
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
        // VPN2 = (((2 * i * PAGE_SIZE) & 0xffffe000) >> 13);
        VPN2 = 0xffff7;
        cp0_entryhi = (VPN2 << 13);
        cp0_pagemask = 0;
        PFN0 = 0;
        PFN1 = 0;
        // cp0_entrylo0 = (PFN0 << 6) | PTE_G | PTE_V | PTE_D | PTE_C;
        // cp0_entrylo1 = (PFN1 << 6) | PTE_G | PTE_V | PTE_D | PTE_C;

        // init TLB as invalid!!!!!!!
        cp0_entrylo0 = 0;
        cp0_entrylo1 = 0;
        cp0_index = i;

        tlb_table[i].VPN2 = VPN2;
        tlb_table[i].PFN0 = PFN0;
        tlb_table[i].PFN1 = PFN1;
        tlb_table[i].pid  = 0;

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
    sys_move_cursor(1, 14);
    printf("Em \t Id \t V2 \t P0 \t P1");
    // for(; i < TLB_ENTRIES_NUM; i++){
    for(; i < 16; i++){
        sys_move_cursor(1, 15+i);
        printf("                                      ");
        sys_move_cursor(1, 15+i);
        printf("%d \t", tlb_table[i].empty);
        printf("%d \t", tlb_table[i].index);
        printf("%d \t", tlb_table[i].VPN2);
        printf("%d \t", tlb_table[i].PFN0);
        printf("%d ", tlb_table[i].PFN1);
    }
}


//for debug
/*
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
*/

void init_memory()
{
    //init page frames 
    int i;
    for(i = 0; i < FRAME_PAGES; i++){
        page_map[i].paddr = page_frame_paddr(i);
        page_map[i].vaddr = 0;        
        page_map[i].VPN = 0;
        page_map[i].PFN = ((page_map[i].paddr & 0xfffff000) >> 12);
        page_map[i].pid = 0;
        page_map[i].index = i;
        page_map[i].avail = 1;
        page_map[i].dirty = 1;
        page_map[i].pinned = 0;
        page_map[i].swaped = 0;
        page_map[i].swap_index = -1;
        page_map[i].R = 0;
    }

    for(; i < PAGEABLE_PAGES; i++){
        page_map[i].paddr = 0;
        page_map[i].vaddr = 0;        
        page_map[i].VPN = 0;
        page_map[i].PFN = 0;
        page_map[i].pid = 0;
        page_map[i].index = i;
        page_map[i].avail = 1;
        page_map[i].dirty = 1;
        page_map[i].pinned = 0;
        page_map[i].swaped = 0;
        page_map[i].swap_index = 1;
        page_map[i].R = 0;
    }
    //arrange n pinned page frame for global page table
    set_up_page_table();
    page_table_base_ptr = PAGE_FRAME_START;

    // bzero((uint32_t *)(PAGE_FRAME_START + PAGE_TABLE_SIZE), VM_SIZE);


    //TASK 1
    // fill_page_table();

    fill_tlb();


    // fill_page_table();

}

/*
uint32_t get_user_stack_top()
{
	bool_t pinned = 0;
	uint32_t frame_index = page_alloc(pinned);
	uint32_t user_stack_top = page_frame_vaddr(frame_index);

    return user_stack_top;
}
*/

void print()
{
	vt100_move_cursor(1, 1);
	printk("init_exception");
}

static bool_t check_tlb_wrong_access()
{
    uint32_t VPN2 = ((get_cp0_entryhi() & 0xffffe000) >> 13);

    int i = 0;
    while(i < TLB_ENTRIES_NUM){
        if(tlb_table[i].VPN2 == VPN2 && tlb_table[i].pid != current_running->pid){
            wrong_pid = tlb_table[i].pid;
            return 1;
        }
        i++;
    }
    return 0;
}

static uint32_t get_tlb_invalid_index()
{
    uint32_t VPN2 = ((get_cp0_entryhi() & 0xffffe000) >> 13);

    int i = 0;
    while(i < TLB_ENTRIES_NUM){
        if(tlb_table[i].VPN2 == VPN2 && tlb_table[i].pid == current_running->pid){
            return i;
        }
        i++;
    }
}

void handle_tlb_exception_helper()
{

    vt100_move_cursor(1, 47);
    printk("DEBUG >> ");
    printk("%d %d", ((uint32_t *)page_table_base_ptr)[0], ((uint32_t *)page_table_base_ptr)[1]);

	vt100_move_cursor(1, 48);
    printk("%d %d %d", page_alloc_ptr, page_map[page_alloc_ptr].paddr, page_map[page_alloc_ptr].PFN);
    vt100_move_cursor(1, 49);
    printk("%d %d %d", 0, page_map[0].paddr, page_map[0].PFN);

    uint32_t cp0_pagemask = 0;
    uint32_t EntryHi = get_cp0_entryhi();
    uint32_t badvpn2 = ((EntryHi & 0xffffe000) >> 13);
    uint32_t badvaddr = get_cp0_badvaddr();
    uint32_t EntryLo_is_odd = ((badvaddr & 0x1000) >> 12);
    uint32_t badvpn = ((badvaddr & 0xfffff000) >> 12);
    uint32_t *page_table = (uint32_t *)page_table_base_ptr;
    // uint32_t *badpte_ptr = &(page_table[badvpn]);

	// vt100_move_cursor(1, 1);
    // printk("%d ", EntryHi);

    uint32_t pid = current_running->pid;

    asm volatile("tlbp");
    uint32_t index = get_cp0_index();
    //TLB refill handler
    if(((index & 0x80000000) >> 31) == 1){

        vt100_move_cursor(1, 46);
        printk("DEBUG >> ");
        printk("%d", 111);

        tlb_refill_count++;
        
        //check whether have the right to have access to the virtual address
        if(check_tlb_wrong_access() == 1){
            vt100_move_cursor(1, 45);
            printk("ERROR! Current running pid is %d, but virtual address is in pid %d address space", pid, wrong_pid);
            return;
        }
        
        //PTE is not empty and PFN is in memory
        if(page_table[badvpn] != 0 && ((page_table[badvpn] & PTE_V) == PTE_V)){

            vt100_move_cursor(1, 46);
            printk("DEBUG >> ");
            printk("%d", 222);

            uint32_t PFN = ((page_table[badvpn] & 0xfffff000) >> 12);

            if(EntryLo_is_odd == 1){
                uint32_t EntryLo1 = (PFN << 6) | PTE_C | PTE_D | PTE_V;
                set_cp0_entrylo1(EntryLo1);
            }
            else{
                uint32_t EntryLo0 = (PFN << 6) | PTE_C | PTE_D | PTE_V;
                set_cp0_entrylo0(EntryLo0);                
            }
            set_cp0_pagemask(cp0_pagemask);

            tlb_entry_index_ptr = (tlb_entry_index_ptr + 1) % TLB_ENTRIES_NUM;
            set_cp0_index(tlb_entry_index_ptr);      

            EntryHi = ((EntryHi & 0xffffe000) | pid);
            set_cp0_entryhi(EntryHi);

            asm volatile("tlbwi");

            tlb_table[tlb_entry_index_ptr].pid = current_running->pid;
            tlb_table[tlb_entry_index_ptr].empty = 0;
            tlb_table[tlb_entry_index_ptr].index = tlb_entry_index_ptr;
            if(EntryLo_is_odd == 1){
                tlb_table[tlb_entry_index_ptr].PFN1 = ((get_cp0_entrylo1() & 0xffffffc0) >> 6);
            }
            else{
                tlb_table[tlb_entry_index_ptr].PFN0 = ((get_cp0_entrylo0() & 0xffffffc0) >> 6);
            }
            tlb_table[tlb_entry_index_ptr].VPN2 = ((get_cp0_entryhi() & 0xffffe000) >> 13);

            return;
        }
        //PTE is not empty but PFN is in swap
        else if(page_table[badvpn] != 0 && ((page_table[badvpn] & PTE_V) == 0)){
            vt100_move_cursor(1, 46);
            printk("DEBUG >> ");
            printk("%d", 333);

        }
        //PTE is empty 
        else if(page_table[badvpn] == 0){
    
            vt100_move_cursor(1, 46);
            printk("DEBUG >> ");
            printk("%d", 444);

            if(free_page_frame_num != 0){
                bool_t pinned = 0;
                int index = page_alloc(pinned);
                uint32_t PFN = ((page_map[index].paddr & 0xfffff000) >> 12);
                page_table[badvpn] = (PFN << 12) | PTE_C | PTE_D | PTE_V;

                vt100_move_cursor(1, 50);
                printk("%d %d %d %d %d", page_alloc_ptr, page_map[page_alloc_ptr].paddr, page_map[page_alloc_ptr].PFN, index, PFN);

                if(EntryLo_is_odd == 1){
                    uint32_t EntryLo1 = (PFN << 6) | PTE_C | PTE_D | PTE_V;
                    set_cp0_entrylo1(EntryLo1);
                }
                else{
                    uint32_t EntryLo0 = (PFN << 6) | PTE_C | PTE_D | PTE_V;
                    set_cp0_entrylo0(EntryLo0);                
                }
                set_cp0_pagemask(cp0_pagemask);

                tlb_entry_index_ptr = (tlb_entry_index_ptr + 1) % TLB_ENTRIES_NUM;
                set_cp0_index(tlb_entry_index_ptr);      

                EntryHi = ((EntryHi & 0xffffe000) | pid);
                set_cp0_entryhi(EntryHi);

                asm volatile("tlbwi");

                tlb_table[tlb_entry_index_ptr].pid = current_running->pid;
                tlb_table[tlb_entry_index_ptr].empty = 0;
                tlb_table[tlb_entry_index_ptr].index = tlb_entry_index_ptr;
                if(EntryLo_is_odd == 1){
                    tlb_table[tlb_entry_index_ptr].PFN1 = ((get_cp0_entrylo1() & 0xffffffc0) >> 6);
                }
                else{
                    tlb_table[tlb_entry_index_ptr].PFN0 = ((get_cp0_entrylo0() & 0xffffffc0) >> 6);
                }
                tlb_table[tlb_entry_index_ptr].VPN2 = ((get_cp0_entryhi() & 0xffffe000) >> 13);
                
                return;
            }
            else{
  
            }
        }
    }
    //TLB invalid handler
    else{
        vt100_move_cursor(1, 46);
        printk("DEBUG >> ");
        printk("%d", 555);

        tlb_invalid_count++;
        
        //PTE is empty
        if(page_table[badvpn] == 0){
            vt100_move_cursor(1, 46);
            printk("DEBUG >> ");
            printk("%d", 666);

            if(free_page_frame_num != 0){
                bool_t pinned = 0;
                int index = page_alloc(pinned);
                uint32_t PFN = ((page_map[index].paddr & 0xfffff000) >> 12);
                page_table[badvpn] = (PFN << 12) | PTE_C | PTE_D | PTE_V;

                vt100_move_cursor(1, 50);
                printk("%d %d %d %d %d", page_alloc_ptr, page_map[page_alloc_ptr].paddr, page_map[page_alloc_ptr].PFN, index, PFN);

                uint32_t tlb_invalid_index = get_tlb_invalid_index();
                set_cp0_index(tlb_invalid_index); 

                uint32_t EntryLo1, EntryLo0;
                if(EntryLo_is_odd == 1){
                    EntryLo1 = (PFN << 6) | PTE_C | PTE_D | PTE_V;
                    EntryLo0 = (tlb_table[tlb_invalid_index].PFN0 << 6) | PTE_C | PTE_D | PTE_V;
                }
                else{
                    EntryLo0 = (PFN << 6) | PTE_C | PTE_D | PTE_V;
                    EntryLo1 = (tlb_table[tlb_invalid_index].PFN1 << 6) | PTE_C | PTE_D | PTE_V;
                }
                set_cp0_entrylo0(EntryLo0); 
                set_cp0_entrylo1(EntryLo1);

                set_cp0_pagemask(cp0_pagemask);     

                EntryHi = ((EntryHi & 0xffffe000) | pid);
                set_cp0_entryhi(EntryHi);

                asm volatile("tlbwi");

                tlb_table[tlb_invalid_index].pid = current_running->pid;
                tlb_table[tlb_invalid_index].empty = 0;
                tlb_table[tlb_invalid_index].index = tlb_invalid_index;
                if(EntryLo_is_odd == 1){
                    tlb_table[tlb_invalid_index].PFN1 = ((get_cp0_entrylo1() & 0xffffffc0) >> 6);
                }
                else{
                    tlb_table[tlb_invalid_index].PFN0 = ((get_cp0_entrylo0() & 0xffffffc0) >> 6);
                }
                tlb_table[tlb_invalid_index].VPN2 = ((get_cp0_entryhi() & 0xffffe000) >> 13);
                
                return;
            }
            else{
  
            }            
        }
        //PTE is not empty, but PFN is in swap
        else if(page_table[badvpn] != 0 && ((page_table[badvpn] & PTE_V) == 0)){
            vt100_move_cursor(1, 46);
            printk("DEBUG >> ");
            printk("%d", 777);


        }


    }
}

void print_sp_reg()
{
    uint32_t sp = get_sp_reg();
	vt100_move_cursor(2, 2);
    printk("%d ", sp);
}


