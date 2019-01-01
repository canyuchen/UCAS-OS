#include "mac.h"
#include "regs.h"
#include "irq.h"
#include "stdio.h"

desc_t *send_desc_table_ptr;
desc_t *recv_desc_table_ptr;

desc_t send_desc_table[PNUM];
desc_t recv_desc_table[PNUM];

uint32_t recv_buffer_ptr = 0;

uint32_t reg_read_32(uint32_t addr)
{
    return *((uint32_t *)addr);
}
uint32_t read_register(uint32_t base, uint32_t offset)
{
    uint32_t addr = base + offset;
    uint32_t data;

    data = *(volatile uint32_t *)addr;
    return data;
}

void reg_write_32(uint32_t addr, uint32_t data)
{
    *((uint32_t *)addr) = data;
}

static void gmac_get_mac_addr(uint8_t *mac_addr)
{
    uint32_t addr;

    addr = read_register(GMAC_BASE_ADDR, GmacAddr0Low);
    mac_addr[0] = (addr >> 0) & 0x000000FF;
    mac_addr[1] = (addr >> 8) & 0x000000FF;
    mac_addr[2] = (addr >> 16) & 0x000000FF;
    mac_addr[3] = (addr >> 24) & 0x000000FF;

    addr = read_register(GMAC_BASE_ADDR, GmacAddr0High);
    mac_addr[4] = (addr >> 0) & 0x000000FF;
    mac_addr[5] = (addr >> 8) & 0x000000FF;
}

/* print DMA regs */
void print_dma_regs()
{
    uint32_t regs_val1, regs_val2;

    printk(">>[DMA Register]\n");

    // [0] Bus Mode Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaBusMode);

    printk("  [0] Bus Mode : 0x%x, ", regs_val1);

    // [3-4] RX/TX List Address Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaRxBaseAddr);
    regs_val2 = read_register(DMA_BASE_ADDR, DmaTxBaseAddr);
    printk("  [3-4] TX/RX : 0x%x/0x%x\n", regs_val2, regs_val1);

    // [5] Status Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaStatus);
    printk("  [5] Status : 0x%x, ", regs_val1);

    // [6] Operation Mode Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaControl);
    printk("  [6] Control : 0x%x\n", regs_val1);

    // [7] Interrupt Enable Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaInterrupt);
    printk("  [7] Interrupt : 0x%x, ", regs_val1);

    // [8] Miss
    regs_val1 = read_register(DMA_BASE_ADDR, DmaMissedFr);
    printk("  [8] Missed : 0x%x\n", regs_val1);

    // [18-19] Current Host TX/RX Description Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaTxCurrDesc);
    regs_val2 = read_register(DMA_BASE_ADDR, DmaRxCurrDesc);
    printk("  [18-19] Current Host TX/RX Description : 0x%x/0x%x\n", regs_val1, regs_val2);

    // [20-21] Current Host TX/RX Description Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaTxCurrAddr);
    regs_val2 = read_register(DMA_BASE_ADDR, DmaRxCurrAddr);
    printk("  [20-21] Current Host TX/RX Buffer Address : 0x%x/0x%x\n", regs_val1, regs_val2);
}

