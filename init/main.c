/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *         The kernel's entry, where most of the initialization work is done.
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

#include "irq.h"
#include "test.h"
#include "stdio.h"
#include "sched.h"
#include "screen.h"
#include "common.h"
#include "syscall.h"
#include "regs.h"
#include "string.h"
#include "lock.h"
#include "barrier.h"
#include "mailbox.h"

int is_init = 0;

queue_t ready_queue;
queue_t block_queue;
queue_t sleeping_queue;

pcb_t pcb_init;

pcb_t pcb[NUM_MAX_TASK];

/* current running task PCB */
pcb_t *current_running = &pcb_init;

/* All stacks should be STACK_SIZE bytes large
 * The first stack should be placed at location STACK_MIN
 * Only memory below STACK_MAX should be used for stacks
 */
// uint32_t STACK_MIN = 0xa0f00000;
// uint32_t STACK_SIZE =   0x80000;
// uint32_t STACK_MAX = 0xa2000000;
uint32_t STACK_TOP = STACK_MIN;

uint32_t PID = 2;

uint32_t time_elapsed = 0;

extern mailbox_t mboxs[MAX_NUM_BOX];

// defined in entry.S
extern void exception_handler_start();
extern void exception_handler_end();
extern void handle_int();
extern void handle_syscall();
extern void handle_others();
extern uint32_t get_cp0_status();
extern void set_cp0_status(uint32_t);

extern void first_entry();
extern void reset_timer();

uint32_t exception_handlers[32];
priority_t my_priority[MAX_PID];
priority_t now_priority[MAX_PID];

#define EBASE 0xbfc00000
#define EBASE_OFFSET 0x380

mutex_lock_t mutex_lock;
mutex_lock_t mutex_lock_1;
mutex_lock_t mutex_lock_2;

// extern barrier_t barrier;
// extern int barrier_is_init;
// extern int barrier_num_task;

// barrier_t barrier;
// int barrier_is_init = 0;
// int barrier_num_task = 3;

