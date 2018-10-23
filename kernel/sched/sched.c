#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"

pcb_t pcb[NUM_MAX_TASK];

/* global process id */
pid_t process_id = 1;

priority_t my_priority[MAX_PID];
priority_t now_priority[MAX_PID];

static void check_sleeping()
{
}

/*
void scheduler(void)
{
    // TODO schedule
    // Modify the current_running pointer.
    if(current_running->status != TASK_BLOCKED){
        current_running->status = TASK_READY;
        if(current_running->entry_point != 0){
            //initial pcb does not need to be pushed to ready_queue
            queue_push(&ready_queue, current_running);           
        }        
    }

    current_running = queue_dequeue(&ready_queue);
    current_running->status = TASK_RUNNING;

}
*/

/* Change current_running to the next task */
void scheduler(void)
{

    // TODO schedule
    // Modify the current_running pointer.
    if(current_running->status != TASK_BLOCKED){
        current_running->status = TASK_READY;
        if(current_running->entry_point != 0){
            //initial pcb does not need to be pushed to ready_queue
            queue_push(&ready_queue, current_running);           
        }        
    }

    current_running = queue_dequeue(&ready_queue);
    if(now_priority[current_running->pid]<0) {
        pcb_t *base = current_running;
        do {
            queue_push(&ready_queue, current_running);
            current_running = queue_dequeue(&ready_queue);
            if(now_priority[current_running->pid]>=0) break;
        } while(base != current_running);
        if(now_priority[current_running->pid]<0){
            do {
                queue_push(&ready_queue, current_running);
                current_running = queue_dequeue(&ready_queue);
                now_priority[current_running->pid] = my_priority[current_running->pid];
            } while(base != current_running);            
        }
    }
    now_priority[current_running->pid]--;
    current_running->status = TASK_RUNNING;
}

void do_sleep(uint32_t sleep_time)
{
    // TODO sleep(seconds)
}

void do_block(queue_t *queue)
{
    // block the current_running task into the queue
    current_running->status = TASK_BLOCKED;
    queue_push(&block_queue, current_running);
    do_scheduler();
}

void do_unblock_one(queue_t *queue)
{
    // unblock the head task from the queue
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
