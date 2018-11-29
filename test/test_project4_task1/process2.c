#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "time.h"
#include "screen.h"
#include "test4.h"
#include "stdarg.h"
#include "string.h"

static int rand()
{	
	int current_time = get_timer();
	return current_time % 100000;
}

static void disable_interrupt()
{
    uint32_t cp0_status = get_cp0_status();
    cp0_status &= 0xfffffffe;
    set_cp0_status(cp0_status);
}

static void enable_interrupt()
{
    uint32_t cp0_status = get_cp0_status();
    cp0_status |= 0x01;
    set_cp0_status(cp0_status);
}

static char read_uart_ch(void)
{
    char ch = 0;
    unsigned char *read_port = (unsigned char *)(0xbfe48000 + 0x00);
    unsigned char *stat_port = (unsigned char *)(0xbfe48000 + 0x05);

    while ((*stat_port & 0x01))
    {
        ch = *read_port;
    }
    return ch;
}

typedef struct InputBuffer {
    char *buffer;
    int32_t buffer_length;
    int32_t input_length;
    int pointer;
} InputBuffer_t;

#define INPUT_BUFFER_MAX_LENGTH 1000

static char Buffer[INPUT_BUFFER_MAX_LENGTH];

static InputBuffer_t inputBuffer;

static void init_InputBuffer(InputBuffer_t *p)
{
    p->buffer = Buffer;
    p->buffer_length = INPUT_BUFFER_MAX_LENGTH;
    p->input_length = 0;
    p->pointer = 0;
}

static void scanf(int *mem)
{
	//TODO:Use read_uart_ch() to complete scanf(), read input as a hex number.
	//Extending function parameters to (const char *fmt, ...) as printf is recommended but not required.
    InputBuffer_t *inputBuffer_ptr = &inputBuffer;
    init_InputBuffer(inputBuffer_ptr);

    while (1)
    {
        disable_interrupt();
        char ch = read_uart_ch();
        enable_interrupt();

		int i = 0, k = 0;

        if(ch != 0){
            printf("%c", ch);        
            *(inputBuffer_ptr->buffer + i) = ch;
            inputBuffer_ptr->input_length++;
            i++;
            if(ch == '\r'){
                if(*(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer) == '0' 
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1) == 'x'){
                    sys_ps();
                    printf("[PROCESS TABLE]\n");
                    int j = 0;
                    while(ProcessShow[j].num >= 0){
                        printf("[%d] PID : %d STATUS : %s\n", ProcessShow[j].num, \
                               ProcessShow[j].pid, status_type_to_string(ProcessShow[j].status));
                        j++;
                    }
                    printf("> root@UCAS_OS: ");
                }

                inputBuffer_ptr->pointer = i;
            }            
        }
    }
}

/*
static void scanf(const char *fmt, ...)
{

}
*/

/*
void rw_task1(void)
{
	int mem1, mem2 = 0;
	int curs = 0;
	int memory[RW_TIMES];
	int i = 0;
	for(i = 0; i < RW_TIMES; i++)
	{
		vt100_move_cursor(1, curs+i);
		
		scanf(&mem1);
		// scanf("%x", &mem1);
		vt100_move_cursor(1, curs+i);
		memory[i] = mem2 = rand();
		*(int *)mem1 = mem2;
		printk("Write: 0x%x, %d", mem1, mem2);
	}
	curs = RW_TIMES;
	for(i = 0; i < RW_TIMES; i++)
	{
		vt100_move_cursor(1, curs+i);
		scanf(&mem1);
		// scanf("%x", &mem1);
		vt100_move_cursor(1, curs+i);
		memory[i+RW_TIMES] = *(int *)mem1;
		if(memory[i+RW_TIMES] == memory[i])
			printk("Read succeed: %d", memory[i+RW_TIMES]);
		else
			printk("Read error: %d", memory[i+RW_TIMES]);
	}
	while(1);
	//Only input address.
	//Achieving input r/w command is recommended but not required.
}
*/

void rw_task1(void)
{
	int mem1, mem2 = 0;
	int curs = 0;
	int memory[RW_TIMES];
	int i = 0;
	for(i = 0; i < RW_TIMES; i++)
	{
		vt100_move_cursor(1, curs+i);
		printf("the m")
		scanf(&mem1);
		// scanf("%x", &mem1);
		vt100_move_cursor(1, curs+i);
		memory[i] = mem2 = rand();
		*(int *)mem1 = mem2;
		printk("Write: 0x%x, %d", mem1, mem2);
	}
	curs = RW_TIMES;
	for(i = 0; i < RW_TIMES; i++)
	{
		vt100_move_cursor(1, curs+i);
		scanf(&mem1);
		// scanf("%x", &mem1);
		vt100_move_cursor(1, curs+i);
		memory[i+RW_TIMES] = *(int *)mem1;
		if(memory[i+RW_TIMES] == memory[i])
			printk("Read succeed: %d", memory[i+RW_TIMES]);
		else
			printk("Read error: %d", memory[i+RW_TIMES]);
	}
	while(1);
	//Only input address.
	//Achieving input r/w command is recommended but not required.
}

