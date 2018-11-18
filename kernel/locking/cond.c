#include "cond.h"
#include "lock.h"

void do_condition_init(condition_t *condition)
{
    queue_init(&(condition->waiting_queue));
}

void do_condition_wait(mutex_lock_t *lock, condition_t *condition)
{
    queue_push(&(condition->waiting_queue), current_running);
    current_running->status = TASK_BLOCKED;
    do_mutex_lock_release(lock);
    do_scheduler();
    do_mutex_lock_acquire(lock);
}

void do_condition_signal(condition_t *condition)
{
    if(!queue_is_empty(&(condition->waiting_queue))){
        pcb_t *pcb = queue_dequeue(&(condition->waiting_queue));
        pcb->status = TASK_READY;
        queue_push(&ready_queue, pcb);
    }
}

void do_condition_broadcast(condition_t *condition)
{
    while(!queue_is_empty(&(condition->waiting_queue))){
        pcb_t *pcb = queue_dequeue(&(condition->waiting_queue));
        pcb->status = TASK_READY;
        queue_push(&ready_queue, pcb);
    }
}