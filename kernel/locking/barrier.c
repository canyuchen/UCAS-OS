#include "barrier.h"

void do_barrier_init(barrier_t *barrier, int goal)
{
    barrier->max_num = goal;
    barrier->not_arrive_num = goal;
    queue_init(&(barrier->waiting_queue));
}

void do_barrier_wait(barrier_t *barrier)
{
    if(barrier->not_arrive_num == 1){
        clear_waiting_queue(&(barrier->waiting_queue));
        barrier->not_arrive_num = barrier->max_num;
    }
    else{
        barrier->not_arrive_num--;
        current_running->status = TASK_BLOCKED;
        queue_push(&(barrier->waiting_queue), current_running);
        do_scheduler();
    }
}