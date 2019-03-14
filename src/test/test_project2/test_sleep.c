#include "test2.h"
#include "sched.h"
#include "stdio.h"
#include "syscall.h"

#include "sched.h"

extern pcb_t *current_running;

static char blank[] = {"                                                              "};

void sleep_task(void)
{
    int i;
    int print_location_1 = 9;
    int print_location_2 = 10;
    int print_location_3 = 11;
    int sleep_time = 5;
#ifdef DEBUG_P_2
    sys_move_cursor(1, 19);
    printf("%s", blank);

    sys_move_cursor(1, 19);
    printf("> [SLEEP TASK] current running priority : %d.\n", current_running->priority);
#endif
    while (1)
    {
        for (i = 0; i < 20; i++)
        {
            sys_move_cursor(1, print_location_1);
            printf("%s", blank);

            sys_move_cursor(1, print_location_1);
            printf("> [SLEEP TASK] This task is to test sleep(). (%d)\n", i);
        }
        sys_move_cursor(1, print_location_2);
        printf("%s", blank);
            
        sys_move_cursor(1, print_location_2);
        printf("> [SLEEP TASK] This task is sleeping, sleep time is %d.\n", sleep_time);

        /* call syscall sleep() */
        sys_sleep(sleep_time);

        sys_move_cursor(1, print_location_3);
        printf("%s", blank);
            
        sys_move_cursor(1, print_location_3);
        printf("> [SLEEP TASK] Sleeping time finished.\n");

        sys_move_cursor(1, print_location_2);
        printf("%s", blank);
    }
}