/* print DMA regs */
void print_mac_regs()
{
    printk(">>[MAC Register]\n");
    uint32_t regs_val1, regs_val2;
    uint8_t mac_addr[6];

    // [0] MAC Configure Register
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacConfig);
    printk("  [0] Configure : 0x%x, ", regs_val1);

    // [1] MAC Frame Filter
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacFrameFilter);
    printk("  [1] Frame Filter : 0x%x\n", regs_val1);

    // [2-3] Hash Table High/Low Register
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacHashHigh);
    regs_val2 = read_register(GMAC_BASE_ADDR, GmacHashLow);
    printk("  [2-3] Hash Table High/Low : 0x%x-0x%x\n", regs_val1, regs_val2);

    // [6] Flow Control Register
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacFlowControl);
    printk("  [6] Flow Control : 0x%x, ", regs_val1);

    // [8] Version Register
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacVersion);
    printk("  [8] Version : 0x%x\n", regs_val1);

    // [14] Interrupt Status Register and Interrupt Mask
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacInterruptStatus);
    regs_val2 = read_register(GMAC_BASE_ADDR, GmacInterruptMask);
    printk("  [14-15] Interrupt Status/Mask : 0x%x/0x%x\n", regs_val1, regs_val2);

    // MAC address
    gmac_get_mac_addr(mac_addr);
    printk("  [16-17] Mac Addr : %X:%X:%X:%X:%X:%X\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

void printf_dma_regs()
{
    uint32_t regs_val1, regs_val2;

    printf(">>[DMA Register]\n");

    // [0] Bus Mode Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaBusMode);

    printf("  [0] Bus Mode : 0x%x, ", regs_val1);

    // [3-4] RX/TX List Address Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaRxBaseAddr);
    regs_val2 = read_register(DMA_BASE_ADDR, DmaTxBaseAddr);
    printf("  [3-4] RX/TX : 0x%x/0x%x\n", regs_val1, regs_val2);

    // [5] Status Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaStatus);
    printf("  [5] Status : 0x%x, ", regs_val1);

    // [6] Operation Mode Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaControl);
    printf("  [6] Control : 0x%x\n", regs_val1);

    // [7] Interrupt Enable Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaInterrupt);
    printf("  [7] Interrupt : 0x%x, ", regs_val1);

    // [8] Miss
    regs_val1 = read_register(DMA_BASE_ADDR, DmaMissedFr);
    printf("  [8] Missed : 0x%x\n", regs_val1);

    // [18-19] Current Host TX/RX Description Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaTxCurrDesc);
    regs_val2 = read_register(DMA_BASE_ADDR, DmaRxCurrDesc);
    printf("  [18-19] Current Host TX/RX Description : 0x%x/0x%x\n", regs_val1, regs_val2);

    // [20-21] Current Host TX/RX Description Register
    regs_val1 = read_register(DMA_BASE_ADDR, DmaTxCurrAddr);
    regs_val2 = read_register(DMA_BASE_ADDR, DmaRxCurrAddr);
    printk("  [20-21] Current Host TX/RX Buffer Address : 0x%x/0x%x\n", regs_val1, regs_val2);
}

/* print DMA regs */
void printf_mac_regs()
{
    printf(">>[MAC Register]\n");
    uint32_t regs_val1, regs_val2;
    uint8_t mac_addr[6];

    // [0] MAC Configure Register
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacConfig);
    printf("  [0] Configure : 0x%x, ", regs_val1);

    // [1] MAC Frame Filter
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacFrameFilter);
    printf("  [1] Frame Filter : 0x%x\n", regs_val1);

    // [2-3] Hash Table High/Low Register
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacHashHigh);
    regs_val2 = read_register(GMAC_BASE_ADDR, GmacHashLow);
    printf("  [2-3] Hash Table High/Low : 0x%x-0x%x\n", regs_val1, regs_val2);

    // [6] Flow Control Register
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacFlowControl);
    printf("  [6] Flow Control : 0x%x, ", regs_val1);

    // [8] Version Register
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacVersion);
    printf("  [8] Version : 0x%x\n", regs_val1);

    // [14] Interrupt Status Register and Interrupt Mask
    regs_val1 = read_register(GMAC_BASE_ADDR, GmacInterruptStatus);
    regs_val2 = read_register(GMAC_BASE_ADDR, GmacInterruptMask);
    printf("  [14-15] Interrupt Status/Mask : 0x%x/0x%x\n", regs_val1, regs_val2);

    // MAC address
    gmac_get_mac_addr(mac_addr);
    printf("  [16-17] Mac Addr : %X:%X:%X:%X:%X:%X\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

void print_tx_dscrb(mac_t *mac)
{
    uint32_t i;
    printf("send buffer mac->saddr=0x%x ", mac->saddr);
    printf("mac->saddr_phy=0x%x ", mac->saddr_phy);
    printf("send discrb mac->td_phy=0x%x\n", mac->td_phy);
#if 0
    desc_t *send=mac->td;
    for(i=0;i<mac->pnum;i++)
    {
        printf("send[%d].tdes0=0x%x ",i,send[i].tdes0);
        printf("send[%d].tdes1=0x%x ",i,send[i].tdes1);
        printf("send[%d].tdes2=0x%x ",i,send[i].tdes2);
        printf("send[%d].tdes3=0x%x ",i,send[i].tdes3);
    }
#endif
}

void print_rx_dscrb(mac_t *mac)
{
    uint32_t i;
    printf("recieve buffer add mac->daddr=0x%x ", mac->daddr);
    printf("mac->daddr_phy=0x%x ", mac->daddr_phy);
    printf("recieve discrb add mac->rd_phy=0x%x\n", mac->rd_phy);
    desc_t *recieve = (desc_t *)mac->rd;
#if 0
    for(i=0;i<mac->pnum;i++)
    {
        printf("recieve[%d].tdes0=0x%x ",i,recieve[i].tdes0);
        printf("recieve[%d].tdes1=0x%x ",i,recieve[i].tdes1);
        printf("recieve[%d].tdes2=0x%x ",i,recieve[i].tdes2);
        printf("recieve[%d].tdes3=0x%x\n",i,recieve[i].tdes3);
    }
#endif
}

void irq_mac(void)
{
    //TODO
    clear_interrupt();
    do_unblock_one(&recv_block_queue);

    return;
}

void irq_enable(int IRQn)
{

}

void check_recv(mac_t *test_mac)
{
    int pnow = 0;
    while (pnow < (PNUM - 1))
    {
        if (((desc_t *)((uint32_t)recv_desc_table_ptr + DESC_SIZE * pnow))->des0 & 0x80000000) //FIXIT wait for the end
        {
            //pooling
            // vt100_move_cursor(1,14);
            // printk("Recv failed at pkg %x\n", pnow);
            return; //not finished
        }
        pnow++;
    }
    // vt100_move_cursor(1,14);
    // printk(" [CHECK] Recv completed.\n");

    return;
}

void set_sram_ctr()
{
    *((volatile unsigned int *)0xbfd00420) = 0x8000; /* 使能GMAC0 */
}

static void s_reset(mac_t *mac) //reset mac regs
{
    uint32_t time = 1000000;
    reg_write_32(mac->dma_addr, 0x01);

    while ((reg_read_32(mac->dma_addr) & 0x01))
    {
        reg_write_32(mac->dma_addr, 0x01);
        while (time)
        {
            time--;
        }
    };
}

void disable_interrupt_all(mac_t *mac)
{
    reg_write_32(mac->dma_addr + DmaInterrupt, DmaIntDisable);
    return;
}

void set_mac_addr(mac_t *mac)
{
    uint32_t data;
    uint8_t MacAddr[6] = {0x00, 0x55, 0x7b, 0xb5, 0x7d, 0xf7};
    uint32_t MacHigh = 0x40, MacLow = 0x44;
    data = (MacAddr[5] << 8) | MacAddr[4];
    reg_write_32(mac->mac_addr + MacHigh, data);
    data = (MacAddr[3] << 24) | (MacAddr[2] << 16) | (MacAddr[1] << 8) | MacAddr[0];
    reg_write_32(mac->mac_addr + MacLow, data);
}

uint32_t do_net_recv(uint32_t rd, uint32_t rd_phy, uint32_t daddr)
{
    //TODO
    // //PLEASE enable MAC-RX

    // // reg_write_32(DMA_BASE_ADDR + 0x18, reg_read_32(GMAC_BASE_ADDR + 0x18) | 0x02200002); // start tx, rx
    // reg_write_32(DMA_BASE_ADDR + 0x18, reg_read_32(DMA_BASE_ADDR + 0x18) | 0x02200002); // start tx, rx
    // reg_write_32(DMA_BASE_ADDR + 0x1c, 0x10001 | (1 << 6));
    
    // //you should add some code to start recv and check recv packages

    //PLEASE enable MAC-RX

    // 分别在DMA寄存器4（Transmit Descriptor List Address Register，偏移为0x10）和DMA寄存器3（Receive Descriptor
    // List Address Register，偏移为 0xC）中填入发送描述符和接收描述符的首物理地址。这个操作大家可以调用我
    // 们提供的 reg_write_32（）函数对寄存器赋值
    reg_write_32(DMA_BASE_ADDR + 0xC, (uint32_t)recv_desc_table_ptr);
    // reg_write_32(DMA_BASE_ADDR + 0xC, rd_phy);

    //分别将 mac 第 0 寄存器的第 2 位和第 3 位(从0开始)设置为 1，这样可以分别使能 MAC 传输功能和接收功能
    reg_write_32(GMAC_BASE_ADDR, reg_read_32(GMAC_BASE_ADDR) | 0x4);
    // reg_write_32(GMAC_BASE_ADDR, reg_read_32(GMAC_BASE_ADDR) | (1 << 2) | (1 << 3));
    // reg_write_32(GMAC_BASE_ADDR,reg_read_32(GMAC_BASE_ADDR)|1<<3|1<<4);

    // 配置 DMA 第 6 寄存器、DMA 第 7 寄存器。这个操作在 do_net_send（）和 do_net_recv（）里已经帮大家实
    // 现了，请大家查看《Loongson1C300_user_manual_v1.3》手册，了解配置的每一位的含义
    reg_write_32(DMA_BASE_ADDR + 0x18, reg_read_32(DMA_BASE_ADDR + 0x18) | 0x02200002); // start tx, rx
    reg_write_32(DMA_BASE_ADDR + 0x1c, 0x10001 | (1 << 6));

    // 在发送和接收前，每个描述符的 OWN 位需置 1，当开始发送和接收后，OWN 位从 1 变成 0 时（硬件自动置
    // 位），则此描述符已经完成的发送或接收，可以以 OWN 为判断是否完成了发送或接收；

    // 每次接收前需要在 DMA 寄存器 2（Receive Poll Demand Register）中写入任意值，接收 DMA 控制器将会读取
    // 寄存器 19 对应的描述符，这样当有数据包到达板卡时就会接收一个数据包?

    //注意每次写1对应一个包
    int l;
    for(l=0;l<64;l++)
    {
        ((desc_t*)(rd+l*DESC_SIZE))->des0=0x80000000;
    }

    for(l=0;l<64;l++)
    {
        reg_write_32(DMA_BASE_ADDR + DmaRxPollDemand, 0x1);
        // while(((desc_t*)(rd_phy+l*DESC_SIZE))->tdes0&0x80000000)
        // {
        //     //pooling
        // }
    }

    return 0; //if recev succeed
}

void do_net_send(uint32_t td, uint32_t td_phy)
{
     //TODO
    
    //  //PLEASE enable MAC-TX
    
    // reg_write_32(DMA_BASE_ADDR + 0x18, reg_read_32(DMA_BASE_ADDR + 0x18) | 0x02202000); //0x02202002); // start tx, rx
    // reg_write_32(DMA_BASE_ADDR + 0x1c, 0x10001 | (1 << 6));

    // //you should add some code to start send packages

    //PLEASE enable MAC-TX

    // 分别在DMA寄存器4（Transmit Descriptor List Address Register，偏移为0x10）和DMA寄存器3（Receive Descriptor
    // List Address Register，偏移为 0xC）中填入发送描述符和接收描述符的首物理地址。这个操作大家可以调用我
    // 们提供的 reg_write_32（）函数对寄存器赋值。
    reg_write_32(DMA_BASE_ADDR + 0x10, td_phy);

    // 分别将 mac 第 0 寄存器的第 2 位和第 3 位设置为 1，这样可以分别使能 MAC 传输功能和接收功能
    reg_write_32(GMAC_BASE_ADDR, reg_read_32(GMAC_BASE_ADDR) | (1 << 2) | (1 << 3) );

    // 配置 DMA 第 6 寄存器、DMA 第 7 寄存器。
    reg_write_32(DMA_BASE_ADDR + 0x18, reg_read_32(DMA_BASE_ADDR + 0x18) | 0x02202000); //0x02202002); // start tx, rx
    reg_write_32(DMA_BASE_ADDR + 0x1c, 0x10001 | (1 << 6));

    // 在发送和接收前，每个描述符的 OWN 位需置 1，当开始发送和接收后，OWN 位从 1 变成 0 时（硬件自动置
    // 位），则此描述符已经完成的发送或接收，可以以 OWN 为判断是否完成了发送或接收

    // 每次发送前需要在 DMA 寄存器 1（Transmit Poll Demand Register）中写入任意值，发送 DMA 控制器将会读取
    // 寄存器 18 对应的描述符，这样就开始发送了一个数据包。
    int l;
    for(l=0;l<64;l++)
    {
        ((desc_t*)(td+l*DESC_SIZE))->des0=0x80000000;
    }

    for(l=0;l<64;l++)
    {
        reg_write_32(DMA_BASE_ADDR + DmaTxPollDemand, 0x1);
        // vt100_move_cursor(1,9);
        // printk("\n %p\n",send_desc_table_ptr);
        // vt100_move_cursor(1,14);
        // printk(" [MAC] sending %d pkg",l);

        while((((desc_t*)(td+l*DESC_SIZE))->des0)&0x80000000)
        {
            //pooling
            // vt100_move_cursor(1,10);
            // printk("tdes0 %x\n",((desc_t*)(td+l*DESC_SIZE))->des0);
            // printk("tdes1 %x\n",((desc_t*)(td+l*DESC_SIZE))->des1);
            // printk("tdes2 %x\n",((desc_t*)(td+l*DESC_SIZE))->des2);
            // printk("tdes3 %x\n",((desc_t*)(td+l*DESC_SIZE))->des3);
            // printk(" %x\n",td+l*DESC_SIZE);
        }
    }

    return;
}

void do_init_mac(void)
{
    mac_t test_mac;
    uint32_t i;

    test_mac.mac_addr = 0xbfe10000;
    test_mac.dma_addr = 0xbfe11000;

    test_mac.psize = PSIZE * 4; // 64bytes
    test_mac.pnum = PNUM;       // pnum

    set_sram_ctr(); /* 使能GMAC0 */
    s_reset(&test_mac);
    disable_interrupt_all(&test_mac);
    set_mac_addr(&test_mac);

    // 在task3中，需要在网卡初始化时加入对第一组中断寄存器组的赋值：
    // p5无限进网卡中断的解决办法：
    // 1. INT1_CLR寄存器赋值为0xFFFFFFFF;
    // 2. INT_POL寄存器赋值为0xFFFFFFFF;
    // 3. INT_EDGE寄存器赋值为0
    *((uint32_t *)INT1_CLR ) = 0xFFFFFFFF;
    *((uint32_t *)INT1_POL ) = 0xFFFFFFFF;
    *((uint32_t *)INT1_EDGE) = 0;

    // send_desc_table_ptr = (desc_t*)SEND_DESC;
    // recv_desc_table_ptr = (desc_t*)RECV_DESC;
    send_desc_table_ptr = (desc_t*)send_desc_table;
    recv_desc_table_ptr = (desc_t*)recv_desc_table;

    bzero(send_desc_table_ptr, SEND_DESC_SIZE);
    bzero(recv_desc_table_ptr, RECV_DESC_SIZE);
}

void do_wait_recv_package(void)
{
    desc_t *recv_desc_table = recv_desc_table_ptr;
    enable_mac_int();

    if((recv_desc_table[63].des0 & 0x80000000) == 0x80000000){
        do_block(&recv_block_queue);        
    }

    return;
}

void check_recv_block_queue(void)
{
    if (recv_block_queue.head != 0)
    {
        int pnow = 0;
        while (pnow < (PNUM - 1))
        {
            if (((desc_t *)((uint32_t)recv_desc_table_ptr + DESC_SIZE * pnow))->des0 & 0x80000000) //FIXIT wait for the end
            {
                //pooling
                // vt100_move_cursor(1, 13);
                // printk(" [TI] waiting package %d, head: %x", pnow, *(recv_desc_table_ptr + pnow));
                return; //not finished
            }
            pnow++;
        }
        // vt100_move_cursor(1, 13);
        // printk(" [TI] package recieve 64 succeed. \n");

        vt100_move_cursor(1,15);
        int i, j;

        for(i = 30; i > 0; i--){
            for(j = 0; j < 16; j++){
                printk("%x ",  recv_buffer[(PNUM-i)*PSIZE+j]); 
            }

            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE     ]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 1 ]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 2 ]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 3 ]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 4 ]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 5 ]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 6 ]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 7 ]); 
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 8 ]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 9 ]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 10]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 11]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 12]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 13]);                
            // printk("%x ",  recv_buffer[(PNUM-i)*PSIZE + 14]);                
            // printk("%x\n", recv_buffer[(PNUM-i)*PSIZE + 15]);                
        }
        
        do_unblock_one(&recv_block_queue);
    }

    return;
}

