#include "test2.h"
#include "lock.h"
#include "stdio.h"
#include "syscall.h"
#include "sched.h"

//int is_init = FALSE;
extern int is_init;
static char blank[] = {"                                             "};

/* if you want to use spin lock, you need define SPIN_LOCK */
//  #define SPIN_LOCK
spin_lock_t spin_lock;

/* if you want to use mutex lock, you need define MUTEX_LOCK */
//#define MUTEX_LOCK
extern mutex_lock_t mutex_lock;

void lock_task1(void)
{
        //CLOSE_INTERRUPT;
        int print_location = 3;
        while (1)
        {
                int i;
                // int j = 9;

                if (!is_init)
                {

#ifdef SPIN_LOCK
                        spin_lock_init(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                        mutex_lock_init(&mutex_lock);

                        //sys_move_cursor(1, j);
                        //printf("1 lock init : mutex_lock = %d (%d)\n", mutex_lock, j);
#endif
                        is_init = TRUE;
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Applying for a lock.\n");

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

                // sys_move_cursor(1, j+1);
                // printf("1 lock to be acquired : mutex_lock = %d (%d)\n ", mutex_lock, j);

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock);
#endif

                // sys_move_cursor(1, j+2);
                // printf("1 lock acquired : mutex_lock = %d\n", mutex_lock);

                //CLOSE_INTERRUPT;
                for (i = 0; i < 20; i++)
                {
                        //vt100_move_cursor(1, print_location);
                        sys_move_cursor(1,print_location);

                        //CLOSE_INTERRUPT;

                        printf("> [TASK] Has acquired lock and running.(%d)\n", i);

                        //CLOSE_INTERRUPT;
                }
                //START_INTERRUPT;

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Has acquired lock and exited.\n");

#ifdef SPIN_LOCK
                spin_lock_release(&spin_lock);
#endif

                // sys_move_cursor(1, j+3);
                // printf("1 lock to be released : mutex_lock = %d\n", mutex_lock);

#ifdef MUTEX_LOCK
                mutex_lock_release(&mutex_lock);
#endif

                // sys_move_cursor(1, j+4);
                // printf("1 lock released : mutex_lock = %d\n", mutex_lock);

                // j += 5;
        }
        //START_INTERRUPT;
}

void lock_task2(void)
{
        //CLOSE_INTERRUPT;
        int print_location = 4;
        while (1)
        {
                int i;
                if (!is_init)
                {

#ifdef SPIN_LOCK
                        spin_lock_init(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                        mutex_lock_init(&mutex_lock);
#endif

                //         sys_move_cursor(1, 14);
                //    //     printf("2 lock init : mutex_lock = %d\n", mutex_lock);

                        is_init = TRUE;
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Applying for a lock.\n");
                // sys_move_cursor(1, j+1);
                // printf("1 lock to be acquired : mutex_lock = %d (%d)\n ", mutex_lock, j);


#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

        //         sys_move_cursor(1, 15);
        //    //     printf("2 lock to be acquired : mutex_lock = %d\n", mutex_lock);

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock);
#endif

        //         sys_move_cursor(1, 16);
        //   //      printf("2 lock acquired : mutex_lock = %d\n", mutex_lock);


                //CLOSE_INTERRUPT;
                for (i = 0; i < 20; i++)
                {
                        //vt100_move_cursor(1, print_location);
                        sys_move_cursor(1,print_location);

                       // CLOSE_INTERRUPT;

                        printf("> [TASK] Has acquired lock and running.(%d)\n", i);

                       // CLOSE_INTERRUPT;
                }
                //START_INTERRUPT;

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Has acquired lock and exited.\n");

#ifdef SPIN_LOCK
                spin_lock_release(&spin_lock);
#endif

        //         sys_move_cursor(1, 17);
        // //        printf("2 lock to be released : mutex_lock = %d\n", mutex_lock);

#ifdef MUTEX_LOCK
                mutex_lock_release(&mutex_lock);
#endif

        //         sys_move_cursor(1, 18);
        // //        printf("2 lock released : mutex_lock = %d\n", mutex_lock);
        }
        //START_INTERRUPT;
}