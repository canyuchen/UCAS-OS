/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *                       System call related processing
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#ifndef INCLUDE_SYSCALL_H_
#define INCLUDE_SYSCALL_H_

#include "type.h"
#include "sync.h"
#include "queue.h"
#include "sched.h"

#define IGNORE 0
#define NUM_SYSCALLS 64

/* define */
#define SYSCALL_SLEEP 2

#define SYSCALL_BLOCK 10
#define SYSCALL_UNBLOCK_ONE 11
#define SYSCALL_UNBLOCK_ALL 12

#define SYSCALL_WRITE 20
#define SYSCALL_READ 21
#define SYSCALL_CURSOR 22
#define SYSCALL_REFLUSH 23
#define SYSCALL_SCREEN_CLEAR 24

#define SYSCALL_MUTEX_LOCK_INIT 30
#define SYSCALL_MUTEX_LOCK_ACQUIRE 31
#define SYSCALL_MUTEX_LOCK_RELEASE 32

#define SYSCALL_SEMAPHORE_INIT 33
#define SYSCALL_SEMAPHORE_UP 34
#define SYSCALL_SEMAPHORE_DOWN 35
#define SYSCALL_CONDITION_INIT 36
#define SYSCALL_CONDITION_WAIT 37
#define SYSCALL_CONDITION_SIGNAL 38
#define SYSCALL_CONDITION_BROADCAST 39
#define SYSCALL_BARRIER_INIT 40
#define SYSCALL_BARRIER_WAIT 41

#define SYSCALL_EXIT 42
#define SYSCALL_WAITPID 43
#define SYSCALL_GETPID 44
#define SYSCALL_SPAWN 45
#define SYSCALL_KILL 46
#define SYSCALL_PS 47

#define SYSCALL_SCANF 48

#define SYSCALL_NET_RECV 49
#define SYSCALL_WAIT_RECV_PACKAGE 50
#define SYSCALL_INIT_MAC 51
#define SYSCALL_NET_SEND 52
#define SYSCALL_NET_FAST_RECV 53

#define SYSCALL_FS_OPEN 54
#define SYSCALL_FS_WRITE 55
#define SYSCALL_FS_READ 56
#define SYSCALL_FS_CLOSE 57
#define SYSCALL_FS_EXIT 58

/* syscall function pointer */
extern int (*syscall[NUM_SYSCALLS])();

extern void system_call_helper(int, int, int, int);
extern int invoke_syscall(int, int, int, int);

extern void sys_sleep(uint32_t);

extern void sys_block(queue_t *);
extern void sys_unblock_one(queue_t *);
extern void sys_unblock_all(queue_t *);

extern void sys_write(char *);
extern void sys_read(char *);
extern void sys_move_cursor(int, int);
extern void sys_reflush();
extern void sys_screen_clear();

extern void mutex_lock_init(mutex_lock_t *);
extern void mutex_lock_acquire(mutex_lock_t *);
extern void mutex_lock_release(mutex_lock_t *);

extern void invalid_syscall(void);

extern void semaphore_init(semaphore_t *semaphore, int n);
extern void semaphore_up(semaphore_t *semaphore);
extern void semaphore_down(semaphore_t *semaphore);
extern void condition_init(condition_t *condition);
extern void condition_wait(mutex_lock_t *lock, condition_t *condition);
extern void condition_signal(condition_t *condition);
extern void condition_broadcast(condition_t *condition);
extern void barrier_init(barrier_t *barrier, int n);
extern void barrier_wait(barrier_t *barrier);

extern void sys_spawn(task_info_t *task_info);
extern void sys_exit();
extern int  sys_getpid();
extern void sys_waitpid(int n);
extern void sys_kill(int n);
extern void sys_ps();

extern int sys_scanf(int *mem);

extern void sys_init_mac();
// int sys_net_send(uint32_t tgt DMA send desc, uint32_t td_phy)
extern void sys_net_send(uint32_t td, uint32_t td_phy);
// int sys_net_recv(uint32_t, uint32_t, uint32_t)
extern int sys_net_recv(uint32_t rd, uint32_t rd_phy, uint32_t daddr);
extern void sys_wait_recv_package();

extern int sys_fopen(char *name, uint32_t mode);
extern void sys_fwrite(int fd, char *content, int length);
extern void sys_fread(int fd, char *buffer, int length);
extern void sys_fclose(int fd);
extern void sys_fexit();

#endif