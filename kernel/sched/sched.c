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

static int check_sleeping()
{
    uint32_t current_time = get_timer();
    if(queue_is_empty(&sleeping_queue)) return;
    pcb_t* temp = (pcb_t *)(sleeping_queue.head);

    while(temp != NULL){
        if(temp->sleeping_deadline < current_time){
            queue_remove(&sleeping_queue, temp);            
            temp->status = TASK_READY;
            queue_push(&ready_queue, temp);
            return 1;
        }
        temp = (pcb_t *)(temp->next);
    }
    return 0;
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

    if(current_running->status == TASK_RUNNING){
        current_running->status = TASK_READY;
        if(current_running->entry_point != 0){
            //initial pcb does not need to be pushed to ready_queue
            queue_push(&ready_queue, current_running);           
        }        
    }

    check_sleeping(); // wake up sleeping processes
    while (queue_is_empty(&ready_queue)){
        if(queue_is_empty(&sleeping_queue)){
            break;
        }
        check_sleeping();
    }

    pcb_t *_current_running = ((pcb_t *)(ready_queue.head));
    while(_current_running != ((pcb_t *)(ready_queue.tail)) \
        && _current_running->priority < ((pcb_t *)(_current_running->next))->priority){
        _current_running = ((pcb_t *)(_current_running->next));
    }

    current_running->priority--;
    current_running = _current_running;        
    queue_remove(&ready_queue, _current_running);

    current_running->status = TASK_RUNNING;

    screen_cursor_x = current_running->cursor_x;
    screen_cursor_y = current_running->cursor_y;
}

void do_sleep(uint32_t sleep_time)
{
    if(current_running->status == TASK_RUNNING){
        current_running->status = TASK_SLEEPING;
        current_running->sleeping_deadline = get_timer() + sleep_time;
        queue_push(&sleeping_queue, current_running);      
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
        queue_push(queue_ptr, current_running);
    }
        do_scheduler();
}

void do_unblock_one(queue_t *queue_ptr)
{
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
    while(!queue_is_empty(queue)){
        do_unblock_one(queue);
    }
}

void do_ps()
{
    int i = 1, j = 0;
    pcb_t *head = ((pcb_t *)(ready_queue.head));

    ProcessShow[0].num = 0;
    ProcessShow[0].pid = current_running->pid;
    ProcessShow[0].status = current_running->status;

    for(j = 0; j < NUM_MAX_TASK; j++){
        if(pcb[j].status == TASK_READY){
            ProcessShow[i].num = i;
            ProcessShow[i].pid = pcb[j].pid;
            ProcessShow[i].status = pcb[j].status;
            i++;
        }
    }
    for(j = 0; j < NUM_MAX_TASK; j++){
        if(pcb[j].status == TASK_SLEEPING){
            ProcessShow[i].num = i;
            ProcessShow[i].pid = pcb[j].pid;
            ProcessShow[i].status = pcb[j].status;
            i++;
        }
    }
    for(j = 0; j < NUM_MAX_TASK; j++){
        if(pcb[j].status == TASK_BLOCKED){
            ProcessShow[i].num = i;
            ProcessShow[i].pid = pcb[j].pid;
            ProcessShow[i].status = pcb[j].status;
            i++;
        }
    }
    ProcessShow[i].num = -1;
}