uint32_t do_recv_desc_init(void *desc_addr, void *buffer, uint32_t bufsize, uint32_t pnum)
{
    //TODO
    int cnt = 0;
    uint32_t start_addr = (uint32_t)desc_addr;
    uint32_t addr = (uint32_t)desc_addr;

    //Not the last one
    while (++cnt < (pnum))
    {
        ((desc_t *)addr)->des0 = 0x00000000;
        ((desc_t *)addr)->des1 = 0 | (1 << 24) | (1 << 31) | (bufsize & 0x7ff);
        ((desc_t *)addr)->des2 = ((uint32_t)buffer + (cnt - 1) * bufsize)& 0x1fffffff;
        ((desc_t *)addr)->des3 = (addr + DESC_SIZE)& 0x1fffffff;
        addr += DESC_SIZE;
    }

    //The last one
    ((desc_t *)addr)->des0 = 0x00000000;
    ((desc_t *)addr)->des1 = 0 | (1 << 25) | (0 << 31) | (bufsize & 0x7ff);
    ((desc_t *)addr)->des2 = ((uint32_t)buffer + (cnt - 1) * bufsize)& 0x1fffffff;
    ((desc_t *)addr)->des3 = (start_addr)& 0x1fffffff;
    addr += DESC_SIZE;

    return start_addr;
}

