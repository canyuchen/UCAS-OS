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
    queue_init(&(lock->mutex_lock_queue));
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
    // current_running->lock[current_running->lock_num] = lock;
    // current_running->lock_num++;
    int i = 0;
    while(current_running->lock[i] != NULL){
        // current_running->lock[i] = lock;
        // current_running->lock_num++;
        i++;
        if(i >= LOCK_MAX_NUM){
            //ERROR handle
            return;
        }
    }
    current_running->lock[i] = lock;
    current_running->lock_num++;
}
/*
void do_mutex_lock_release(mutex_lock_t *lock)
{
    if(lock->status == LOCKED){
        lock->status = UNLOCKED;
        int i = 0;
        while(current_running->lock[i] != lock){
            current_running->lock[i] = 0;
            current_running->lock_num--;
            i++;
            if(i >= LOCK_MAX_NUM){
                //ERROR handle
                return;
            }
        }
        //do_unblock_one(&(lock->mutex_lock_queue));      
        do_unblock_all(&(lock->mutex_lock_queue));         
    }
}
*/

void do_mutex_lock_release(mutex_lock_t *lock)
{
    if(lock->status == LOCKED){
        lock->status = UNLOCKED;
    /*
        int i = 0;
        while(current_running->lock[i] != lock){
            current_running->lock[i] = 0;
            current_running->lock_num--;
            i++;
            if(i >= LOCK_MAX_NUM){
                //ERROR handle
                return;
            }
        }
    */
        pcb_t *head = lock->mutex_lock_queue.head;
        while(head != NULL){
            int i = 0;
            while(head->lock[i] != lock){
                // head->lock[i] = 0;
                // head->lock_num--;
                i++;
                if(i >= LOCK_MAX_NUM){
                    //ERROR handle
                    break;
                }
                head->lock[i] = NULL;
                head->lock_num--;
            }
            head = head->next;            
        }

        //do_unblock_one(&(lock->mutex_lock_queue));      
        do_unblock_all(&(lock->mutex_lock_queue));         
    }
}