/*
static void init_pcb()
{
	// ready_queue = ready_queue_init;
	// block_queue = block_queue_init;
	uint32_t cp0_status_init = 0x10008000;

	queue_init(&ready_queue);
	queue_init(&block_queue);
	queue_init(&sleeping_queue);

	// queue_init(&(mutex_lock_1.mutex_lock_queue));
	// queue_init(&(mutex_lock_2.mutex_lock_queue));

	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	uint32_t l = 0;
	uint32_t STACK_TOP = STACK_MIN;
	for(i = 0; i < num_sched2_tasks; i++)
	{
		bzero(&(pcb[i].kernel_context), sizeof(pcb[i].kernel_context));
		bzero(&(pcb[i].user_context  ), sizeof(pcb[i].user_context  ));
		pcb[i].kernel_context.regs[29] = STACK_TOP;
		pcb[i].user_context.regs[29] = STACK_TOP + STACK_SIZE;
		pcb[i].kernel_context.regs[30] = STACK_TOP;
		pcb[i].user_context.regs[30] = STACK_TOP + STACK_SIZE;
		pcb[i].kernel_stack_top = STACK_TOP;
		pcb[i].user_stack_top = STACK_TOP + STACK_SIZE;		
		STACK_TOP += STACK_SIZE*2;
		if(STACK_TOP > STACK_MAX)
		{
			//TODO
		}

		pcb[i].prev = NULL;
		pcb[i].next = NULL;
		pcb[i].pid = i;
		pcb[i].type = sched2_tasks[i]->type;
		pcb[i].status = TASK_CREATED;
		pcb[i].cursor_x = 0;
		pcb[i].cursor_y = 0;

		pcb[i].entry_point = sched2_tasks[i]->entry_point;

		pcb[i].kernel_context.regs[31] = (uint32_t)first_entry;

		pcb[i].kernel_context.cp0_status = cp0_status_init;
		pcb[i].user_context.cp0_status = cp0_status_init;

		//pcb[i].kernel_context.cp0_epc = timer_tasks[l]->entry_point;
		//???
		pcb[i].user_context.cp0_epc = sched2_tasks[i]->entry_point;
		//cp0_epc add 4 automatically when encountering interrupt

		pcb[i].mode = (sched2_tasks[i]->type == KERNEL_PROCESS 
					|| sched2_tasks[i]->type == KERNEL_THREAD) ? KERNEL_MODE : USER_MODE;
		my_priority[i] = INITIAL_PRIORITY;
    	now_priority[i] = INITIAL_PRIORITY;
		pcb[i].priority = INITIAL_PRIORITY;
		pcb[i].wait_time = 0;
		pcb[i].sleeping_deadline = 0;

		queue_push(&ready_queue,&pcb[i]);
	}

	for(k = 0; k < num_lock_tasks; i++, k++)
	{
		bzero(&(pcb[i].kernel_context), sizeof(pcb[i].kernel_context));
		bzero(&(pcb[i].user_context  ), sizeof(pcb[i].user_context  ));
		pcb[i].kernel_context.regs[29] = STACK_TOP;
		pcb[i].user_context.regs[29] = STACK_TOP + STACK_SIZE;
		pcb[i].kernel_context.regs[30] = STACK_TOP;
		pcb[i].user_context.regs[30] = STACK_TOP + STACK_SIZE;
		pcb[i].kernel_stack_top = STACK_TOP;
		pcb[i].user_stack_top = STACK_TOP + STACK_SIZE;		
		STACK_TOP += STACK_SIZE*2;
		if(STACK_TOP > STACK_MAX)
		{
			//TODO
		}

		pcb[i].prev = NULL;
		pcb[i].next = NULL;
		pcb[i].pid = i;
		pcb[i].type = lock_tasks[k]->type;
		pcb[i].status = TASK_CREATED;
		pcb[i].cursor_x = 0;
		pcb[i].cursor_y = 0;

		pcb[i].entry_point = lock_tasks[k]->entry_point;

		pcb[i].kernel_context.regs[31] = (uint32_t)first_entry;

		pcb[i].kernel_context.cp0_status = cp0_status_init;
		pcb[i].user_context.cp0_status = cp0_status_init;

		//pcb[i].kernel_context.cp0_epc = timer_tasks[l]->entry_point;
		//???
		pcb[i].user_context.cp0_epc = lock_tasks[k]->entry_point;

		pcb[i].mode = (lock_tasks[k]->type == KERNEL_PROCESS 
					|| lock_tasks[k]->type == KERNEL_THREAD) ? KERNEL_MODE : USER_MODE;
		my_priority[i] = INITIAL_PRIORITY;
    	now_priority[i] = INITIAL_PRIORITY;
		pcb[i].priority = INITIAL_PRIORITY;
		pcb[i].wait_time = 0;
		pcb[i].sleeping_deadline = 0;

		queue_push(&ready_queue,&pcb[i]);
	}

	for(l = 0; l < num_timer_tasks; i++, l++)
	{
		bzero(&(pcb[i].kernel_context), sizeof(pcb[i].kernel_context));
		bzero(&(pcb[i].user_context  ), sizeof(pcb[i].user_context  ));
		pcb[i].kernel_context.regs[29] = STACK_TOP;
		pcb[i].user_context.regs[29] = STACK_TOP + STACK_SIZE;
		pcb[i].kernel_context.regs[30] = STACK_TOP;
		pcb[i].user_context.regs[30] = STACK_TOP + STACK_SIZE;
		pcb[i].kernel_stack_top = STACK_TOP;
		pcb[i].user_stack_top = STACK_TOP + STACK_SIZE;		
		STACK_TOP += STACK_SIZE*2;
		if(STACK_TOP > STACK_MAX)
		{
			//TODO
		}

		pcb[i].prev = NULL;
		pcb[i].next = NULL;
		pcb[i].pid = i;
		pcb[i].type = timer_tasks[l]->type;
		pcb[i].status = TASK_CREATED;
		pcb[i].cursor_x = 0;
		pcb[i].cursor_y = 0;

		pcb[i].entry_point = timer_tasks[l]->entry_point;

		pcb[i].kernel_context.regs[31] = (uint32_t)first_entry;

		pcb[i].kernel_context.cp0_status = cp0_status_init;
		pcb[i].user_context.cp0_status = cp0_status_init;

		//pcb[i].kernel_context.cp0_epc = timer_tasks[l]->entry_point;
		//???
		pcb[i].user_context.cp0_epc = timer_tasks[l]->entry_point;

		pcb[i].mode = (timer_tasks[l]->type == KERNEL_PROCESS 
					|| timer_tasks[l]->type == KERNEL_THREAD) ? KERNEL_MODE : USER_MODE;
		my_priority[i] = INITIAL_PRIORITY;
    	now_priority[i] = INITIAL_PRIORITY;
		pcb[i].priority = INITIAL_PRIORITY;
		pcb[i].wait_time = 0;
		pcb[i].sleeping_deadline = 0;

		queue_push(&ready_queue,&pcb[i]);
	}

	current_running->entry_point = 0;
}
*/

