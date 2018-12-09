/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *        Process scheduling related content, such as: scheduler, process blocking, 
 *                 process wakeup, process creation, process kill, etc.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
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

#ifndef INCLUDE_SCHEDULER_H_
#define INCLUDE_SCHEDULER_H_

#define DEBUG_P_2
#define _PROJECT_3_
//#undef _PROJECT_3_

#include "type.h"
#include "queue.h"
#include "regs.h"
#include "lock.h"

#define NUM_MAX_TASK 40

#define MAX_PID 1024
#define MAX_PRIORITY 5
#define INITIAL_PRIORITY 4

#define STACK_MIN 0xa0f00000
#define STACK_SIZE 0x80000
#define STACK_MAX 0xa2000000
#define CP0_STATUS_INIT 0x10008000
#define LOCK_MAX_NUM 10
#define MAX_LOCK_NUM_TOTAL 34

#define CLOSE_INTERRUPT \
do{ \
	uint32_t cp0_status = get_cp0_status(); \
    cp0_status &= 0xfffffffe; \
	set_cp0_status(cp0_status); \
}while(0) //IE <= 0

#define START_INTERRUPT \
do{ \
	uint32_t cp0_status = get_cp0_status(); \
    cp0_status |= 0x1; \
	set_cp0_status(cp0_status); \
}while(0) //CU <= 1, IM7 <= 1, IE <= 1

typedef int priority_t;
/* used to save register infomation */
typedef struct regs_context
{
    /* Saved main processor registers.*/
    /* 32 * 4B = 128B */
    uint32_t regs[32];

    /* Saved special registers. */
    /* 7 * 4B = 28B */
    uint32_t cp0_status;
    uint32_t hi;
    uint32_t lo;
    uint32_t cp0_badvaddr;
    uint32_t cp0_cause;
    uint32_t cp0_epc;
    uint32_t pc;

} regs_context_t; /* 128 + 28 = 156B */

typedef enum {
    TASK_BLOCKED,
    TASK_RUNNING,
    TASK_READY,
    TASK_EXITED,
    TASK_CREATED,  
    TASK_SLEEPING,  
} task_status_t;

typedef enum {
    KERNEL_PROCESS,
    KERNEL_THREAD,
    USER_PROCESS,
    USER_THREAD,
} task_type_t;

typedef enum {
    KERNEL_MODE,    
    USER_MODE,
} task_mode_t;

/* Process Control Block */
typedef struct pcb
{
    /* register context */
    regs_context_t kernel_context;
    regs_context_t user_context;
    //312
    uint32_t kernel_stack_top;
    uint32_t user_stack_top;
    //320
    /* process id */
    pid_t pid;
    //324
    task_mode_t mode;

    uint32_t entry_point;
    /* previous, next pointer */
    void *prev;
    void *next;

    /* kernel/user thread/process */
    task_type_t type;

    /* BLOCK | READY | RUNNING */
    task_status_t status;

    /* cursor position */
    int cursor_x;
    int cursor_y;

    uint32_t wait_time;
    priority_t priority;
    
    mutex_lock_t *lock[LOCK_MAX_NUM];
    uint32_t lock_num;

    queue_t waiting_queue;

    uint32_t sleeping_deadline;

    uint32_t page_table_base_addr;

} pcb_t;

/* task information, used to init PCB */
typedef struct task_info
{
#ifdef _PROJECT_3_
    char *task_name;
#endif
    uint32_t entry_point;
    task_type_t type;
} task_info_t;

typedef struct process_show {
    int num;
    int pid;
    // char *status;
    task_status_t status;
} process_show_t;

extern process_show_t ProcessShow[40];

/* ready queue to run */
extern queue_t ready_queue;

//extern queue_p ready_queue_ptr;

/* block queue to wait */
extern queue_t block_queue;

/* sleeping queue to be waken up */
extern queue_t sleeping_queue;

/* current running task PCB */
extern pcb_t *current_running;
extern pid_t process_id;

extern pcb_t pcb[NUM_MAX_TASK];
extern uint32_t initial_cp0_status;
extern uint32_t PID;
extern uint32_t STACK_TOP;

extern void first_entry();

extern void do_scheduler(void);
extern void do_sleep(uint32_t);

extern void do_block(queue_t *);
extern void do_unblock_one(queue_t *);
extern void do_unblock_all(queue_t *);

extern void do_ps();
extern void do_spawn(task_info_t *task_info);
extern void do_exit();
extern int  do_getpid();
extern void do_waitpid(int n);
extern void do_kill(int n); 

extern priority_t my_priority[MAX_PID];
extern priority_t now_priority[MAX_PID];

extern mutex_lock_t *Lock[MAX_LOCK_NUM_TOTAL];

extern mutex_lock_t lock1;
extern mutex_lock_t lock2;
extern mutex_lock_t *Lock[MAX_LOCK_NUM_TOTAL];

extern uint32_t get_pcb_index(int pid);

#endif