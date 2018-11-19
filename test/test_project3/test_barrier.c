#include "time.h"
#include "stdio.h"
#include "syscall.h"
#include "barrier.h"
#include "test3.h"

// static barrier_t barrier;
// static int is_init = 0;
// static int num_task = 3;

// static barrier_t barrier;
// static int barrier_is_init = 0;
// static int barrier_num_task = 3;

barrier_t barrier;
int barrier_is_init = 0;
int barrier_num_task = 3;

// extern barrier_t barrier;
// extern int barrier_is_init;
// extern int barrier_num_task;

void barrier_task1(void)
{
    int i;
    int print_location = 0;

    if (!barrier_is_init)
    {
        barrier_is_init = 1;

        // sys_move_cursor(0, 4);
        // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
        // printf("%d :", queue_is_empty(&ready_queue));
        // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);

        barrier_init(&barrier, barrier_num_task);

        // barrier->max_num = goal;
        // barrier->not_arrive_num = goal;
        // queue_init(&(barrier->waiting_queue));

        // sys_move_cursor(0, 5);
        // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
        // printf("%d :", queue_is_empty(&ready_queue));
        // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
    }

    for (i = 0;; i++)
    {
        sys_move_cursor(0, print_location);
        printf("> [TASK] Ready to enter the barrier.(%d)", i);

        // sys_move_cursor(0, 4);
        // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
        // printf("%d :", queue_is_empty(&ready_queue));
        // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
        // if(!(((pcb_t *)(ready_queue.head))->next))
        //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

        barrier_wait(&barrier);

        // sys_move_cursor(0, 5);
        // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
        // printf("%d :", queue_is_empty(&ready_queue));
        // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
        // if(!(((pcb_t *)(ready_queue.head))->next))
        //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

        sys_move_cursor(0, print_location);
        printf("> [TASK] Exited barrier (%d).           ", i);

        sys_sleep(1);

        // sys_move_cursor(0, 6);
        // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
        // printf("%d :", queue_is_empty(&ready_queue));
        // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
        // if(!(((pcb_t *)(ready_queue.head))->next))
        //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

    }
}

void barrier_task2(void)
{
    int i;
    int print_location = 1;

    if (!barrier_is_init)
    {
        barrier_is_init = 1;

        // barrier->max_num = goal;
        // barrier->not_arrive_num = goal;
        // queue_init(&(barrier->waiting_queue));

        barrier_init(&barrier, barrier_num_task);
    }

    for (i = 0;; i++)
    {
        sys_move_cursor(0, print_location);
        printf("> [TASK] Ready to enter the barrier.(%d)", i);

        barrier_wait(&barrier);

        sys_move_cursor(0, print_location);
        printf("> [TASK] Exited barrier (%d).           ", i);

        sys_sleep(1);
    }
}

void barrier_task3(void)
{
    int i;
    int print_location = 2;

    if (!barrier_is_init)
    {
        barrier_is_init = 1;

        // barrier->max_num = goal;
        // barrier->not_arrive_num = goal;
        // queue_init(&(barrier->waiting_queue));

        barrier_init(&barrier, barrier_num_task);
    }

    for (i = 0;; i++)
    {
        sys_move_cursor(0, print_location);
        printf("> [TASK] Ready to enter the barrier.(%d)", i);

        barrier_wait(&barrier);

        sys_move_cursor(0, print_location);
        printf("> [TASK] Exited barrier (%d).           ", i);

        sys_sleep(1);
    }
}