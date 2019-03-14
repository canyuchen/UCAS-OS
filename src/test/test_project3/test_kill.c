#include "time.h"
#include "test3.h"
#include "lock.h"
#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "queue.h"

#include "fs.h"

static char blank[] = {"                                                              "};

mutex_lock_t lock1;
mutex_lock_t lock2;
extern pcb_t *current_running;

mutex_lock_t *Lock[MAX_LOCK_NUM_TOTAL] = {0};

// static char test_fs_buff[64];

// void test_fs_1()
// {
//     sys_move_cursor(1,35);
//     printf("[DEBUG test_fs]");

//     int i, j;
//     int fd = sys_fopen("1.txt", O_RDWR);

//     for (i = 0; i < 10; i++)
//     {
//         sys_fwrite(fd, "hello world!\n", 13);
//     }

//     for (i = 0; i < 10; i++)
//     {
//         sys_fread(fd, test_fs_buff, 13);
//         sys_move_cursor(1,i);
//         for (j = 0; j < 13; j++)
//         {
//             printf("%c", test_fs_buff[j]);
//         }
//     }

//     sys_fclose(fd);
//     sys_exit();
// }

// pid = 2
void ready_to_exit_task()
{
    int i = 0, print_location = 0;

    mutex_lock_acquire(&lock1);

    mutex_lock_acquire(&lock2);

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

    sys_move_cursor(0, print_location);
    printf("> [TASK 2] I want to acquire a mute lock from task(pid=2).");

    mutex_lock_acquire(&lock1);

    sys_move_cursor(0, print_location);
    printf("> [TASK 2] I have acquired a mutex lock from task(pid=2).");

    sys_exit(); // test exit
}

// pid = 4
void wait_exit_task()
{
    int i, print_location = 2;

    sys_move_cursor(0, print_location);
    printf("> [TASK 2] I want to wait task (pid=2) to exit.");

    sys_waitpid(2); //test waitpid

    sys_move_cursor(0, print_location);
    printf("> [TASK 2] Task (pid=2) has exited.                ");

    sys_exit(); // test exit
}