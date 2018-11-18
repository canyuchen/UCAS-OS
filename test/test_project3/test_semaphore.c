#include "sem.h"
#include "test3.h"
#include "time.h"
#include "stdio.h"
#include "syscall.h"

static semaphore_t semaphore;
static int global_count = 0;

void semaphore_add_task1(void)
{
    int i;
    int print_location = 0;
    // int sum_up = 0;

    semaphore_init(&semaphore, 1);

    for (i = 0; i < 10; i++)
    {
        semaphore_down(&semaphore); // semaphore.value--
        // semaphore = 0
        global_count++; 
        semaphore_up(&semaphore);

        // sys_move_cursor(0, 4);
        // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
        // printf("%d :", queue_is_empty(&ready_queue));
        // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
        // if(!(((pcb_t *)(ready_queue.head))->next))
        //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);


        sys_move_cursor(0, print_location);
        printf("> [TASK] current global value %d. (%d)", global_count, i + 1);

        // sys_move_cursor(0, 5);
        // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
        // printf("%d :", queue_is_empty(&ready_queue));
        // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
        // if(!(((pcb_t *)(ready_queue.head))->next))
        //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

        sys_sleep(1);

        // sys_move_cursor(0, 6);
        // printf("%d :", check_in_queue(&ready_queue, &pcb[1]));
        // printf("%d :", queue_is_empty(&ready_queue));
        // printf("%d :", ((pcb_t *)(ready_queue.head))->pid);
        // if(!(((pcb_t *)(ready_queue.head))->next))
        //     printf("%d :", ((pcb_t *)(((pcb_t *)(ready_queue.head))->next))->pid);

    }

    sys_exit();
}

void semaphore_add_task2(void)
{
    int i;
    int print_location = 1;
    // int sum_down = 0;

    for (i = 0; i < 20; i++)
    {
        semaphore_down(&semaphore); // semaphore.value--
        // semaphore = 0
        global_count++; 
        semaphore_up(&semaphore);

        sys_move_cursor(0, print_location);
        printf("> [TASK] current global value %d. (%d)", global_count, i + 1);

        sys_sleep(1);
    }

    sys_exit();
}

void semaphore_add_task3(void)
{
    int i;
    int print_location = 2;
    // int sum_down = 0;

    for (i = 0; i < 30; i++)
    {
        semaphore_down(&semaphore); // semaphore.value--
        // semaphore = 0
        global_count++; 
        semaphore_up(&semaphore);

        sys_move_cursor(0, print_location);
        printf("> [TASK] current global value %d. (%d)", global_count, i + 1);

        sys_sleep(1);
    }

    sys_exit();
}