uint32_t do_send_desc_init(void *desc_addr, void *buffer, uint32_t bufsize, uint32_t pnum)
{
    //TODO
    int cnt = 0;
    uint32_t start_addr = (uint32_t)desc_addr;
    uint32_t addr = (uint32_t)desc_addr;

    //Not the last one
    while (++cnt < (pnum))
    {
        ((desc_t *)addr)->des0 = 0x00000000;
        ((desc_t *)addr)->des1 = (0 | (0 << 31) | (1<<30) | (1<<29) | (1 << 24) | (bufsize & 0x7ff));
        ((desc_t *)addr)->des2 = ((uint32_t)buffer) & 0x1fffffff;
        ((desc_t *)addr)->des3 = (addr + DESC_SIZE) & 0x1fffffff;
        addr += DESC_SIZE;
    }

    //The last one
    ((desc_t *)addr)->des0 = 0x00000000;
    ((desc_t *)addr)->des1 = (0 | (0 << 31) | (1<<30) | (1<<29) | (1 << 25) | (1 << 24) | (bufsize & 0x7ff));
    ((desc_t *)addr)->des2 = ((uint32_t)buffer) & 0x1fffffff;
    ((desc_t *)addr)->des3 = (start_addr) & 0x1fffffff;
    addr += DESC_SIZE;

    return start_addr;
}

