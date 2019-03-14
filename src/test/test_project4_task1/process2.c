#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "time.h"
#include "screen.h"
#include "test4_1.h"
#include "stdarg.h"
#include "string.h"
#include "mm.h"
#include "scanf.h"

#define RW_TIMES 2

static int rand()
{	
	int current_time = get_ticks();
	return current_time % 1000;
}

// static void disable_interrupt()
// {
//     uint32_t cp0_status = get_cp0_status();
//     cp0_status &= 0xfffffffe;
//     set_cp0_status(cp0_status);
// }

// static void enable_interrupt()
// {
//     uint32_t cp0_status = get_cp0_status();
//     cp0_status |= 0x01;
//     set_cp0_status(cp0_status);
// }

// static char read_uart_ch(void)
// {
//     char ch = 0;
//     unsigned char *read_port = (unsigned char *)(0xbfe48000 + 0x00);
//     unsigned char *stat_port = (unsigned char *)(0xbfe48000 + 0x05);

//     while ((*stat_port & 0x01))
//     {
//         ch = *read_port;
//     }
//     return ch;
// }

// typedef struct InputBuffer {
//     char *buffer;
//     int32_t buffer_length;
//     int32_t input_length;
//     int pointer;
// } InputBuffer_t;

// #define INPUT_BUFFER_MAX_LENGTH 1000

// static char Buffer[INPUT_BUFFER_MAX_LENGTH];

// static InputBuffer_t inputBuffer;

// static void init_InputBuffer(InputBuffer_t *p)
// {
//     p->buffer = Buffer;
//     p->buffer_length = INPUT_BUFFER_MAX_LENGTH;
//     p->input_length = 0;
//     p->pointer = 0;
// }

// /*
// static void scanf(int *mem)
// {
// 	//TODO:Use read_uart_ch() to complete scanf(), read input as a hex number.
// 	//Extending function parameters to (const char *fmt, ...) as printf is recommended but not required.
//     InputBuffer_t *inputBuffer_ptr = &inputBuffer;
//     init_InputBuffer(inputBuffer_ptr);

//     disable_interrupt();
//     while (1)
//     {
//         char ch = read_uart_ch();

// 		int i = 0, k = 0;

//         if(ch != 0){
//             printf("%c", ch);        
//             *(inputBuffer_ptr->buffer + i) = ch;
//             inputBuffer_ptr->input_length++;
//             i++;
//             if(ch == '\r'){
//                 printf("\n%c", *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer));
//                 printf("%c", *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1));
//                 printf("%d", atoi(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 2));
//                 if(*(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer) == '0' 
//                 && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1) == 'x'
//                 && (k = atoi(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 2)) >= 0
//                 && k <= VM_SIZE ){
//                     *mem = k;
//                     // break;
//                     return;
//                 }
//             }            
//         }
//     }
//     enable_interrupt();
// }
// */

// static void scanf(int *mem)
// {
// 	//TODO:Use read_uart_ch() to complete scanf(), read input as a hex number.
// 	//Extending function parameters to (const char *fmt, ...) as printf is recommended but not required.
//     InputBuffer_t *inputBuffer_ptr = &inputBuffer;
//     init_InputBuffer(inputBuffer_ptr);

//     disable_interrupt();
//     while (1)
//     {
//         char ch = read_uart_ch();

// 		int i = 0, k = 0;

//         if(ch != 0){
//             printf("%c", ch);        
//             *(inputBuffer_ptr->buffer + i) = ch;
//             inputBuffer_ptr->input_length++;
//             i++;
//             if(ch == '\r'){
//                 printf("\n%c", *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer));
//                 printf("%c", *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1));
//                 printf("%d", atoi(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 2));
//                 if(*(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer) == '0' 
//                 && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1) == 'x'
//                 && (k = atoi(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 2)) >= 0
//                 && k <= VM_SIZE ){
//                     *mem = k;
//                     // break;
//                     return;
//                 }
//             }            
//         }
//     }
//     enable_interrupt();
// }

// /*
// static void scanf(const char *fmt, ...)
// {

// }
// */


void rw_task4_1(void)
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

