#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"

pcb_t pcb[NUM_MAX_TASK];

/* ready queue to run */
//queue_t ready_queue;
// extern queue_p ready_queue_ptr;
// extern queue_p block_queue_ptr;

/* global process id */
pid_t process_id = 1;

static void check_sleeping()
{
}

void scheduler(void)
{
    // TODO schedule
    // Modify the current_running pointer.

    // if(!current_running)
    // {
    //     current_running->status = TASK_READY;
    //     //queue_push(ready_queue_ptr, current_running);
    //     queue_push(&ready_queue, current_running);        
    // }
    
    if(current_running->status != TASK_BLOCKED){
        current_running->status = TASK_READY;
        //queue_push(ready_queue_ptr, current_running);
        if(current_running->entry_point != 0){
            //initial pcb does not need to be pushed to ready_queue
            queue_push(&ready_queue, current_running);           
        }        
    }

    current_running = queue_dequeue(&ready_queue);
    current_running->status = TASK_RUNNING;

}

void do_sleep(uint32_t sleep_time)
{
    // TODO sleep(seconds)
}

void do_block(queue_t *queue)
{
    // block the current_running task into the queue
    //SAVE_CONTEXT(0);
/*
    asm(
        "addi $26, %0, 0x0 \t\n"
        "sw $0 , ($26)     \t\n"
        "sw $1 , 4($26)    \t\n"   
        "sw $2 , 8($26)    \t\n"   
        "sw $3 , 12($26)   \t\n"  
        "sw $4 , 16($26)   \t\n"  
        "sw $5 , 20($26)   \t\n"  
        "sw $6 , 24($26)   \t\n"  
        "sw $7 , 28($26)   \t\n"  
        "sw $8 , 32($26)   \t\n"  
        "sw $9 , 36($26)   \t\n"  
        "sw $10, 40($26)   \t\n"  
        "sw $11, 44($26)   \t\n"  
        "sw $12, 48($26)   \t\n"  
        "sw $13, 52($26)   \t\n"  
        "sw $14, 56($26)   \t\n"  
        "sw $15, 60($26)   \t\n"  
        "sw $16, 64($26)   \t\n"  
        "sw $17, 68($26)   \t\n"   
        "sw $18, 72($26)   \t\n"  
        "sw $19, 76($26)   \t\n"  
        "sw $20, 80($26)   \t\n"  
        "sw $21, 84($26)   \t\n"  
        "sw $22, 88($26)   \t\n"  
        "sw $23, 92($26)   \t\n"  
        "sw $24, 96($26)   \t\n"  
        "sw $25, 100($26)  \t\n"   
        "sw $27, 108($26)  \t\n"   
        "sw $28, 112($26)  \t\n"   
        "sw $29, 116($26)  \t\n"   
        "sw $30, 120($26)  \t\n"   
        "sw $31, 124($26)  \t\n"   
        :
        :"r"(current_running)
    );
*/
    //save_pcb();
    current_running->status = TASK_BLOCKED;
    queue_push(&block_queue, current_running);
    do_scheduler();
}

void do_unblock_one(queue_t *queue)
{
    // unblock the head task from the queue
    // save_pcb();
    // current_running->status = TASK_READY;
    // queue_push(&ready_queue, current_running);
    
    pcb_t *block_queue_head_ptr;

    if(!queue_is_empty(&block_queue)){
        block_queue_head_ptr = queue_dequeue(&block_queue);
        block_queue_head_ptr->status = TASK_READY;
        queue_push(&ready_queue, block_queue_head_ptr);
    }
}

void do_unblock_all(queue_t *queue)
{
    // unblock all task in the queue
    while(!queue_is_empty(&block_queue)){
        do_unblock_one(&block_queue);
    }
}
