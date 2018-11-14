#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"

pcb_t pcb[NUM_MAX_TASK];

/* global process id */
pid_t process_id = 1;

static uint32_t get_queue_head_daedline(queue_t *queue)
{
    return ((pcb_t *)(queue->head))->sleeping_deadline;
}

/* TODO:wake up sleeping processes whose deadlines have passed */
/*
static void check_sleeping()
{
    //CLOSE_INTERRUPT;
    uint32_t current_time = get_timer();
    //  printf(12, 10, "current time: %d", current_time);
    pcb_t* temp;
    // while(!queue_is_empty(&sleeping_queue) \
    //   && (((pcb_t*) queue_dequeue(&sleeping_queue))-> sleeping_deadline <= current_time)) {
    //     temp = queue_dequeue(&sleeping_queue);
    //     queue_push(&ready_queue, temp);
    //     temp->status = TASK_READY;
    // }
    while(!queue_is_empty(&sleeping_queue) \
      && (((pcb_t *)(sleeping_queue.head))->sleeping_deadline <= current_time)) {
      //&& (get_queue_head_daedline(&sleeping_queue) <= current_time)) {
        temp = queue_dequeue(&sleeping_queue);
        temp->status = TASK_READY;        
        queue_push(&ready_queue, temp);
    }
}
*/

static void check_sleeping()
{
    uint32_t current_time = get_timer();
    if(queue_is_empty(&sleeping_queue)) return;
    pcb_t* temp = (pcb_t *)sleeping_queue.head;
    while(temp->next != NULL){
        if(temp->sleeping_deadline < current_time){
            queue_remove(&sleeping_queue, temp);            
            temp->status = TASK_READY;
            queue_push(&ready_queue, temp);
        }
        temp = (pcb_t *)temp->next;
    }
}

static int deadline_comp(void *item_1, void *item_2)
{
    pcb_t *_item_1 = (pcb_t *)item_1;
    pcb_t *_item_2 = (pcb_t *)item_2;
    return (_item_2->sleeping_deadline > _item_1->sleeping_deadline) ? 1 : 0 ;
}

/* Change current_running to the next task */
void scheduler(void)
{

    current_running->cursor_x = screen_cursor_x;
    current_running->cursor_y = screen_cursor_y;

    //CLOSE_INTERRUPT;
    check_sleeping(); // wake up sleeping processes
    while (queue_is_empty(&ready_queue)){
        //BUG P3!!!!!
        if(queue_is_empty(&sleeping_queue)){
            break;
        }
        check_sleeping();
    }

    // TODO schedule
    // Modify the current_running pointer.
    // if(current_running->status != TASK_BLOCKED \
    // || current_running->status != TASK_SLEEPING){
    if(current_running->status == TASK_RUNNING){
        current_running->status = TASK_READY;
        if(current_running->entry_point != 0){
            //initial pcb does not need to be pushed to ready_queue
            queue_push(&ready_queue, current_running);           
        }        
    }

    pcb_t *_current_running = ((pcb_t *)(ready_queue.head));
    while(_current_running != ((pcb_t *)(ready_queue.tail)) \
        && _current_running->priority < ((pcb_t *)(_current_running->next))->priority){
        _current_running = ((pcb_t *)(_current_running->next));
    }

/*
    if(_current_running->priority <= 0){
        _current_running = ((pcb_t *)(ready_queue.head));
        while(_current_running != ((pcb_t *)(ready_queue.tail))){
            _current_running->priority = INITIAL_PRIORITY;
             _current_running = ((pcb_t *)(_current_running->next));
        }
        _current_running->priority = INITIAL_PRIORITY;
        current_running = queue_dequeue(&ready_queue);
    }
    else{
        current_running = _current_running;        
        queue_remove(&ready_queue, _current_running);
    }
*/

    current_running = _current_running;        
    queue_remove(&ready_queue, _current_running);

    //now_priority[current_running->pid]--;
    current_running->priority--;

    //current_running = queue_dequeue(&ready_queue);
    current_running->status = TASK_RUNNING;

    screen_cursor_x = current_running->cursor_x;
    screen_cursor_y = current_running->cursor_y;
}

