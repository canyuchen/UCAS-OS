#include "irq.h"
#include "time.h"
#include "sched.h"
#include "string.h"
#include "queue.h"

static void irq_timer()
{
    // TODO clock interrupt handler.
    // scheduler, time counter in here to do, emmmmmm maybe.
    
}

void interrupt_helper(uint32_t status, uint32_t cause)
{
    // TODO interrupt handler.
    // Leve3 exception Handler.
    // read CP0 register to analyze the type of interrupt.
    
}

void other_exception_handler()
{
    // TODO other exception handler

}

/* Round-robin scheduling: Save current_running before preempting */
void put_current_running(){
    //enqueue(&ready_queue, current_running);
    queue_push(&ready_queue, current_running);
    current_running->status = TASK_READY;
}