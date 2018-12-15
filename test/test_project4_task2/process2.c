#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "time.h"
#include "screen.h"
#include "test4_2.h"
#include "stdarg.h"
#include "string.h"
#include "mm.h"
#include "scanf.h"

static int rand()
{	
	int current_time = get_ticks();
	return current_time % 1000;
}

static void scanf(int *mem1)
{
    int re;
    while((re = sys_scanf(mem1)) != '\r'){
    //while((re = do_scanf(mem1)) != '\r'){
        printf("%c", re);
    }
}

void rw_task4_2(void)
{

    sys_move_cursor(1, 13);
    read_tlb_1();

    sys_move_cursor(1, 11);
    printf("tlb_refill_count : %d", tlb_refill_count);
    sys_move_cursor(1, 12);
    printf("tlb_invalid_count : %d", tlb_invalid_count);

	// int mem1, mem2, mem3, mem4;
	// int memory[4];
	// int i = 0;
    // int ch;
	uint32_t mem1, mem2, mem3, mem4;
	uint32_t memory[4];
	uint32_t i = 0;
    uint32_t ch;

    init_InputBuffer_s(inputBuffer_s_ptr);

    // close_clock_interrupt();

    sys_move_cursor(1, 0);
    printf("Write Address 1 : ");
    scanf(&mem1);

    // enable_clock_interrupt();

    sys_move_cursor(1, 1);
    mem2 = rand();
    memory[0] = mem2;
    printf("Write Number : %d", mem2);

    sys_move_cursor(1, 2);
    *(uint32_t *)mem1 = mem2;
    printf("%d is written to virtual addr 1 : %d", mem2, mem1);

    sys_move_cursor(1, 13);
    read_tlb_1();

    sys_move_cursor(1, 11);
    printf("tlb_refill_count : %d", tlb_refill_count);
    sys_move_cursor(1, 12);
    printf("tlb_invalid_count : %d", tlb_invalid_count);

    // close_clock_interrupt();

    sys_move_cursor(1, 3);
    printf("Write Address 2 : ");
    scanf(&mem3);

    // enable_clock_interrupt();

    sys_move_cursor(1, 4);
    mem4 = rand();
    memory[1] = mem4;
    printf("Write Number : %d", mem4);

    sys_move_cursor(1, 5);
    *(uint32_t *)mem3 = mem4;
    printf("%d is written to virtual addr 2 : %d", mem4, mem3);

    sys_move_cursor(1, 13);
    read_tlb_1();
    
    sys_move_cursor(1, 11);
    printf("tlb_refill_count : %d", tlb_refill_count);
    sys_move_cursor(1, 12);
    printf("tlb_invalid_count : %d", tlb_invalid_count);

    // close_clock_interrupt();

    sys_move_cursor(1, 6);
    printf("Read Address 1 : ");
    scanf(&mem1);

    // enable_clock_interrupt();

    sys_move_cursor(1, 7);
    memory[2] = *(uint32_t *)mem1;
    if(memory[2] == memory[0])
        printf("Read succeed: %d", memory[2]);
    else
        printf("Read error: %d", memory[2]);

    // close_clock_interrupt();

    sys_move_cursor(1, 8);
    printf("Read Address 2 : ");
    scanf(&mem3);

    // enable_clock_interrupt();

    sys_move_cursor(1, 9);
    memory[3] = *(uint32_t *)mem3;
    if(memory[3] == memory[1])
        printf("Read succeed: %d", memory[3]);
    else
        printf("Read error: %d", memory[3]);

    sys_move_cursor(1, 13);
    read_tlb_1();

	while(1);
	//Only input address.
	//Achieving input r/w command is recommended but not required.
}