void do_spawn(task_info_t *task_info)
{
    //TO_IMPROVE
    int i = 0;
    while(pcb[i].status != TASK_EXITED){
        i++;
        if(i >= NUM_MAX_TASK){
            //ERROR handle
            return;
        }
    }

    queue_init(&(pcb[i].waiting_queue));

	bzero(&(pcb[i].kernel_context), sizeof(pcb[i].kernel_context));
	bzero(&(pcb[i].user_context  ), sizeof(pcb[i].user_context  ));
	int j = 0;
	for(; j < LOCK_MAX_NUM; j++){
		pcb[i].lock[j] = NULL;
	}
    pcb[i].kernel_context.regs[29] = STACK_TOP;
	pcb[i].user_context.regs[29] = STACK_TOP + STACK_SIZE;
	pcb[i].kernel_context.regs[30] = STACK_TOP;
	pcb[i].user_context.regs[30] = STACK_TOP + STACK_SIZE;
	pcb[i].kernel_stack_top = STACK_TOP;
	pcb[i].user_stack_top = STACK_TOP + STACK_SIZE;		
	STACK_TOP += STACK_SIZE*2;
	if(STACK_TOP > STACK_MAX)
	{
		//TODO
	}

	pcb[i].prev = NULL;
	pcb[i].next = NULL;
	pcb[i].pid = PID;
	pcb[i].type = task_info->type;
	pcb[i].status = TASK_CREATED;
	pcb[i].cursor_x = 0;
	pcb[i].cursor_y = 0;

	pcb[i].entry_point = task_info->entry_point;

	pcb[i].kernel_context.regs[31] = (uint32_t)first_entry;

	pcb[i].kernel_context.cp0_status = CP0_STATUS_INIT;
	pcb[i].user_context.cp0_status = CP0_STATUS_INIT;

	pcb[i].kernel_context.cp0_epc = pcb[i].entry_point;
	//???
	pcb[i].user_context.cp0_epc = pcb[i].entry_point;
	//cp0_epc add 4 automatically when encountering interrupt

	pcb[i].mode = USER_MODE;
	pcb[i].priority = INITIAL_PRIORITY;
	pcb[i].wait_time = 0;
	pcb[i].sleeping_deadline = 0;
    pcb[i].lock_num = 0;

    PID++;
	queue_push(&ready_queue,&pcb[i]); 
}

void do_exit()
{
    int i = 0;
    for(;i<LOCK_MAX_NUM;i++){
        if(current_running->lock[i] != 0){
            do_mutex_lock_release(current_running->lock[i]);
        }
    }

    while(!queue_is_empty(&(current_running->waiting_queue))){
        pcb_t *head = queue_dequeue(&(current_running->waiting_queue));
        head->status = TASK_READY;
        queue_push(&ready_queue, head);
    }

    current_running->status = TASK_EXITED;
    do_scheduler();
}

int  do_getpid()
{
    return current_running->pid;
}

void do_waitpid(int n)
{
    int i = 0;
    while(pcb[i].pid != n){
        i++;
        if(i >= NUM_MAX_TASK){
            //ERROR
            return;
        }
    }
    if(pcb[i].status != TASK_EXITED){
        current_running->status = TASK_BLOCKED;
        queue_push(&(pcb[i].waiting_queue), current_running);
        do_scheduler();
    }
}

void do_kill(int n)
{
    int i = 0;
    while(pcb[i].pid != n){
        i++;
        if(i >= NUM_MAX_TASK){
            //ERROR
            return;
        }
    }
    if(pcb[i].status != TASK_EXITED){
        if(pcb[i].status == TASK_READY){
            queue_remove(&ready_queue, &pcb[i]);
        }
        else if(pcb[i].status == TASK_BLOCKED){
            int k = 0;
            for(;k<MAX_LOCK_NUM_TOTAL;k++){
                if(check_in_queue(&(Lock[k]->mutex_lock_queue), &pcb[i])){
                    queue_remove(&(Lock[k]->mutex_lock_queue), &pcb[i]);
                }
            }
        }
        else if(pcb[i].status == TASK_SLEEPING){
            queue_remove(&sleeping_queue, &pcb[i]);
        }
        else if(pcb[i].status == TASK_CREATED){     
            queue_remove(&ready_queue, &pcb[i]);     
        }

        pcb[i].status = TASK_EXITED;

        int j = 0;
        for(;j<LOCK_MAX_NUM;j++){
            if(pcb[i].lock[j] != 0){
                do_mutex_lock_release(pcb[i].lock[j]);
            }
        }
        clear_waiting_queue(&(pcb[i].waiting_queue));
    }

    if(current_running->pid == n){
        do_scheduler();
    }
}

uint32_t get_pcb_index(int pid)
{
    int i = 0;
    while(i < NUM_MAX_TASK && pcb[i].pid != pid){
        i++;
    }
    return i;
}
