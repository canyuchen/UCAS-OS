#include "lock.h"
#include "sched.h"
#include "common.h"
#include "screen.h"
#include "syscall.h"

/* syscall function pointer */
int (*syscall[NUM_SYSCALLS])();

/* This function handles executing a given syscall, and returns the result
   to syscall_entry in entry.S, from where it is returned to the calling
   process. Before we get here, syscall_entry() will have stored the
   context of the process making the syscall, and entered a critical
   section (through enter_critical()).
*/
void system_call_helper(int fn, int arg1, int arg2, int arg3)
{
    int ret_val = 0;

    current_running->mode = KERNEL_MODE;

    // Call function and return result as usual (ie, "return ret_val");

    /* In C's calling convention, caller is responsible for cleaning up
     * the stack. Therefore we don't really need to distinguish between
     * different argument numbers. Just pass all 3 arguments and it will
     * work 
    */
    ret_val = syscall[fn] (arg1,arg2,arg3);
    
    current_running->mode = USER_MODE;

    // We can not leave the critical section we enter here before we
    // return in syscall_entry.
    // This is due to a potential race condition on a scratch variable
    // used by syscall_entry.
    current_running->user_context.regs[2] = ret_val;

    if(fn != SYSCALL_MUTEX_LOCK_ACQUIRE){
        current_running->user_context.cp0_epc = current_running->user_context.cp0_epc + 4;        
    }

    // current_running->user_context.cp0_epc = current_running->user_context.cp0_epc + 4;
    // printf(20, 10, "syscall: %d, nested_count: %d", fn, current_running->nested_count);

    // leave_critical() will be called during restoring context.
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

