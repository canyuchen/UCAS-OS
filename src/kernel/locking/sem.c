#include "sem.h"
#include "stdio.h"

void do_semaphore_init(semaphore_t *s, int val)
{
    s->sem_value = val;
    queue_init(&(s->waiting_queue));
}

void do_semaphore_up(semaphore_t *s)
{
    if(!queue_is_empty(&(s->waiting_queue))){
        pcb_t *pcb = queue_dequeue(&(s->waiting_queue));
        pcb->status = TASK_READY;
        queue_push(&ready_queue, pcb);
    }
    else{
        s->sem_value++;
    }
}

void do_semaphore_down(semaphore_t *s)
{
    if(s->sem_value == 0){
        current_running->status = TASK_BLOCKED;
        queue_push(&(s->waiting_queue), current_running);
        do_scheduler();
    }
    else{
        s->sem_value--;
    }
}