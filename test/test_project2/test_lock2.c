
#undef DEBUG

#ifdef DEBUG

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
        int k = 0;
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
                        is_init = TRUE;
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, 7);
                printf("> mutex_lock : %d ", mutex_lock.status);
                printf("> [TASK] [%d] Applying for a lock.\n", k);

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock);
#endif
                sys_move_cursor(1, 8);
                printf("1> mutex_lock : %d \n", mutex_lock.status);
                for (i = 0; i < 20; i++)
                {
                        sys_move_cursor(1,9);
                        printf("2> mutex_lock : %d \n", mutex_lock.status);
                        sys_move_cursor(1,10);
                        printf("> [TASK] [%d] Has acquired lock and running.(%d)\n", k, i);
                        sys_move_cursor(1,11);
                        printf("3> mutex_lock : %d \n", mutex_lock.status);
                }
                sys_move_cursor(1,12);
                printf("4> mutex_lock : %d \n", mutex_lock.status);

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, 13);
                printf("> mutex_lock : %d ", mutex_lock.status);
                printf("> [TASK] [%d] Has acquired lock and exited.\n", k);

#ifdef SPIN_LOCK
                spin_lock_release(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_release(&mutex_lock);
                sys_move_cursor(1, 14);
                printf("> mutex_lock : %d ", mutex_lock.status);
#endif
        k++;
        }
}

void lock_task2(void)
{
        int print_location = 4;
        int k = 0;
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

                        is_init = TRUE;
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, 15);
                printf("> mutex_lock : %d ", mutex_lock.status);
                printf("> [TASK] [%d] Applying for a lock.\n", k);

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock);
#endif
                sys_move_cursor(1,16);
                printf("1> mutex_lock : %d \n", mutex_lock.status);
                for (i = 0; i < 20; i++)
                {
                        sys_move_cursor(1,17);
                        printf("2> mutex_lock : %d \n", mutex_lock.status);
                        sys_move_cursor(1,18);
                        printf("> [TASK] [%d] Has acquired lock and running.(%d)\n", k, i);
                        sys_move_cursor(1,19);
                        printf("3> mutex_lock : %d \n", mutex_lock.status);
                }
                sys_move_cursor(1,20);
                printf("4> mutex_lock : %d \n", mutex_lock.status);

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, 21);
                printf("> mutex_lock : %d ", mutex_lock.status);
                printf("> [TASK] [%d] Has acquired lock and exited.\n", k);

#ifdef SPIN_LOCK
                spin_lock_release(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_release(&mutex_lock);
                sys_move_cursor(1, 22);
                printf("> mutex_lock : %d ", mutex_lock.status);
#endif
        k++;

        }
}

#endif

// #include "test2.h"
// #include "lock.h"
// #include "stdio.h"
// #include "syscall.h"

// //int is_init = FALSE;
// extern int is_init;
// static char blank[] = {"                                             "};

// /* if you want to use spin lock, you need define SPIN_LOCK */
// //  #define SPIN_LOCK
// spin_lock_t spin_lock;

// /* if you want to use mutex lock, you need define MUTEX_LOCK */
// #define MUTEX_LOCK
// extern mutex_lock_t mutex_lock;

// void lock_task1(void)
// {
//         int print_location = 3;
//         while (1)
//         {
//                 int i;
//                 if (!is_init)
//                 {

// #ifdef SPIN_LOCK
//                         spin_lock_init(&spin_lock);
// #endif

// #ifdef MUTEX_LOCK
//                         mutex_lock_init(&mutex_lock);
// #endif
//                         is_init = TRUE;
//                 }

//                 sys_move_cursor(1, print_location);
//                 printf("%s", blank);

//                 sys_move_cursor(1, print_location);
//                 printf("> [TASK] Applying for a lock.\n");

// #ifdef SPIN_LOCK
//                 spin_lock_acquire(&spin_lock);
// #endif

// #ifdef MUTEX_LOCK
//                 mutex_lock_acquire(&mutex_lock);
// #endif

//                 for (i = 0; i < 20; i++)
//                 {
//                         sys_move_cursor(1, print_location);
//                         printf("> [TASK] Has acquired lock and running.(%d)\n", i);
//                 }

//                 sys_move_cursor(1, print_location);
//                 printf("%s", blank);

//                 sys_move_cursor(1, print_location);
//                 printf("> [TASK] Has acquired lock and exited.\n");

// #ifdef SPIN_LOCK
//                 spin_lock_release(&spin_lock);
// #endif

// #ifdef MUTEX_LOCK
//                 mutex_lock_release(&mutex_lock);

//                 sys_move_cursor(1, print_location);
//                 printf("%s", blank);

