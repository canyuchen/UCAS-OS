/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *                  The shell acts as a task running in user mode. 
 *       The main function is to make system calls through the user's output.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
 * persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#include "test.h"
#include "stdio.h"
#include "screen.h"
#include "syscall.h"
#include "string.h"
#include "sched.h"
#include "queue.h"

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

/*@FUNCTION return : character if input is not NONE
 *                           0 if input is NONE
 */                    
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

struct task_info task1 = {"task1", (uint32_t)&ready_to_exit_task, USER_PROCESS};
struct task_info task2 = {"task2", (uint32_t)&wait_lock_task, USER_PROCESS};
struct task_info task3 = {"task3", (uint32_t)&wait_exit_task, USER_PROCESS};

struct task_info task4 = {"task4", (uint32_t)&semaphore_add_task1, USER_PROCESS};
struct task_info task5 = {"task5", (uint32_t)&semaphore_add_task2, USER_PROCESS};
struct task_info task6 = {"task6", (uint32_t)&semaphore_add_task3, USER_PROCESS};

struct task_info task7 = {"task7", (uint32_t)&producer_task, USER_PROCESS};
struct task_info task8 = {"task8", (uint32_t)&consumer_task1, USER_PROCESS};
struct task_info task9 = {"task9", (uint32_t)&consumer_task2, USER_PROCESS};

struct task_info task10 = {"task10", (uint32_t)&barrier_task1, USER_PROCESS};
struct task_info task11 = {"task11", (uint32_t)&barrier_task2, USER_PROCESS};
struct task_info task12 = {"task12", (uint32_t)&barrier_task3, USER_PROCESS};

struct task_info task13 = {"SunQuan",(uint32_t)&SunQuan, USER_PROCESS};
struct task_info task14 = {"LiuBei", (uint32_t)&LiuBei, USER_PROCESS};
struct task_info task15 = {"CaoCao", (uint32_t)&CaoCao, USER_PROCESS};

static struct task_info *test_tasks[16] = {&task1, &task2, &task3,
                                           &task4, &task5, &task6,
                                           &task7, &task8, &task9,
                                           &task10, &task11, &task12,
                                           &task13, &task14, &task15};
static int num_test_tasks = 15;

#define INPUT_BUFFER_MAX_LENGTH 1000
#define MAX_COMMAND 5
#define COMMAND_RECOGNIZED 1

typedef struct InputBuffer {
    char *buffer;
    int32_t buffer_length;
    int32_t input_length;
    int pointer;
} InputBuffer_t;

typedef enum {
    COMMAND_PARSING_RECOGNIZED = 0,
    COMMAND_PARSING_INVALID
} PARSING_t;

process_show_t ProcessShow[20];

char Buffer[INPUT_BUFFER_MAX_LENGTH];

InputBuffer_t inputBuffer;
char *Command[MAX_COMMAND] = {"ps", "clear", "spawn", "exec", "kill"};

static void init_InputBuffer(InputBuffer_t *p)
{
    //p->buffer = NULL;
    p->buffer = Buffer;
    p->buffer_length = INPUT_BUFFER_MAX_LENGTH;
    p->input_length = 0;
    p->pointer = 0;
}

static void init_ProcessShow()
{
    int i = 0;
    for(;i < 20; i++){
        ProcessShow[i].num = 0;
    }
}

/*
static void init_Command(char *Command[])
{
    Command[0] = "ps";
    Command[1] = "spawn";
    Command[2] = "exec";
    Command[3] = "kill";
}
*/

/*
static int command_cmp(char *str1, char *str2)
{
	while (*str1 && *str2 && (*str1 == *str2)){
		str1++;
		str2++;
	};
    if(*str1 == '\0' && str2 == '\n'){
        return 1;
    }
    else if(*str1 == '\0' && (str2 == ' ') || (str2 == '\t')){
        return 2;
    }
    else if(*str1 == '\0'){
        return 0;
    }
}

static PARSING_t command_parsing(InputBuffer_t *inputBuffer_ptr, int parsing_sign)
{
    //TODO
    int i, k;
    for(i = 0; i < MAX_COMMAND; i++){
        k = command_cmp(Command[i], inputBuffer_ptr->buffer + inputBuffer_ptr->pointer);
        if(k == 1){
            parsing_sign = i;
        }
        else if(k == 2){

        }
    }

    if(i == 4 && !command_cmp(Command[i], inputBuffer_ptr->buffer + inputBuffer_ptr->pointer)){
        return COMMAND_PARSING_INVALID;
    }
}
*/

#define ENUM_TYPE_CASE(x)   case x: return(#x);

static inline const char *status_type_to_string(task_status_t status)
{
    switch (status)
    {
        ENUM_TYPE_CASE(TASK_BLOCKED)
        ENUM_TYPE_CASE(TASK_RUNNING)
        ENUM_TYPE_CASE(TASK_READY)  
        ENUM_TYPE_CASE(TASK_EXITED)
        ENUM_TYPE_CASE(TASK_CREATED)
        ENUM_TYPE_CASE(TASK_SLEEPING)
    }
    return "Invalid Status";
}

void test_shell()
{
    sys_move_cursor(1, 15);
    printf("-----------------COMMAND-------------------\n");
    printf("> root@UCAS_OS: ");

    int i = 0, k = 0;
    // int parsing_sign = -1;

    InputBuffer_t *inputBuffer_ptr = &inputBuffer;
    init_InputBuffer(inputBuffer_ptr);
    init_ProcessShow();

    while (1)
    {
        // read command from UART port
        disable_interrupt();
        char ch = read_uart_ch();
        enable_interrupt();

        // TODO solve command
        if(ch != 0){
            printf("%c", ch);        
            *(inputBuffer_ptr->buffer + i) = ch;
            inputBuffer_ptr->input_length++;
            i++;
            //if(ch == '\n'){ //BUG???//!!!!!!
            if(ch == '\r'){
                if(*(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer) == 'p' 
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1) == 's'){
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
                if(*(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer) == 'c' 
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1) == 'l'
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 2) == 'e'
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 3) == 'a'
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 4) == 'r'){
                    sys_screen_clear();
                    sys_move_cursor(1, 15);
                    printf("-----------------COMMAND-------------------\n");
                    printf("> root@UCAS_OS: ");
                }
                if(*(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer) == 'e' 
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1) == 'x'
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 2) == 'e'
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 3) == 'c'
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 4) == ' '
                && (k = atoi(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 5)) >= 0
                && k <= 14 ){
                    printf("exec test_tasks[%d].\n", k);
                    sys_spawn(test_tasks[k]);
                    printf("> root@UCAS_OS: ");
                }                
                // if(*(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer) == 's' 
                // && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1) == 'p'
                // && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 2) == 'a'
                // && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 3) == 'w'
                // && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 4) == 'n'){
                    
                // }   
                if(*(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer) == 'k' 
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1) == 'i'
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 2) == 'l'
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 3) == 'l'
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 4) == ' '
                && (k = atoi(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 5)) >= 0
                && k <= 14 ){
                    printf("kill process PID = %d\n", k);
                    sys_kill(k);
                    // sys_move_cursor(0, 8);
                    // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
                    printf("> root@UCAS_OS: ");
                }      

                inputBuffer_ptr->pointer = i;
            }            
        }
    }
}
