#ifndef INCLUDE_BARRIER_H_
#define INCLUDE_BARRIER_H_

#include "queue.h"
#include "sched.h"

typedef struct barrier{
    int max_num;
    int not_arrive_num;
    queue_t waiting_queue;
} barrier_t;

void do_barrier_init(barrier_t *, int);
void do_barrier_wait(barrier_t *);

#endif