void do_sleep(uint32_t sleep_time)
{
    if(current_running->status == TASK_RUNNING){
        current_running->status = TASK_SLEEPING;
        current_running->sleeping_deadline = get_timer() + sleep_time;
        // queue_sort(&sleeping_queue, current_running, deadline_comp);
        queue_push(&sleeping_queue, current_running);

    //    do_scheduler();        
    }
        do_scheduler(); 
}

static int priority_comp(void *item_1, void *item_2)
{
    pcb_t *_item_1 = (pcb_t *)item_1;
    pcb_t *_item_2 = (pcb_t *)item_2;
    return (_item_2->priority > _item_1->priority) ? 1 : 0 ;
}

void do_block(queue_t *queue_ptr)
{
    // block the current_running task into the queue
    if(current_running->status == TASK_RUNNING){
        current_running->status = TASK_BLOCKED;
        //REVISED: 
        //queue_push(&block_queue, current_running);
        queue_push(queue_ptr, current_running);
        //queue_sort(queue_ptr, current_running, priority_comp);
    //    do_scheduler();
    }
        do_scheduler();
}

/*
void do_unblock_one(queue_t *queue_ptr)
{
    //CLOSE_INTERRUPT;
    // unblock the head task from the queue
    pcb_t *block_queue_head_ptr;

    // if(!queue_is_empty(&block_queue)){
    if(!queue_is_empty(queue_ptr)){
        //REVISED: 
        //block_queue_head_ptr = queue_dequeue(&block_queue);
        block_queue_head_ptr = queue_dequeue(queue_ptr);
        block_queue_head_ptr->status = TASK_READY;
        queue_push(&ready_queue, block_queue_head_ptr);
    }
}
*/

void do_unblock_one(queue_t *queue_ptr)
{
    //CLOSE_INTERRUPT;
    // unblock the head task from the queue
    pcb_t *unblock_one_ptr;

    if(!queue_is_empty(queue_ptr)){
        pcb_t *_unblock_one_ptr = ((pcb_t *)(queue_ptr->head));
        while(_unblock_one_ptr != ((pcb_t *)(queue_ptr->tail)) \
            && _unblock_one_ptr->priority < ((pcb_t *)(_unblock_one_ptr->next))->priority){
            _unblock_one_ptr = ((pcb_t *)(_unblock_one_ptr->next));
        }
        unblock_one_ptr = _unblock_one_ptr;
        queue_remove(queue_ptr, _unblock_one_ptr);
        unblock_one_ptr->status = TASK_READY;
        queue_push(&ready_queue, unblock_one_ptr);
    }
}

void do_unblock_all(queue_t *queue)
{
    //CLOSE_INTERRUPT;
    // unblock all task in the queue
    while(!queue_is_empty(&block_queue)){
        do_unblock_one(&block_queue);
    }
}

void do_ps()
{
    int i = 1;
    pcb_t *head = ((pcb_t *)(ready_queue.head));

    // printk("\n[PROCESS TABLE]\n");
    // // screen_cursor_add(0, 2);
    // printk("[0] PID : %d STATUS : TASK_RUNNING\n", current_running->pid);
    ProcessShow[0].num = 0;
    ProcessShow[0].pid = current_running->pid;
    ProcessShow[0].status = current_running->status;
    while(head != ((pcb_t *)(ready_queue.tail))){
        // printk("[%d] PID : %d STATUS : TASK_READY\n", i, head->pid);
        // screen_cursor_add(0,1);
        ProcessShow[i].num = i;
        ProcessShow[i].pid = head->pid;
        ProcessShow[i].status = head->status;
        head = ((pcb_t *)(head->next));
        i++;
    }
    ProcessShow[i].num = -1;
    // printk("> root@UCAS_OS: ");
    // screen_cursor_add(16,2);
}

void do_spawn(task_info_t *task_info)
{

}

void do_exit()
{

}

int  do_getpid()
{

}

void do_waitpid(int n)
{

}

void do_kill(int n)
{

}


