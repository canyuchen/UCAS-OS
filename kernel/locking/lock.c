#include "lock.h"
#include "sched.h"
#include "syscall.h"

void spin_lock_init(spin_lock_t *lock)
{
    // CLOSE_INTERRUPT;

    lock->status = UNLOCKED;

    //START_INTERRUPT;

}

void spin_lock_acquire(spin_lock_t *lock)
{
    //CLOSE_INTERRUPT;

    while (LOCKED == lock->status)
    {
    };
    lock->status = LOCKED;

    //START_INTERRUPT;
}

void spin_lock_release(spin_lock_t *lock)
{
    //CLOSE_INTERRUPT;

    lock->status = UNLOCKED;

    //START_INTERRUPT;
}

void do_mutex_lock_init(mutex_lock_t *lock)
{
    //CLOSE_INTERRUPT;

    lock->status = UNLOCKED;

    //START_INTERRUPT;
}

void do_mutex_lock_acquire(mutex_lock_t *lock)
{
    //CLOSE_INTERRUPT;

    if(lock->status == LOCKED){
        do_block(&block_queue);
    }
    else{
        lock->status = LOCKED;        
    }
    //START_INTERRUPT;
}

void do_mutex_lock_release(mutex_lock_t *lock)
{
    //CLOSE_INTERRUPT;
    if(lock->status == LOCKED){
        lock->status = UNLOCKED;
    }
    
    do_unblock_one(&block_queue);    
    //do_unblock_all(&block_queue);

    //START_INTERRUPT;
}
