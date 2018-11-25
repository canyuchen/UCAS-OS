#ifndef INCLUDE_MAIL_BOX_
#define INCLUDE_MAIL_BOX_

#include "sem.h"
#include "lock.h"

#define MAX_MBOX_LENGTH 32
#define MAX_MESSAGE_LENGTH 32
#define MBOX_NAME_LENGTH 32

typedef struct message{
	/* TODO */
    char value[MAX_MESSAGE_LENGTH];
} message_t;

typedef struct mailbox{
    char name[MBOX_NAME_LENGTH];
    int count;
    int ptr;
    mutex_lock_t lock;
    semaphore_t send, recv;
    message_t msg[MAX_MBOX_LENGTH];
} mailbox_t;

void mbox_init();
mailbox_t *mbox_open(char *);
void mbox_close(mailbox_t *);
void mbox_send(mailbox_t *, void *, int);
void mbox_recv(mailbox_t *, void *, int);

#endif