int register_irq_handler(uint32_t x, uint32_t irq_mac)
{

}

void enable_mac_int()
{
    *((uint32_t*)INT1_EN) = (0x00000001<<3);
    return;
}

void check_irq_mac()
{
    uint32_t CP0_CAUSE = get_cp0_status();
    uint32_t ip = ((CP0_CAUSE >> 8) & 0x000000FF);

    if(ip & (1<<3))//ip[3]==1
    {
        if((*(uint32_t*)INT1_SR)&(0x00000001<<3))
        {
            irq_mac();
        }
        // printk("invalid interrupt:%x\n",status);
        // panic("irq_mac");

        // vt100_move_cursor(1,20);
        // int i;
        // if(recv_buffer_ptr < PNUM){
        //     for(i = 0; i < PSIZE; i += 16){
        //         printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i]);                
        //         printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+1]);                
        //         printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+2]);                
        //         printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+3]);                
        //         printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+4]);                
        //         printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+5]);                
        //         printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+6]);                
        //         printk("%d\n", recv_buffer[recv_buffer_ptr*PSIZE + i+7]);                
        //         // printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+8]);                
        //         // printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+9]);                
        //         // printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+10]);                
        //         // printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+11]);                
        //         // printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+12]);                
        //         // printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+13]);                
        //         // printk("%d", recv_buffer[recv_buffer_ptr*PSIZE + i+14]);                
        //         // printk("%d\n", recv_buffer[recv_buffer_ptr*PSIZE + i+15]);                
        //     }
        //     recv_buffer_ptr++;
        // }


        // char recv_buffer_1[8] = {0};                
        // char recv_buffer_2[8] = {0};
        // char recv_buffer_3[8] = {0};
        // char recv_buffer_4[8] = {0};
        // char recv_buffer_5[8] = {0};
        // char recv_buffer_6[8] = {0};
        // char recv_buffer_7[8] = {0};                
        // char recv_buffer_8[8] = {0};
/*
        vt100_move_cursor(1,15);
        // int i;
        if(recv_buffer_ptr < PNUM){
            // for(i = 0; i < PSIZE; i += 8){
                // htoa(recv_buffer[recv_buffer_ptr*PSIZE + i    ], recv_buffer_1);
                // htoa(recv_buffer[recv_buffer_ptr*PSIZE + i + 1], recv_buffer_2);
                // htoa(recv_buffer[recv_buffer_ptr*PSIZE + i + 2], recv_buffer_3);
                // htoa(recv_buffer[recv_buffer_ptr*PSIZE + i + 3], recv_buffer_4);
                // htoa(recv_buffer[recv_buffer_ptr*PSIZE + i + 4], recv_buffer_5);
                // htoa(recv_buffer[recv_buffer_ptr*PSIZE + i + 5], recv_buffer_6);
                // htoa(recv_buffer[recv_buffer_ptr*PSIZE + i + 6], recv_buffer_7);
                // htoa(recv_buffer[recv_buffer_ptr*PSIZE + i + 7], recv_buffer_8);
                
                // printk("%s ",  recv_buffer_1);                
                // printk("%s ",  recv_buffer_2);                
                // printk("%s ",  recv_buffer_3);                
                // printk("%s ",  recv_buffer_4);                
                // printk("%s ",  recv_buffer_5);                
                // printk("%s ",  recv_buffer_6);                
                // printk("%s ",  recv_buffer_7);                
                // printk("%s\n", recv_buffer_8);     

                printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE  ]);                
                printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE+1]);                
                printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE+2]);                
                printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE+3]);                
                printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE+4]);                
                printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE+5]);                
                printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE+6]);                
                printk("%x ", recv_buffer[recv_buffer_ptr*PSIZE+7]); 

                // printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE + i  ]);                
                // printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE + i+1]);                
                // printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE + i+2]);                
                // printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE + i+3]);                
                // printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE + i+4]);                
                // printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE + i+5]);                
                // printk("%x ",  recv_buffer[recv_buffer_ptr*PSIZE + i+6]);                
                // printk("%x\n", recv_buffer[recv_buffer_ptr*PSIZE + i+7]); 

                printk("%x ", recv_buffer[recv_buffer_ptr*PSIZE + 8]);                
                printk("%x ", recv_buffer[recv_buffer_ptr*PSIZE + 9]);                
                printk("%x ", recv_buffer[recv_buffer_ptr*PSIZE + 10]);                
                printk("%x ", recv_buffer[recv_buffer_ptr*PSIZE + 11]);                
                printk("%x ", recv_buffer[recv_buffer_ptr*PSIZE + 12]);                
                printk("%x ", recv_buffer[recv_buffer_ptr*PSIZE + 13]);                
                printk("%x ", recv_buffer[recv_buffer_ptr*PSIZE + 14]);                
                printk("%x \n", recv_buffer[recv_buffer_ptr*PSIZE +15]);                
            // }
            recv_buffer_ptr++;
        }
*/
        return;
    }
}

