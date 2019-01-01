#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#include "type.h"

#define O_RDONLY    00000000
#define O_WRONLY    00000001
#define O_RDWR      00000002


int do_fopen(char *name, uint32_t mode);
void do_fwrite(int fd, char *content, int length);
void do_fread(int fd, char *buffer, int length);
void do_fclose(int fd);
void do_fexit();

#endif