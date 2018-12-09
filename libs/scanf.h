#ifndef INCLUDE_SCANF_
#define INCLUDE_SCANF_

#include "string.h"

// extern char buffer[100];
// extern int inline_position;
typedef struct InputBuffer {
    char *buffer;
    int32_t buffer_length;
    int32_t input_length;
    int pointer;
} InputBuffer_s_t;

extern InputBuffer_s_t inputBuffer_s;

extern InputBuffer_s_t *inputBuffer_s_ptr;

int do_scanf(int *mem);

void init_InputBuffer_s(InputBuffer_s_t *p);

// extern int index;

#endif