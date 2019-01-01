#ifndef INCLUDE_TEST4_H_
#define INCLUDE_TEST4_H_
#include "queue.h"

// extern queue_t recv_block_queue;
// #define EPT_ARP 0x0608 /* type: ARP */

// #define PSIZE (256)
// #define PNUM (64)
// #define SEND_BUFFER (0xa1200000)
// #define RECV_BUFFER ((SEND_BUFFER) + (PNUM) *( PSIZE))
// #define SEND_DESC ((RECV_BUFFER) + (PSIZE + 16) * PNUM)
// #define RECV_DESC (SEND_DESC + (16) * PNUM)
// #define LAST_RECV_DESC (RECV_DESC+(PNUM-1)*16)

// static void init_data(uint32_t *addr);
//void phy_regs_task(void);
// #if 1
void phy_regs_task1(void);
void phy_regs_task2(void);
void phy_regs_task3(void);
// #endif
// static void init_mac(void);
//extern uint32_t recv_flag[PNUM];
//extern uint32_t ch_flag ;

// #define TEST_NET_1
#define TEST_NET_3

#endif