//                 sys_move_cursor(1, print_location);
//                 printf("> [TASK] The lock has been released.\n");
// #endif
//         }
// }

// void lock_task2(void)
// {
//         int print_location = 4;
//         while (1)
//         {
//                 int i;
//                 if (!is_init)
//                 {

// #ifdef SPIN_LOCK
//                         spin_lock_init(&spin_lock);
// #endif

// #ifdef MUTEX_LOCK
//                         mutex_lock_init(&mutex_lock);
// #endif
//                         is_init = TRUE;
//                 }

//                 sys_move_cursor(1, print_location);
//                 printf("%s", blank);

//                 sys_move_cursor(1, print_location);
//                 printf("> [TASK] Applying for a lock.\n");

// #ifdef SPIN_LOCK
//                 spin_lock_acquire(&spin_lock);
// #endif

// #ifdef MUTEX_LOCK
//                 mutex_lock_acquire(&mutex_lock);
// #endif

//                 for (i = 0; i < 20; i++)
//                 {
//                         sys_move_cursor(1, print_location);
//                         printf("> [TASK] Has acquired lock and running.(%d)\n", i);
//                 }

//                 sys_move_cursor(1, print_location);
//                 printf("%s", blank);

//                 sys_move_cursor(1, print_location);
//                 printf("> [TASK] Has acquired lock and exited.\n");

// #ifdef SPIN_LOCK
//                 spin_lock_release(&spin_lock);
// #endif

// #ifdef MUTEX_LOCK
//                 mutex_lock_release(&mutex_lock);

//                 sys_move_cursor(1, print_location);
//                 printf("%s", blank);
                
//                 sys_move_cursor(1, print_location);
//                 printf("> [TASK] The lock has been released.\n");
// #endif
//         }
// }


#include "test2.h"
#include "lock.h"
#include "stdio.h"
#include "syscall.h"

//int is_init = FALSE;
extern int is_init;
static char blank[] = {"                                                              "};

/* if you want to use spin lock, you need define SPIN_LOCK */
//  #define SPIN_LOCK
spin_lock_t spin_lock;

/* if you want to use mutex lock, you need define MUTEX_LOCK */
#define MUTEX_LOCK
extern mutex_lock_t mutex_lock;

// mutex_lock_t mutex_lock_1;
// mutex_lock_t mutex_lock_2;

void lock_task1(void)
{
        int print_location_1 = 3;
        int print_location_2 = 4;

        while (1)
        {
                int i;
                if (!is_init)
                {

#ifdef SPIN_LOCK
                        spin_lock_init(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                        //mutex_lock_init(&mutex_lock);
                        mutex_lock_init(&mutex_lock_1);
                        mutex_lock_init(&mutex_lock_2);
#endif
                        is_init = TRUE;
                }

                sys_move_cursor(1, print_location_1);
                printf("%s", blank);

                sys_move_cursor(1, print_location_1);
                printf("> [LOCK TASK1] Applying for lock 1.\n");

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock_1);
#endif

                for (i = 0; i < 20; i++)
                {
                        sys_move_cursor(1, print_location_1);
                        printf("> [LOCK TASK1] Has acquired lock 1 and running.(%d)\n", i);
                }

                sys_move_cursor(1, print_location_1);
                printf("%s", blank);

                sys_move_cursor(1, print_location_1);
                printf("> [LOCK TASK1] Has acquired lock 1 and exited.\n");

                sys_move_cursor(1, print_location_2);
                printf("%s", blank);

                sys_move_cursor(1, print_location_2);
                printf("> [LOCK TASK1] Applying for lock 2.\n");

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock_2);
#endif

                for (i = 0; i < 20; i++)
                {
                        sys_move_cursor(1, print_location_2);
                        printf("> [LOCK TASK1] Has acquired lock 2 and running.(%d)\n", i);
                }

                sys_move_cursor(1, print_location_2);
                printf("%s", blank);

                sys_move_cursor(1, print_location_2);
                printf("> [LOCK TASK1] Has acquired lock 2 and exited.\n");

#ifdef SPIN_LOCK
                spin_lock_release(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_release(&mutex_lock_1);

                sys_move_cursor(1, print_location_1);
                printf("%s", blank);

                sys_move_cursor(1, print_location_1);
                printf("> [LOCK TASK1] The lock 1 has been released.\n");

                mutex_lock_release(&mutex_lock_2);

                sys_move_cursor(1, print_location_2);
                printf("%s", blank);

                sys_move_cursor(1, print_location_2);
                printf("> [LOCK TASK1] The lock 2 has been released.\n");
#endif
        }
}







