#include "sched.h"
#include "stdio.h"
#include "test2.h"
#include "syscall.h"

#include "sched.h"

static char blank[] = {"                                                              "};
extern pcb_t *current_running;

// static char blank[] = {"                    "};
static char plane1[] = {"    ___         _   "};
static char plane2[] = {"| __\\_\\______/_|  "};
static char plane3[] = {"<[___\\_\\_______|  "};
static char plane4[] = {"|  o'o              "};

void printf_task1(void)
{
    int i;
    int print_location = 1;
// #ifdef DEBUG_P_2
//     sys_move_cursor(1, 17);
//     printf("%s", blank);

//     sys_move_cursor(1, 17);
//     printf("> [PRINTF TASK1] current running priority : %d.\n", current_running->priority);
// #endif
    for (i = 0;; i++)
    {
        sys_move_cursor(1, print_location);
        printf("> [PRINTF TASK1] This task is to test scheduler. (%d)", i);
#ifdef DEBUG_P_2
        // sys_move_cursor(1, 17);
        // printf("%s", blank);

        sys_move_cursor(1, 17);
        printf("> [PRINTF TASK1] current running priority : %d.\n", current_running->priority);
#endif
    }
}

void printf_task2(void)
{
    int i;
    int print_location = 2;
// #ifdef DEBUG_P_2
//     sys_move_cursor(1, 18);
//     printf("%s", blank);

//     sys_move_cursor(1, 18);
//     printf("> [PRINTF TASK2] current running priority : %d.\n", current_running->priority);
// #endif
    for (i = 0;; i++)
    {
        sys_move_cursor(1, print_location);
        printf("> [PRINTF TASK2] This task is to test scheduler. (%d)", i);
#ifdef DEBUG_P_2
        // sys_move_cursor(1, 18);
        // printf("%s", blank);

        sys_move_cursor(1, 18);
        printf("> [PRINTF TASK2] current running priority : %d.\n", current_running->priority);
#endif
    }
}

void drawing_task2(void)
{
    int i, j = 40;

    while (1)
    {
        for (i = 55; i > 0; i--)
        {
            sys_move_cursor(i, j + 0);
            printf("%s", plane1);

            sys_move_cursor(i, j + 1);
            printf("%s", plane2);

            sys_move_cursor(i, j + 2);
            printf("%s", plane3);

            sys_move_cursor(i, j + 3);
            printf("%s", plane4);
        }

        sys_move_cursor(1, j + 0);
        printf("%s", blank);

        sys_move_cursor(1, j + 1);
        printf("%s", blank);

        sys_move_cursor(1, j + 2);
        printf("%s", blank);

        sys_move_cursor(1, j + 3);
        printf("%s", blank);
    }
}