//------------------------BONUS-------------------------

uint32_t do_net_fast_recv(uint32_t rd, uint32_t rd_phy, uint32_t daddr)
{
    //PLEASE enable MAC-RX

    // 分别在DMA寄存器4（Transmit Descriptor List Address Register，偏移为0x10）和DMA寄存器3（Receive Descriptor
    // List Address Register，偏移为 0xC）中填入发送描述符和接收描述符的首物理地址。这个操作大家可以调用我
    // 们提供的 reg_write_32（）函数对寄存器赋值
    reg_write_32(DMA_BASE_ADDR + 0xC, (uint32_t)recv_desc_table_ptr);
    // reg_write_32(DMA_BASE_ADDR + 0xC, rd_phy);

    //分别将 mac 第 0 寄存器的第 2 位和第 3 位(从0开始)设置为 1，这样可以分别使能 MAC 传输功能和接收功能
    reg_write_32(GMAC_BASE_ADDR, reg_read_32(GMAC_BASE_ADDR) | 0x4);
    // reg_write_32(GMAC_BASE_ADDR, reg_read_32(GMAC_BASE_ADDR) | (1 << 2) | (1 << 3));
    // reg_write_32(GMAC_BASE_ADDR,reg_read_32(GMAC_BASE_ADDR)|1<<3|1<<4);

    // 配置 DMA 第 6 寄存器、DMA 第 7 寄存器。这个操作在 do_net_send（）和 do_net_recv（）里已经帮大家实
    // 现了，请大家查看《Loongson1C300_user_manual_v1.3》手册，了解配置的每一位的含义
    reg_write_32(DMA_BASE_ADDR + 0x18, reg_read_32(DMA_BASE_ADDR + 0x18) | 0x02200002); // start tx, rx
    reg_write_32(DMA_BASE_ADDR + 0x1c, 0x10001 | (1 << 6));

    // 在发送和接收前，每个描述符的 OWN 位需置 1，当开始发送和接收后，OWN 位从 1 变成 0 时（硬件自动置
    // 位），则此描述符已经完成的发送或接收，可以以 OWN 为判断是否完成了发送或接收；

    // 每次接收前需要在 DMA 寄存器 2（Receive Poll Demand Register）中写入任意值，接收 DMA 控制器将会读取
    // 寄存器 19 对应的描述符，这样当有数据包到达板卡时就会接收一个数据包?

    //注意每次写1对应一个包
    int l;
    for(l=0;l<256;l++)
    {
        ((desc_t*)(rd+l*DESC_SIZE))->des0=0x80000000;
    }

    for(l=0;l<256;l++)
    {
        reg_write_32(DMA_BASE_ADDR + DmaRxPollDemand, 0x1);
        // while(((desc_t*)(rd_phy+l*DESC_SIZE))->tdes0&0x80000000)
        // {
        //     //pooling
        // }
    }

    return 0; //if recev succeed
}


