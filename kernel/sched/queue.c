#include "queue.h"
#include "sched.h"

typedef pcb_t item_t;

void queue_init(queue_t *queue)
{
    queue->head = queue->tail = NULL;
}

int queue_is_empty(queue_t *queue)
{
    if (queue->head == NULL)
    {
        return 1;
    }
    return 0;
}

void queue_push(queue_t *queue, void *item)
{
    item_t *_item = (item_t *)item;
    /* queue is empty */
    if (queue->head == NULL)
    {
        queue->head = item;
        queue->tail = item;
        _item->next = NULL;
        _item->prev = NULL;
    }
    else
    {
        ((item_t *)(queue->tail))->next = item;
        _item->next = NULL;
        _item->prev = queue->tail;
        queue->tail = item;
    }
}

void *queue_dequeue(queue_t *queue)
{
    item_t *temp = (item_t *)queue->head;

    /* this queue only has one item */
    if (temp->next == NULL)
    {
        queue->head = queue->tail = NULL;
    }
    else
    {
        queue->head = ((item_t *)(queue->head))->next;
        ((item_t *)(queue->head))->prev = NULL;
    }

    temp->prev = NULL;
    temp->next = NULL;

    return (void *)temp;
}

int check_in_queue(queue_t *queue, void *item)
{
    item_t *head = queue->head;
    item_t *tail = queue->tail;
    while(head != NULL){
        if(head == (item_t *)item){
            return 1;
        }
        head = head->next;
    } 
    return 0;
}

/* remove this item and return next item */
void *queue_remove(queue_t *queue, void *item)
{
    item_t *_item = (item_t *)item;
    item_t *next = (item_t *)_item->next;

    if (item == queue->head && item == queue->tail)
    {
        queue->head = NULL;
        queue->tail = NULL;
    }
    else if (item == queue->head)
    {
        queue->head = _item->next;
        ((item_t *)(queue->head))->prev = NULL;
    }
    else if (item == queue->tail)
    {
        queue->tail = _item->prev;
        ((item_t *)(queue->tail))->next = NULL;
    }
    else
    {
        ((item_t *)(_item->prev))->next = _item->next;
       //((item_t *)(pcb->next))->prev = _item->prev;
        ((item_t *)(_item->next))->prev = _item->prev;
    }

    _item->prev = NULL;
    _item->next = NULL;

    return (void *)next;
}

void enqueue(queue_t *queue, void *item)
{
    item_t *_item = (item_t *)item;
    /* queue is empty */
    if (queue->head == NULL)
    {
        queue->head = item;
        queue->tail = item;
        _item->next = NULL;
        _item->prev = NULL;
    }
    else
    {
        ((item_t *)(queue->head))->prev = item;
        _item->prev = NULL;
        _item->next = queue->head;
        queue->head = item;
    }
}
/*
void queue_sort(queue_t *queue, void *item, item_comp_t item_comp)
{
    item_t *_item = (item_t *)item;
    item_t *item_next = NULL;
    item_t *item_prev = NULL;

    if(queue_is_empty(queue)){
        queue->head = item;
        queue->tail = item;
        _item->next = NULL;
        _item->prev = NULL;
    }
    else{
        void *head = queue->head;
        while(((item_t *)(queue->head))->next != NULL \
            // && item_comp(queue,item) == 1){
            // && item_comp(((item_t *)(queue->head)),item) == 1){
            // && item_comp(((item_t *)(queue->head)),item) == 0){
             && item_comp(item, ((item_t *)(queue->head))) == 1){
                queue->head = ((item_t *)(queue->head))->next;
            }
        if(((item_t *)(queue->head))->next == NULL 
            && item_comp(((item_t *)(queue->head)),item) == 0){
            ((item_t *)(queue->tail))->next = item;
            // ((item_t *)(queue->head))->next == item;
            _item->next = NULL;
            _item->prev = queue->tail;
            queue->tail = item;

            queue->head = head;
        }
        else if(head == queue->head
                && item_comp(((item_t *)(queue->head)),item) == 1) {
            _item->prev = NULL;
            _item->next = queue->head;
            ((item_t *)(queue->head))->next = item;
            queue->head = item;
        }
        else{
            item_next = ((item_t *)(queue->head))->next;
            item_prev = ((item_t *)(queue->head));

            item_prev->next = item;
            _item->prev = item_prev;
            item_next->prev = item;
            _item->next = item_next;

            queue->head = head;
        }
        
    }
}
*/