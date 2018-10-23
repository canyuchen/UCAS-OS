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

// queue_t ready_queue_init;
// queue_t block_queue_init;

// queue_p ready_queue_ptr;
// queue_p block_queue_ptr;

queue_t ready_queue;
queue_t block_queue;

pcb_t pcb_init;

pcb_t pcb[NUM_MAX_TASK];

/* current running task PCB */
pcb_t *current_running = &pcb_init;

/* All stacks should be STACK_SIZE bytes large
 * The first stack should be placed at location STACK_MIN
 * Only memory below STACK_MAX should be used for stacks
 */
uint32_t STACK_MIN = 0xa0f00000;
uint32_t STACK_SIZE =   0x80000;
uint32_t STACK_MAX = 0xa2000000;

// defined in entry.S
extern void exception_handler_start();
extern void exception_handler_end();
extern void handle_int();
extern void handle_syscall();
extern void simple_handler();
extern uint32_t get_cp0_status();
extern void set_cp0_status(uint32_t);

extern void first_entry();

uint32_t exception_handlers[32];
priority_t my_priority[MAX_PID];
priority_t now_priority[MAX_PID];

#define EBASE 0xbfc00000
#define EBASE_OFFSET 0x380

static void init_pcb()
{
	// ready_queue = ready_queue_init;
	// block_queue = block_queue_init;
	uint32_t cp0_status_init = 0x10008000;

	queue_init(&ready_queue);
	queue_init(&block_queue);

	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	uint32_t STACK_TOP = STACK_MIN;
	for(i = 0; i < num_sched1_tasks; i++)
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
		pcb[i].type = sched1_tasks[i]->type;
		pcb[i].status = TASK_CREATED;
		pcb[i].cursor_x = 0;
		pcb[i].cursor_y = 0;

		pcb[i].entry_point = sched1_tasks[i]->entry_point;

		//pcb[i].kernel_context.regs[31] = sched1_tasks[i]->entry_point;
		pcb[i].kernel_context.regs[31] = (uint32_t)first_entry;
		//pcb[i].user_context.regs[31] = sched2_tasks[i]->entry_point;

		pcb[i].kernel_context.cp0_status = cp0_status_init;
		pcb[i].user_context.cp0_status = cp0_status_init;

		pcb[i].user_context.cp0_epc = sched1_tasks[k]->entry_point;

		pcb[i].mode = (sched1_tasks[i]->type == KERNEL_PROCESS 
					|| sched1_tasks[i]->type == KERNEL_THREAD) ? KERNEL_MODE : USER_MODE;
		my_priority[i] = INITIAL_PRIORITY;
    	now_priority[i] = INITIAL_PRIORITY;
		pcb[i].priority = INITIAL_PRIORITY;
		pcb[i].wait_time = 0;

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

		//pcb[i].kernel_context.regs[31] = sched1_tasks[i]->entry_point;
		pcb[i].kernel_context.regs[31] = (uint32_t)first_entry;
		//pcb[i].user_context.regs[31] = lock_tasks[k]->entry_point;

		pcb[i].kernel_context.cp0_status = cp0_status_init;
		pcb[i].user_context.cp0_status = cp0_status_init;

		//pcb[i].kernel_context.cp0_epc = lock_tasks[k]->entry_point;
		pcb[i].user_context.cp0_epc = lock_tasks[k]->entry_point;

		pcb[i].mode = (lock_tasks[k]->type == KERNEL_PROCESS 
					|| lock_tasks[k]->type == KERNEL_THREAD) ? KERNEL_MODE : USER_MODE;
		my_priority[i] = INITIAL_PRIORITY;
    	now_priority[i] = INITIAL_PRIORITY;
		pcb[i].priority = INITIAL_PRIORITY;
		pcb[i].wait_time = 0;

		queue_push(&ready_queue,&pcb[i]);
	}

	current_running->entry_point = 0;
}

static void init_exception_handler()
{
	int i = 0;
	for (i = 0; i < 32; ++i) {
		exception_handlers[i] = (uint32_t)simple_handler;
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
	init_exception_handler();

	// Copy the level 2 exception handling code to 0x80000180
	// When BEV=1, EBASE is BFC00380
	// fill nop
	bzero(EBASE,EBASE_OFFSET);
	// copy the exception handler to EBase
	memcpy(EBASE+EBASE_OFFSET,exception_handler_begin,\
		   exception_handler_end-exception_handler_begin);

	// When BEV=0, EBASE change to 0x80000000
	// offset change to 0x180
	bzero(0x80000000,0x180);
	memcpy(0x80000180,exception_handler_begin,\
		   exception_handler_end-exception_handler_begin);

	// reset_timer(TIMER_INTERVAL); // 300MHz

	// Get CP0_STATUS 
	uint32_t cp0_status = get_cp0_status();
	cp0_status |= 0x8001;
	set_cp0_status(STATUS_CU0 | cp0_status); //CU <= 1, IM7 <= 1, IE <= 1

}

static void init_syscall(void)
{
	// init system call table.
	int fn;

	for (fn = 0; fn < NUM_SYSCALLS; ++fn) {
		syscall[fn] = &invalid_syscall;
	}
	syscall[SYSCALL_SLEEP] = (int (*)()) &sys_sleep;
	syscall[SYSCALL_BLOCK] = (int (*)()) &sys_block;
	syscall[SYSCALL_UNBLOCK_ONE] = (int (*)()) &sys_unblock_one;
	syscall[SYSCALL_UNBLOCK_ALL] = (int (*)()) &sys_unblock_all;
	syscall[SYSCALL_WRITE] = (int (*)()) &sys_write;
	syscall[SYSCALL_READ] = (int (*)()) &sys_read;
	syscall[SYSCALL_CURSOR] = (int (*)()) &sys_move_cursor;
	syscall[SYSCALL_REFLUSH] = (int (*)()) &sys_reflush;
	syscall[SYSCALL_MUTEX_LOCK_INIT] = (int (*)()) &mutex_lock_init;
	syscall[SYSCALL_MUTEX_LOCK_ACQUIRE] = (int (*)()) &mutex_lock_acquire;
	syscall[SYSCALL_MUTEX_LOCK_RELEASE] = (int (*)()) &mutex_lock_release;
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
	printk("> [INIT] Interrupt processing initialization succeeded.\n");

	// init system call table (0_0)
	init_syscall();
	printk("> [INIT] System call initialized successfully.\n");

	// init Process Control Block (-_-!)
	init_pcb();
	printk("> [INIT] PCB initialization succeeded.\n");

	// init screen (QAQ)
	init_screen();
	printk("> [INIT] SCREEN initialization succeeded.\n");

	// TODO Enable interrupt

/*	
    current_running = queue_dequeue(&ready_queue);
    current_running->status = TASK_RUNNING;
	// __asm__ __volatile__(
	// 	//"lw t0, %0       \n\t"
	// 	"lw $8, %0       \n\t"
	// 	"addiu $8,$8,320 \n\t"
	// 	"jr $8"
	// 	:
	// 	:"r"(current_running)
	// );

	//printk_task1();
*/
	init_exception();
	init_syscall();

	while (1)
	{
		// (QAQQQQQQQQQQQ)
		// If you do non-preemptive scheduling, you need to use it to surrender control
		// do_scheduler();
		do_scheduler();
	};
	return;
}
