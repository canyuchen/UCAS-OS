#include "lock.h"
#include "sched.h"
#include "syscall.h"

void spin_lock_init(spin_lock_t *lock)
{
    lock->status = UNLOCKED;
}

void spin_lock_acquire(spin_lock_t *lock)
{
    while (LOCKED == lock->status)
    {
    };
    lock->status = LOCKED;
}

void spin_lock_release(spin_lock_t *lock)
{
    lock->status = UNLOCKED;
}

void do_mutex_lock_init(mutex_lock_t *lock)
{
    lock->status = UNLOCKED;
	//queue_init(&(lock->mutex_lock_queue));
}


void do_mutex_lock_acquire(mutex_lock_t *lock)
{
    // if(lock->status == LOCKED){
    //     do_block(&block_queue);
    // }
    while(lock->status == LOCKED){
        do_block(&(lock->mutex_lock_queue));
    }
    lock->status = LOCKED;        
}

void do_mutex_lock_release(mutex_lock_t *lock)
{
    if(lock->status == LOCKED){
        lock->status = UNLOCKED;
        do_unblock_one(&(lock->mutex_lock_queue));            
    }
}
