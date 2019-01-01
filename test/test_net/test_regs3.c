#include "mac.h"
#include "irq.h"
#include "type.h"
#include "screen.h"
#include "syscall.h"
#include "sched.h"
#include "test5.h"

#ifdef TEST_NET_3

desc_t *send_desc;
desc_t *receive_desc;
uint32_t cnt = 1; //record the time of iqr_mac
//uint32_t buffer[PSIZE] = {0x00040045, 0x00000100, 0x5d911120, 
//                          0x0101a8c0, 0xfb0000e0, 0xe914e914, 
//                          0x00000801, 0x45000400, 0x00010000, 
//                          0x2011915d, 0xc0a80101, 0xe00000fb, 
//                          0x14e914e9, 0x01080000};
uint32_t buffer[PSIZE] = {0xffffffff, 0x5500ffff, 0xf77db57d, 
                          0x00450008, 0x0000d400, 0x11ff0040, 
                          0xa8c073d8, 0x00e00101, 0xe914fb00, 
                          0x0004e914, 0x00000000, 0x005e0001, 
                          0x2300fb00, 0x84b7f28b, 0x00450008, 
                          0x0000d400, 0x11ff0040, 0xa8c073d8, 
                          0x00e00101, 0xe914fb00, 0x0801e914, 
                          0x00000000};

uint32_t recv_buffer[RECV_BUFFER_SIZE] = {0x00000000};

queue_t recv_block_queue;
uint32_t recv_flag[PNUM];
uint32_t ch_flag;

