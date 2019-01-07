#include "lock.h"
#include "sched.h"
#include "common.h"
#include "screen.h"
#include "syscall.h"

/* syscall function pointer */
int (*syscall[NUM_SYSCALLS])();

void system_call_helper(int fn, int arg1, int arg2, int arg3)
{
    int ret_val = 0;

    current_running->mode = KERNEL_MODE;

    ret_val = syscall[fn] (arg1,arg2,arg3);
    
    current_running->mode = USER_MODE;

    current_running->user_context.regs[2] = ret_val;

    current_running->user_context.cp0_epc = current_running->user_context.cp0_epc + 4;
}

void invalid_syscall(void)
{
    //TODO
}

void sys_sleep(uint32_t time)
{
    invoke_syscall(SYSCALL_SLEEP, time, IGNORE, IGNORE);
}

void sys_block(queue_t *queue)
{
    invoke_syscall(SYSCALL_BLOCK, (int)queue, IGNORE, IGNORE);
}

void sys_unblock_one(queue_t *queue)
{
    invoke_syscall(SYSCALL_UNBLOCK_ONE, (int)queue, IGNORE, IGNORE);
}

void sys_unblock_all(queue_t *queue)
{
    invoke_syscall(SYSCALL_UNBLOCK_ALL, (int)queue, IGNORE, IGNORE);
}

void sys_write(char *buff)
{
    invoke_syscall(SYSCALL_WRITE, (int)buff, IGNORE, IGNORE);
}

void sys_read(char *buff)
{
    //TODO
}

void sys_reflush()
{
    invoke_syscall(SYSCALL_REFLUSH, IGNORE, IGNORE, IGNORE);
}

void sys_move_cursor(int x, int y)
{
    invoke_syscall(SYSCALL_CURSOR, x, y, IGNORE);
}

void sys_screen_clear()
{
    invoke_syscall(SYSCALL_SCREEN_CLEAR, IGNORE, IGNORE, IGNORE);
}

void mutex_lock_init(mutex_lock_t *lock)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_INIT, (int)lock, IGNORE, IGNORE);
}

void mutex_lock_acquire(mutex_lock_t *lock)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_ACQUIRE, (int)lock, IGNORE, IGNORE);
}

void mutex_lock_release(mutex_lock_t *lock)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_RELEASE, (int)lock, IGNORE, IGNORE);
}

void semaphore_init(semaphore_t *semaphore, int n)
{
    invoke_syscall(SYSCALL_SEMAPHORE_INIT, (int)semaphore, n, IGNORE);
}

void semaphore_up(semaphore_t *semaphore)
{
    invoke_syscall(SYSCALL_SEMAPHORE_UP, (int)semaphore, IGNORE, IGNORE);
}

void semaphore_down(semaphore_t *semaphore)
{
    invoke_syscall(SYSCALL_SEMAPHORE_DOWN, (int)semaphore, IGNORE, IGNORE);
}


void condition_init(condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_INIT, (int)condition, IGNORE, IGNORE);
}

void condition_wait(mutex_lock_t *lock, condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_WAIT, (int)lock, (int)condition, IGNORE);
}

void condition_signal(condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_SIGNAL, (int)condition, IGNORE, IGNORE);
}

void condition_broadcast(condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_BROADCAST, (int)condition, IGNORE, IGNORE);
}

void barrier_init(barrier_t *barrier, int n)
{
    invoke_syscall(SYSCALL_BARRIER_INIT, (int)barrier, n, IGNORE);
}

void barrier_wait(barrier_t *barrier)
{
    invoke_syscall(SYSCALL_BARRIER_WAIT, (int)barrier, IGNORE, IGNORE);
}

void sys_spawn(task_info_t *task_info)
{
    invoke_syscall(SYSCALL_SPAWN, (int)task_info, IGNORE, IGNORE);
}

void sys_exit()
{
    invoke_syscall(SYSCALL_EXIT, IGNORE, IGNORE, IGNORE);
}

int sys_getpid()
{
    invoke_syscall(SYSCALL_GETPID, IGNORE, IGNORE, IGNORE);
}

void sys_waitpid(int n)
{
    invoke_syscall(SYSCALL_WAITPID, n, IGNORE, IGNORE);
}

void sys_kill(int n)
{
    invoke_syscall(SYSCALL_KILL, n, IGNORE, IGNORE);
}

void sys_ps()
{
    invoke_syscall(SYSCALL_PS, IGNORE, IGNORE, IGNORE);
}

int sys_scanf(int *mem)
{
    invoke_syscall(SYSCALL_SCANF, (int)mem, IGNORE, IGNORE);
}

//P5

void sys_init_mac()
{
    invoke_syscall(SYSCALL_INIT_MAC, IGNORE, IGNORE, IGNORE);
}

// int sys_net_send(uint32_t tgt DMA send desc, uint32_t td_phy)
void sys_net_send(uint32_t td, uint32_t td_phy)
{
    invoke_syscall(SYSCALL_NET_SEND, (int)td, (int)td_phy, IGNORE);
}

// int sys_net_recv(uint32_t, uint32_t, uint32_t)
int sys_net_recv(uint32_t rd, uint32_t rd_phy, uint32_t daddr)
{
    invoke_syscall(SYSCALL_NET_RECV, (int)rd, (int)rd_phy, (int)daddr);
}

void sys_wait_recv_package()
{
    invoke_syscall(SYSCALL_WAIT_RECV_PACKAGE, IGNORE, IGNORE, IGNORE);
}

void sys_net_fast_recv(uint32_t rd, uint32_t rd_phy, uint32_t daddr)
{
    invoke_syscall(SYSCALL_NET_FAST_RECV, (int)rd, (int)rd_phy, (int)daddr);
}

//P6

int sys_fopen(char *name, uint32_t mode)
{
    invoke_syscall(SYSCALL_FS_OPEN, (int)name, (int)mode, IGNORE);
}

void sys_fwrite(int fd, char *content, int length)
{
    invoke_syscall(SYSCALL_FS_WRITE, fd, (int)content, length);
}

void sys_fread(int fd, char *buffer, int length)
{
    invoke_syscall(SYSCALL_FS_READ, fd, (int)buffer, length);
}

void sys_fclose(int fd)
{
    invoke_syscall(SYSCALL_FS_CLOSE, fd, IGNORE, IGNORE);
}

void sys_fexit()
{
    invoke_syscall(SYSCALL_FS_EXIT, IGNORE, IGNORE, IGNORE);
}

void sys_mkfs()
{
    invoke_syscall(SYSCALL_FS_MKFS, IGNORE, IGNORE, IGNORE);
}

void sys_statfs()
{
    invoke_syscall(SYSCALL_FS_STATFS, IGNORE, IGNORE, IGNORE);
}