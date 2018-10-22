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


static void init_pcb()
{
	// ready_queue = ready_queue_init;
	// block_queue = block_queue_init;

	queue_init(&ready_queue);
	queue_init(&block_queue);

	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	uint32_t STACK_TOP = STACK_MIN;
	for(i = 0; i < num_sched1_tasks; i++)
	{
		for(j = 0; j < 32; j++)
		{
			pcb[i].kernel_context.regs[j] = 0;	
			pcb[i].user_context.regs[j] = 0;		
		}
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
		pcb[i].status = TASK_READY;
		pcb[i].cursor_x = 0;
		pcb[i].cursor_y = 0;

		pcb[i].entry_point = sched1_tasks[i]->entry_point;

		pcb[i].kernel_context.regs[31] = sched1_tasks[i]->entry_point;
		pcb[i].user_context.regs[31] = sched2_tasks[i]->entry_point;
/*
		asm(
			"addi $31, %0, 0x0 \t\n"
			"lw $31, 320($31)  \t\n"
			"sw $31, 124($31)  \t\n"
			:
			:"r"(&pcb[i])
		);
*/
		queue_push(&ready_queue,&pcb[i]);
	}

	for(k = 0; k < num_lock_tasks; i++, k++)
	{
		for(j = 0; j < 32; j++)
		{
			pcb[i].kernel_context.regs[j] = 0;	
			pcb[i].user_context.regs[j] = 0;		
		}
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
		pcb[i].status = TASK_READY;
		pcb[i].cursor_x = 0;
		pcb[i].cursor_y = 0;

		pcb[i].entry_point = lock_tasks[k]->entry_point;

		pcb[i].kernel_context.regs[31] = lock_tasks[k]->entry_point;

		queue_push(&ready_queue,&pcb[i]);
	}

	current_running->entry_point = 0;
}

static void init_exception_handler()
{
}

static void init_exception()
{
	// 1. Get CP0_STATUS
	// 2. Disable all interrupt
	// 3. Copy the level 2 exception handling code to 0x80000180
	// 4. reset CP0_COMPARE & CP0_COUNT register
}

static void init_syscall(void)
{
	// init system call table.
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

	while (1)
	{
		// (QAQQQQQQQQQQQ)
		// If you do non-preemptive scheduling, you need to use it to surrender control
		// do_scheduler();
		do_scheduler();
	};
	return;
}