/**
 * Clears all the pending interrupts.
 * If the Dma status register is read then all the interrupts gets cleared
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void clear_interrupt()
{
    uint32_t data;
    data = reg_read_32(0xbfe11000 + DmaStatus);
    reg_write_32(0xbfe11000 + DmaStatus, data);
}

static void send_desc_init(mac_t *mac)
{
    do_send_desc_init(send_desc_table_ptr, buffer, PSIZE*sizeof(uint32_t), PNUM);
}

static void recv_desc_init(mac_t *mac)
{
    do_recv_desc_init(recv_desc_table_ptr, recv_buffer, PSIZE*sizeof(uint32_t), PNUM);
}


static void mii_dul_force(mac_t *mac)
{
    reg_write_32(mac->dma_addr, 0x80); //?s
                                       //   reg_write_32(mac->dma_addr, 0x400);
    uint32_t conf = 0xc800;            //0x0080cc00;

    // loopback, 100M
    reg_write_32(mac->mac_addr, reg_read_32(mac->mac_addr) | (conf) | (1 << 8));
    //enable recieve all
    reg_write_32(mac->mac_addr + 0x4, reg_read_32(mac->mac_addr + 0x4) | 0x80000001);
}

static void start_tran(mac_t *mac)
{

   
}

void dma_control_init(mac_t *mac, uint32_t init_value)
{
    reg_write_32(mac->dma_addr + DmaControl, init_value);
    return;
}

void phy_regs_task1()
{

    mac_t test_mac;
    uint32_t i;
    uint32_t print_location = 3;

    test_mac.mac_addr = 0xbfe10000;
    test_mac.dma_addr = 0xbfe11000;

    test_mac.psize = PSIZE * 4; // 64bytes
    test_mac.pnum = PNUM;       // pnum

    test_mac.td_phy = PHYADDR((uint32_t)send_desc_table_ptr);
    test_mac.td = (uint32_t)send_desc_table_ptr;
    test_mac.saddr = (uint32_t)buffer;
    test_mac.saddr_phy = PHYADDR((uint32_t)buffer);
    
    send_desc_init(&test_mac);

    dma_control_init(&test_mac, DmaStoreAndForward | DmaTxSecondFrame | DmaRxThreshCtrl128);
    clear_interrupt(&test_mac);

    mii_dul_force(&test_mac);

    register_irq_handler(LS1C_MAC_IRQ, irq_mac);

    irq_enable(LS1C_MAC_IRQ);
    sys_move_cursor(1, print_location);
    printf("> [SEND TASK] start send package.               \n");

    uint32_t cnt = 0;
    i = 4;
    while (i > 0)
    {
        sys_net_send(test_mac.td, test_mac.td_phy);
        cnt += PNUM;
        sys_move_cursor(1, print_location+(cnt/PNUM));
        printf("> [SEND TASK] now totally send package %d !        \n", cnt);
        i--;
    }

    sys_move_cursor(1, print_location+5);
    printf("> [SEND TASK] send task finished        \n", cnt);

    sys_exit();
}

void phy_regs_task2()
{

    mac_t test_mac;
    uint32_t i;
    uint32_t ret;
    uint32_t print_location = 9;

    test_mac.mac_addr = 0xbfe10000;
    test_mac.dma_addr = 0xbfe11000;

    test_mac.psize = PSIZE * 4; // 64bytes
    test_mac.pnum = PNUM;       // pnum

    test_mac.rd_phy = PHYADDR((uint32_t)recv_desc_table_ptr);
    test_mac.rd = (uint32_t)recv_desc_table_ptr;
    test_mac.daddr = (uint32_t)recv_buffer;
    test_mac.daddr_phy = PHYADDR((uint32_t)recv_buffer);
    test_mac.saddr = (uint32_t)buffer;

    recv_desc_init(&test_mac);

    dma_control_init(&test_mac, DmaStoreAndForward | DmaTxSecondFrame | DmaRxThreshCtrl128);
    clear_interrupt(&test_mac);

    mii_dul_force(&test_mac);

    queue_init(&recv_block_queue);
    sys_move_cursor(1, print_location);
    printf("> [RECV TASK] start recv:                    ");
    ret = sys_net_recv(test_mac.rd, test_mac.rd_phy, test_mac.daddr);
  
    ch_flag = 0;
    for (i = 0; i < PNUM; i++)
    {
        recv_flag[i] = 0;
    }

    uint32_t cnt = 0;
    uint32_t *Recv_desc;
    Recv_desc = (uint32_t *)(test_mac.rd + (PNUM - 1) * 16);
    //printf("(test_mac.rd 0x%x ,Recv_desc=0x%x,REDS0 0X%x\n", test_mac.rd, Recv_desc, *(Recv_desc));
    if (((*Recv_desc) & 0x80000000) == 0x80000000)
    {
        sys_move_cursor(1, print_location+1);
        printf("> [RECV TASK] waiting receive package.\n");
        sys_wait_recv_package();
    }

    check_recv(&test_mac);
    sys_move_cursor(1, print_location+2);
    printf("> [RECV TASK] 64 packages received, now exit.\n");

    sys_exit();
}

void phy_regs_task3()
{
    uint32_t print_location = 1;

    send_desc = (desc_t*)send_desc_table_ptr;
    receive_desc = (desc_t*)recv_desc_table_ptr;
    // recv_buffer = BIG_RECEIVE_BUFFER;

    bzero(recv_buffer, RECV_BUFFER_SIZE*sizeof(uint32_t));

    sys_move_cursor(1, print_location);
    printf("> [INIT] Waiting for MAC initialization .\n");

    sys_init_mac();

    sys_move_cursor(1, print_location+1);
    printf("> [INIT] MAC initialization succeeded.           \n");
    sys_exit();
}

//------------------------BONUS-------------------------

static void recv_desc_bonus_init(mac_t *mac)
{
    do_recv_desc_init(recv_desc_table_ptr, (uint32_t)recv_buffer, (PSIZE*sizeof(uint32_t)), PNUM*4);
}

void phy_regs_task_bonus()
{
    mac_t test_mac;
    uint32_t i;
    uint32_t ret;
    uint32_t print_location = 8;

    test_mac.mac_addr = 0xbfe10000;
    test_mac.dma_addr = 0xbfe11000;

    test_mac.psize = PSIZE * 4; // 64bytes
    test_mac.pnum = PNUM;       // pnum

    test_mac.rd_phy = PHYADDR((uint32_t)recv_desc_table_ptr);
    test_mac.rd = (uint32_t)recv_desc_table_ptr;
    test_mac.daddr = (uint32_t)recv_buffer;
    test_mac.daddr_phy = PHYADDR((uint32_t)recv_buffer);
    test_mac.saddr = (uint32_t)buffer;

    recv_desc_bonus_init(&test_mac);

    dma_control_init(&test_mac, DmaStoreAndForward | DmaTxSecondFrame | DmaRxThreshCtrl128);
    clear_interrupt(&test_mac);

    mii_dul_force(&test_mac);

    queue_init(&recv_block_queue);
    sys_move_cursor(1, print_location);
    printf("> [RECV TASK] start recv:                    ");

    int mcnt=0;
    // uint32_t start_time=time_elapsed;
    // send pkg for 8s
    while(mcnt<32)
    {
        // sys_move_cursor(1,print_location);
        // int time_passed=(time_elapsed-start_time);//to be fixed
        // printf("> pkg:%d\n", cnt);
        // printf("> pst:%d\n", time_passed);
        // printf("> spd:%d KBps", cnt*(300000000/150000)/(time_passed));
        // printf("> spd:%d KBps", cnt*(300000000/150000)/(time_passed));
    //     recv_desc_init(&test_mac);
        ret = sys_net_fast_recv(test_mac.rd, test_mac.rd_phy, test_mac.daddr);
        sys_wait_recv_package();
        mcnt++;
    }

    sys_move_cursor(1, print_location+1);
    printf("> [RECV TASK] 2**7 pkg received.\n");
  
    sys_exit();
}

#endif
