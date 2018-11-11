#include "time.h"
#include "test2.h"
#include "sched.h"
#include "stdio.h"
#include "syscall.h"

#include "sched.h"

static char blank[] = {"                                                              "};
extern pcb_t *current_running;

void timer_task(void)
{
    int count = 0;
    int print_location = 8;
#ifdef DEBUG_P_2
    sys_move_cursor(1, 20);
    printf("%s", blank);

    sys_move_cursor(1, 20);
    printf("> [TIMER TASK] current running priority : %d.\n", current_running->priority);
#endif
    while (1)
    {
        /* call get_timer() to get time */
        uint32_t time = get_timer();
        sys_move_cursor(1, print_location);
        printf("> [TIMER TASK] This is a thread to timing! (%u/%u seconds).\n", time, time_elapsed);
    }
}