static void init_pcb()
{
	// ready_queue = ready_queue_init;
	// block_queue = block_queue_init;
	// uint32_t cp0_status_init = 0x10008000;

    // barrier_init(&barrier, barrier_num_task);

	int i = 0;
	for(;i < NUM_MAX_TASK; i++){
		pcb[i].status = TASK_EXITED;
	}

	Lock[0] = &lock1;
	Lock[1] = &lock2;

	for(i = 2; i < MAX_LOCK_NUM_TOTAL; i++){
		Lock[i] = &(mboxs[i-2].lock);
	}

	for(i = 0; i < MAX_LOCK_NUM_TOTAL; i++){
		Lock[i]->status = UNLOCKED;
		queue_init(&(Lock[i]->mutex_lock_queue));
	}

	queue_init(&ready_queue);
	queue_init(&block_queue);
	queue_init(&sleeping_queue);

	queue_init(&(pcb[1].waiting_queue));

	// uint32_t i = 0;
	// uint32_t j = 0;
	// uint32_t k = 0;
	// uint32_t l = 0;
	// uint32_t STACK_TOP = STACK_MIN;

	bzero(&(pcb[1].kernel_context), sizeof(pcb[1].kernel_context));
	bzero(&(pcb[1].user_context  ), sizeof(pcb[1].user_context  ));
	// bzero(pcb[1].lock, LOCK_MAX_NUM);
	//???
	int j = 0;
	for(; j < LOCK_MAX_NUM; j++){
		pcb[i].lock[j] = NULL;
	}
	pcb[1].kernel_context.regs[29] = STACK_TOP;
	pcb[1].user_context.regs[29] = STACK_TOP + STACK_SIZE;
	pcb[1].kernel_context.regs[30] = STACK_TOP;
	pcb[1].user_context.regs[30] = STACK_TOP + STACK_SIZE;
	pcb[1].kernel_stack_top = STACK_TOP;
	pcb[1].user_stack_top = STACK_TOP + STACK_SIZE;		
	STACK_TOP += STACK_SIZE*2;
	if(STACK_TOP > STACK_MAX)
	{
		//TODO
	}

	pcb[1].prev = NULL;
	pcb[1].next = NULL;
	pcb[1].pid = 1;
	pcb[1].type = USER_PROCESS;
	pcb[1].status = TASK_CREATED;
	pcb[1].cursor_x = 0;
	pcb[1].cursor_y = 0;

	pcb[1].entry_point = (uint32_t)&test_shell;

	pcb[1].kernel_context.regs[31] = (uint32_t)first_entry;

	pcb[1].kernel_context.cp0_status = CP0_STATUS_INIT;
	pcb[1].user_context.cp0_status = CP0_STATUS_INIT;

	pcb[1].kernel_context.cp0_epc = pcb[1].entry_point;
	//???
	pcb[1].user_context.cp0_epc = pcb[1].entry_point;
	//cp0_epc add 4 automatically when encountering interrupt

	pcb[1].mode = USER_MODE;
	// my_priority[i] = INITIAL_PRIORITY;
	// now_priority[i] = INITIAL_PRIORITY;
	pcb[1].priority = INITIAL_PRIORITY;
	pcb[1].wait_time = 0;
	pcb[1].sleeping_deadline = 0;
	pcb[1].lock_num = 0;

	queue_push(&ready_queue,&pcb[1]);

	current_running->status = TASK_CREATED;

	current_running->entry_point = 0;
	current_running->pid = 0;
	//pcb[0] = pcb_init; //ERROR!!!
}

static void init_exception_handler()
{
	int i = 0;
	for (i = 0; i < 32; ++i) {
		exception_handlers[i] = (uint32_t)handle_others;
	}
	exception_handlers[INT] = (uint32_t)handle_int;
	exception_handlers[SYS] = (uint32_t)handle_syscall;
}

