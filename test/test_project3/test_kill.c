#include "time.h"
#include "test3.h"
#include "lock.h"
#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "queue.h"

static char blank[] = {"                                                              "};

mutex_lock_t lock1;
mutex_lock_t lock2;
extern pcb_t *current_running;

mutex_lock_t *Lock[MAX_LOCK_NUM_TOTAL] = {0};

// pid = 2
void ready_to_exit_task()
{
    int i = 0, print_location = 0;

    // sys_move_cursor(0, 12);
    // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
    // printf("%d :", queue_is_empty(&ready_queue));
    // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
    // if(!(((pcb_t *)(ready_queue.head))->next))
    //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

    mutex_lock_acquire(&lock1);

    // sys_move_cursor(0, 5);
    // printf("%d : ", current_running->lock_num);
    // printf("%d %d : ", current_running->lock[0], current_running->lock[1]);
    // printf("%d %d : ", Lock[0]->status, Lock[1]->status);

    mutex_lock_acquire(&lock2);

    // sys_move_cursor(0, 6);
    // printf("%d : ", current_running->lock_num);
    // printf("%d %d : ", current_running->lock[0], current_running->lock[1]);
    // printf("%d %d : ", Lock[0]->status, Lock[1]->status);

    // sys_spawn(&task1);
    // sys_spawn(&task2);

    for (i = 0; i < 50000; i++)
    {
        sys_move_cursor(0, print_location);
        printf("> [TASK 1] I am task with pid %d, I have acquired two mutex lock. (%d)", current_running->pid, i++);
    }
    sys_exit(); // test exit
}

// pid = 3
void wait_lock_task()
{
    int i, print_location = 1;

    // sys_move_cursor(0, 11);
    // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
    // printf("%d :", queue_is_empty(&ready_queue));
    // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
    // if(!(((pcb_t *)(ready_queue.head))->next))
    //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

    sys_move_cursor(0, print_location);
    printf("> [TASK 2] I want to acquire a mute lock from task(pid=2).");

    mutex_lock_acquire(&lock1);

    // sys_move_cursor(0, 10);
    // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
    // printf("%d :", queue_is_empty(&ready_queue));
    // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
    // if(!(((pcb_t *)(ready_queue.head))->next))
    //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

    sys_move_cursor(0, print_location);
    printf("> [TASK 2] I have acquired a mutex lock from task(pid=2).");

    // sys_move_cursor(0, 8);
    // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
    // printf("%d :", queue_is_empty(&ready_queue));
    // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
    // if(!(((pcb_t *)(ready_queue.head))->next))
    //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

    sys_exit(); // test exit
}

// pid = 4
void wait_exit_task()
{
    int i, print_location = 2;

    // sys_move_cursor(0, 9);
    // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
    // printf("%d :", queue_is_empty(&ready_queue));
    // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
    // if(!(((pcb_t *)(ready_queue.head))->next))
    //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

    sys_move_cursor(0, print_location);
    printf("> [TASK 2] I want to wait task (pid=2) to exit.");

    sys_waitpid(2); //test waitpid

    // sys_move_cursor(0, 7);
    // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
    // printf("%d :", queue_is_empty(&ready_queue));
    // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
    // if(!(((pcb_t *)(ready_queue.head))->next))
    //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

    sys_move_cursor(0, print_location);
    printf("> [TASK 2] Task (pid=2) has exited.                ");

    sys_exit(); // test exit
}