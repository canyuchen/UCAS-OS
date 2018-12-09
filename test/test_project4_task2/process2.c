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

void rw_task4_2(void)
{
	int mem1, mem2, mem3, mem4, mem5;
	int memory[4];
	int i = 0;
    int re;
    int ch;

    init_InputBuffer_s(inputBuffer_s_ptr);

    sys_move_cursor(1, 0);
    printf("Write Address 1 : ");
    while((re = sys_scanf(&mem1)) != '\r'){
        printf("%c", re);
    }

    // printf("   %d", *(inputBuffer_s_ptr->buffer));
    // printf("   %d", *(inputBuffer_s_ptr->buffer + 1));
    // printf("   %d", atoi(inputBuffer_s_ptr->buffer ));
    // sys_scanf(&mem1);

    sys_move_cursor(1, 1);
    mem2 = rand();
    memory[0] = mem2;
    printf("Write Number : %d", mem2);

    sys_move_cursor(1, 2);
    *(int *)mem1 = mem2;
    printf("%d is written to virtual addr 1 : %d", mem2, mem1);

    // printf("            %d", inputBuffer_s_ptr->pointer);

    sys_move_cursor(1, 3);
    printf("Write Address 2 : ");
    while((re = sys_scanf(&mem3)) != '\r'){
        printf("%c", re);
    }

    // printf("            %d", inputBuffer_s_ptr->pointer);

    sys_move_cursor(1, 4);
    mem4 = rand();
    memory[1] = mem4;
    printf("Write Number : %d", mem4);

    sys_move_cursor(1, 5);
    *(int *)mem3 = mem4;
    printf("%d is written to virtual addr 2 : %d", mem4, mem3);

    // printf("            %d", inputBuffer_s_ptr->pointer);

    sys_move_cursor(1, 6);
    printf("Read Address 1 : ");
    while((re = sys_scanf(&mem1)) != '\r'){
        printf("%c", re);
    }

    // printf("            %d", index);

    sys_move_cursor(1, 7);
    memory[2] = *(int *)mem1;
    if(memory[2] == memory[0])
        printf("Read succeed: %d", memory[2]);
    else
        printf("Read error: %d", memory[2]);

    sys_move_cursor(1, 8);
    printf("Read Address 2 : ");
    while((re = sys_scanf(&mem1)) != '\r'){
        printf("%c", re);
    }

    sys_move_cursor(1, 9);
    memory[3] = *(int *)mem1;
    if(memory[3] == memory[1])
        printf("Read succeed: %d", memory[3]);
    else
        printf("Read error: %d", memory[3]);

    sys_move_cursor(1, 10);
    read_tlb_1();
    // read_page_map();

	while(1);
	//Only input address.
	//Achieving input r/w command is recommended but not required.
}