static void init_exception()
{
	// 1. Get CP0_STATUS
	// 2. Disable all interrupt 
	// 3. Copy the level 2 exception handling code to 0x80000180
	// 4. reset CP0_COMPARE & CP0_COUNT register

	CLOSE_INTERRUPT;

	init_exception_handler();

	// Copy the level 2 exception handling code to 0x80000180
	// When BEV=1, EBASE is BFC00380
	// fill nop
	//bzero(EBASE,EBASE_OFFSET);
	// copy the exception handler to EBase
	memcpy((uint8_t *)(EBASE+EBASE_OFFSET),(uint8_t *)exception_handler_begin,\
		   exception_handler_end-exception_handler_begin);

	// When BEV=0, EBASE change to 0x80000000
	// offset change to 0x180
	//bzero(0x80000000,0x180);
	memcpy((uint8_t *)0x80000180, (uint8_t *)exception_handler_begin,\
		   exception_handler_end-exception_handler_begin);

	reset_timer();
}

static void init_syscall(void)
{
	// init system call table.
	int fn;

	for (fn = 0; fn < NUM_SYSCALLS; ++fn) {
		syscall[fn] = (int (*)()) &invalid_syscall;
	}
	syscall[SYSCALL_SLEEP] = (int (*)()) &do_sleep;
	syscall[SYSCALL_BLOCK] = (int (*)()) &do_block;
	syscall[SYSCALL_UNBLOCK_ONE] = (int (*)()) &do_unblock_one;
	syscall[SYSCALL_UNBLOCK_ALL] = (int (*)()) &do_unblock_all;
	syscall[SYSCALL_WRITE] = (int (*)()) &do_write;
	syscall[SYSCALL_CURSOR] = (int (*)()) &screen_move_cursor;
	syscall[SYSCALL_REFLUSH] = (int (*)()) &screen_reflush;
	syscall[SYSCALL_MUTEX_LOCK_INIT] = (int (*)()) &do_mutex_lock_init;
	syscall[SYSCALL_MUTEX_LOCK_ACQUIRE] = (int (*)()) &do_mutex_lock_acquire;
	syscall[SYSCALL_MUTEX_LOCK_RELEASE] = (int (*)()) &do_mutex_lock_release;
	syscall[SYSCALL_SCREEN_CLEAR] = (int (*)()) &screen_clear;
	syscall[SYSCALL_SEMAPHORE_INIT] = (int (*)()) &do_semaphore_init;
	syscall[SYSCALL_SEMAPHORE_UP] = (int (*)()) &do_semaphore_up;
	syscall[SYSCALL_SEMAPHORE_DOWN] = (int (*)()) &do_semaphore_down;
	syscall[SYSCALL_CONDITION_INIT] = (int (*)()) &do_condition_init;
	syscall[SYSCALL_CONDITION_WAIT] = (int (*)()) &do_condition_wait;
	syscall[SYSCALL_CONDITION_SIGNAL] = (int (*)()) &do_condition_signal;
	syscall[SYSCALL_CONDITION_BROADCAST] = (int (*)()) &do_condition_broadcast;
	syscall[SYSCALL_BARRIER_INIT] = (int (*)()) &do_barrier_init;
	syscall[SYSCALL_BARRIER_WAIT] = (int (*)()) &do_barrier_wait;
	syscall[SYSCALL_EXIT] = (int (*)()) &do_exit;
	syscall[SYSCALL_WAITPID] = (int (*)()) &do_waitpid;
	syscall[SYSCALL_GETPID] = (int (*)()) &do_getpid;
	syscall[SYSCALL_SPAWN] = (int (*)()) &do_spawn;
	syscall[SYSCALL_KILL] = (int (*)()) &do_kill;
	syscall[SYSCALL_PS] = (int (*)()) &do_ps;
}

// jump from bootloader.
// The beginning of everything >_< ~~~~~~~~~~~~~~
void __attribute__((section(".entry_function"))) _start(void)
{
	// Close the cache, no longer refresh the cache 
	// when making the exception vector entry copy
	asm_start();

	//__asm__ __volatile__("addiu $29, $29, -8\n\tsw $31, 28($29)");
	// init interrupt (^_^)
	init_exception();
	//printk("> [INIT] Interrupt processing initialization succeeded.\n");

	// init system call table (0_0)
	init_syscall();
	//printk("> [INIT] System call initialized successfully.\n");

	// init Process Control Block (-_-!)
	init_pcb();
	//printk("> [INIT] PCB initialization succeeded.\n");

	// init screen (QAQ)
	init_screen();
	//printk("> [INIT] SCREEN initialization succeeded.\n");

	// TODO Enable interrupt

	// START_INTERRUPT;

	//do_scheduler();

	while (1)
	{
		// (QAQQQQQQQQQQQ)
		// If you do non-preemptive scheduling, you need to use it to surrender control
		do_scheduler();
		//scheduler();
	};
	return;
}
