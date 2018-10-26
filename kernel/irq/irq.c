#include "irq.h"
#include "time.h"
#include "sched.h"
#include "string.h"
#include "queue.h"

extern void irq_timer();

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
