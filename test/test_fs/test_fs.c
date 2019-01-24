#include "test6.h"

#include "fs.h"
#include "stdio.h"
#include "string.h"
#include "syscall.h"

static char Buff[64];

void test_fs(void)
{
    // sys_move_cursor(1,35);
    // printf("[DEBUG test_fs]");

    int i, j;
    int fd = sys_fopen("1.txt", O_RDWR);

    for (i = 0; i < 10; i++)
    {
        sys_fwrite(fd, "hello world!\n", 13);
    }

    for (i = 0; i < 10; i++)
    {
        sys_fread(fd, Buff, 13);
        vt100_move_cursor(1,i);
        // sys_move_cursor(1,i);
        for (j = 0; j < 13; j++)
        {
            // printf("%c", Buff[j]);
            printk("%c", Buff[j]);
        }
    }

    sys_fclose(fd);
    sys_exit();
}