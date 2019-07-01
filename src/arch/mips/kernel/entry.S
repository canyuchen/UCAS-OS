#include "asm.h"

.equ    KERNEL, 0
.equ    USER,   156
.equ    TASK_MODE_OFFSET, 324
.equ    TASK_PID_OFFSET,  320

//start interrupt
.macro STI
    mfc0    k0, CP0_STATUS
    nop
    li      k1, 0x1
    or      k0, k1
    mtc0    k0, CP0_STATUS
    nop
.endm

//close interrupt
.macro CLI 
    mfc0    k0, CP0_STATUS
    nop
    li      k1, 0xfffffffe
    and     k0, k1
    mtc0    k0, CP0_STATUS
    nop
.endm

/* TODO: Test current_running->nested_count, clobbering only eflags */
.macro TEST_TASK_MODE
    lw      k1, current_running
    lw      k1, TASK_MODE_OFFSET(k1)
.endm

LEAF(asm_start)
    .set    noreorder
    mtc0    zero, CP0_STATUS
    nop
    mtc0    zero, CP0_WATCHLO
    nop
    mtc0    zero, CP0_WATCHHI
    nop
    mfc0    t0, CP0_CONFIG
    nop
    and     t0, ~0x7
    ori     t0, 0x2
    mtc0    t0, CP0_CONFIG
    nop
    jr  ra
    nop
END(asm_start)

LEAF(close_clock_interrupt)
    mfc0    k0, CP0_STATUS
    nop
    li      k1, 0xffff7fff
    and     k0, k1
    mtc0    k0, CP0_STATUS
    nop
    jr      ra
    nop
END(close_clock_interrupt)

LEAF(enable_clock_interrupt)
    mfc0    k0, CP0_STATUS
    nop
    li      k1, 0xffff7fff
    or      k0, k1
    mtc0    k0, CP0_STATUS
    nop
    jr      ra
    nop
END(enable_clock_interrupt)

.macro SAVE_CONTEXT offset
    // TODO save context
    // k0,k1 deal with operating system & exception handling retention 
    // These two registers will not be used during normal operation. 
    // so these two registers donot need be saved

    /* zero register, its content is always 0 */
    lw k0, current_running
    addi k0, k0, \offset

    sw zero, (k0)
    /* reserved for assembler */
    sw AT, 4(k0)  
    /* reserved for function return */
    sw v0, 8(k0)  
    sw v1, 12(k0)
    /* reserved for function parameter */
    sw a0, 16(k0)
    sw a1, 20(k0)
    sw a2, 24(k0)
    sw a3, 28(k0)
    /* temporary register */
    sw t0, 32(k0)
    sw t1, 36(k0)
    sw t2, 40(k0)
    sw t3, 44(k0)
    sw t4, 48(k0)
    sw t5, 52(k0)
    sw t6, 56(k0)
    sw t7, 60(k0)
    sw t8, 64(k0)
    sw t9, 68(k0) 
    sw s0, 72(k0)
    sw s1, 76(k0)
    sw s2, 80(k0)
    sw s3, 84(k0)
    sw s4, 88(k0)
    sw s5, 92(k0)
    sw s6, 96(k0)
    sw s7, 100(k0)
    /* global pointer */
    sw gp, 112(k0)
    /* stack pointer */
    sw sp, 116(k0)
    /* frame pointer */
    sw fp, 120(k0)
    /* saved return address */
    sw ra, 124(k0)

    mfc0    k1, CP0_STATUS
    nop
    sw      k1, 128(k0)

    mfc0    k1, CP0_TAGHI
    nop
    sw      k1, 132(k0)

    mfc0    k1, CP0_TAGLO
    nop
    sw      k1, 136(k0)

    mfc0    k1, CP0_BADVADDR
    nop
    sw      k1, 140(k0)

    mfc0    k1, CP0_CAUSE
    nop
    sw      k1, 144(k0)

    mfc0    k1, CP0_EPC
    nop
    sw      k1, 148(k0)
.endm

.macro SAVE_CONTEXT_SP offset
    lw k0, current_running
    addi k0, k0, \offset
    /* stack pointer */
    sw sp, 116(k0)
.endm

.macro RESTORE_CONTEXT offset
    // TODO restore context
    /* zero register, its content is always 0 */
    lw k0, current_running
    addi k0, k0, \offset

    lw zero, (k0)
    /* reserved for assembler */
    lw AT, 4(k0)  
    /* reserved for function return */
    lw v0, 8(k0)  
    lw v1, 12(k0)
    /* reserved for function parameter */
    lw a0, 16(k0)
    lw a1, 20(k0)
    lw a2, 24(k0)
    lw a3, 28(k0)
    /* temporary register */
    lw t0, 32(k0)
    lw t1, 36(k0)
    lw t2, 40(k0)
    lw t3, 44(k0)
    lw t4, 48(k0)
    lw t5, 52(k0)
    lw t6, 56(k0)
    lw t7, 60(k0)
    lw t8, 64(k0)
    lw t9, 68(k0) 
    lw s0, 72(k0)
    lw s1, 76(k0)
    lw s2, 80(k0)
    lw s3, 84(k0)
    lw s4, 88(k0)
    lw s5, 92(k0)
    lw s6, 96(k0)
    lw s7, 100(k0)
    /* global pointer */
    lw gp, 112(k0)
    /* stack pointer */
    lw sp, 116(k0)
    /* frame pointer */
    lw fp, 120(k0)
    /* saved return address */
    lw ra, 124(k0)

    lw      k1, 128(k0)
    mtc0    k1, CP0_STATUS
    nop //???

    lw      k1, 132(k0)
    mtc0    k1, CP0_TAGHI
    nop

    lw      k1, 136(k0)
    mtc0    k1, CP0_TAGLO
    nop

    lw      k1, 140(k0)
    mtc0    k1, CP0_BADVADDR
    nop

    lw      k1, 144(k0)
    mtc0    k1, CP0_CAUSE
    nop

    lw      k1, 148(k0)
    mtc0    k1, CP0_EPC
    nop
.endm

.macro RESTORE_CONTEXT_SP offset
    lw k0, current_running
    addi k0, k0, \offset
    /* stack pointer */
    lw sp, 116(k0)
.endm

NESTED(do_scheduler, 0, ra)
    .set    noreorder
    //.set  noat
    //???
    //Warning: used $at without ".set noat"
    SAVE_CONTEXT(KERNEL)
    jal     scheduler
    nop
    RESTORE_CONTEXT(KERNEL)
    jr      ra
    nop
END(do_scheduler)

.global exception_handler_begin
.global exception_handler_end

NESTED(exception_handler_entry, 0, sp)   
    .set    noreorder
exception_handler_begin:
    // TODO close interrupt
    // jmp exception_handler[i] which decided by CP0_CAUSE
    // Leve2 exception Handler.
    //.set  noat
    .set  mips32

    CLI //close interrupt

    mfc0  k0, CP0_CAUSE
    nop
    andi  k0, k0, CAUSE_EXCCODE   /* k0 = {exc_code,00} */

    la    k1, exception_handlers
    add   k0, k0, k1
    lw    k0, 0(k0)
    j     k0     /* exception_handlers[exc_code](); */
    nop
exception_handler_end:
END(exception_handler_entry)

NESTED(handle_others,0,sp)
    .set    noreorder

    mfc0 k1, CP0_STATUS
    nop
    mfc0 k1, CP0_CAUSE
    nop

    /* SAVE_CONTEXT(KERNEL) */
    SAVE_CONTEXT(USER)

    jal   do_scheduler
    nop
    
    RESTORE_CONTEXT(USER)
    j     return_from_exception
    nop
END(handle_others)

NESTED(handle_int, 0, sp)
    .set    noreorder
    //.set  noat
    .set  mips32
    // interrupt handler
    // Leve3 exception Handler.

    /* TODO: timer_irq */
    /* read int IP and handle clock interrupt or just call do_nothing */
    SAVE_CONTEXT(USER)
    RESTORE_CONTEXT(KERNEL)
    // ???

    mfc0  k0, CP0_CAUSE
    nop
    andi  k0, k0, CAUSE_IPL
    clz   k0, k0
    nop
    xori  k0, k0, 0x17

    addiu k1, zero, 7
    beq   k0, k1, irq_timer
    nop

    jal   check_irq_mac
    nop

    jal   clear_int
    nop
int_finish:
    SAVE_CONTEXT(KERNEL)
    RESTORE_CONTEXT(USER)
    j     return_from_exception
    nop

irq_timer:
    // TODO clock interrupt handler.
    //      scheduler, time counter in here
    jal   reset_timer
    nop
    jal   clear_int
    nop
    lw    k1, time_elapsed
    addi  k1, k1, 15
    sw    k1, time_elapsed

    jal   check_recv_block_queue
    nop

    TEST_TASK_MODE
    beq   k1, zero, int_finish
    nop

    lw    k0, current_running
    sw    zero, TASK_MODE_OFFSET(k0)

    /* Round-robin scheduling: Save current_running before preempting */
    jal   do_scheduler
    nop

    lw    k0, current_running
    li    k1, 1
    sw    k1, TASK_MODE_OFFSET(k0)

    //LEAVE_CRITICAL
    j     int_finish
    nop

clear_int:
    mfc0  k0, CP0_CAUSE
    nop
    andi  k1, k0, CAUSE_IPL
    xor   k0, k0, k1
    mtc0  k0, CP0_CAUSE
    nop
    jr    ra
    nop
    /* TODO:end */
END(handle_int)

NESTED(handle_syscall, 0, sp)
    .set    noreorder
    // system call handler
    SAVE_CONTEXT(USER)
    // RESTORE_CONTEXT(KERNEL)
    // ???

    add   a3,a2,$0
    add   a2,a1,$0
    add   a1,a0,$0
    add   a0,v0,$0

    mfc0 k1, CP0_EPC
    nop
    jal   system_call_helper
    nop
    mfc0 k1, CP0_EPC
    nop

    //LEAVE_CRITICAL
    RESTORE_CONTEXT(USER)
    nop
    mfc0 k1, CP0_EPC
    nop
    j     return_from_exception
    nop
END(handle_syscall)
/*
NESTED(handle_other, 0, sp)
    // other exception handler
END(handle_other)
*/
LEAF(reset_timer)
    .set    noreorder
    li    a0, TIMER_INTERVAL
    mtc0  $0, CP0_COUNT
    nop
    mtc0  a0, CP0_COMPARE
    nop
    jr    ra
    nop
END(reset_timer)

LEAF(first_entry)
    .set    noreorder
    //LEAVE_CRITICAL

    RESTORE_CONTEXT(KERNEL)
    // jal   clear_int
    jal   reset_timer
    nop
    j     int_finish
    nop
END(first_entry)

/* return CP0_STATUS*/
LEAF(get_cp0_status)
    .set    noreorder
    mfc0  v0, CP0_STATUS
    nop
    jr    ra
    nop
END(get_cp0_status)

/* set CP0_STATUS*/
LEAF(set_cp0_status)
    .set    noreorder
    mtc0  a0, CP0_STATUS
    nop
    jr    ra
    nop
END(set_cp0_status)

LEAF(return_from_exception)
    .set    noreorder
    // jal   reset_timer
	STI
    mfc0 k1, CP0_EPC
    nop
	eret
    nop
    // STI
END(return_from_exception)

//---------------------------P5:virtual memory----------------------------------

LEAF(get_asid)
    mfc0  v0, CP0_ENTRYHI
    nop
    and	v0, 0x7
    nop
    jr ra
    nop
END(get_asid)

LEAF(set_entryhi_asid)
	li 	  k0, 0xffffff00
	mfc0  k1, CP0_ENTRYHI
	nop
	and   k1, k1, k0
    
	or	  k1, k1, a0
	mtc0  k1, CP0_ENTRYHI
	nop
	jr    ra
	nop
END(set_entryhi_asid)

LEAF(get_cp0_index)
    mfc0  v0, CP0_INDEX
    nop
    jr ra
    nop
END(get_cp0_index)

LEAF(set_cp0_index)
    mtc0  a0, CP0_INDEX
    nop
    jr ra
    nop
END(set_cp0_index)

LEAF(get_cp0_entryhi)
    mfc0  v0, CP0_ENTRYHI
    nop
    jr ra
    nop
END(get_cp0_entryhi)

LEAF(set_cp0_entryhi)
    mtc0  a0, CP0_ENTRYHI
    nop
    jr ra
    nop
END(set_cp0_entryhi)

LEAF(get_cp0_badvaddr)
    mfc0  v0, CP0_BADVADDR
    nop
    jr ra
    nop
END(get_cp0_badvaddr)

LEAF(get_cp0_entrylo0)
    mfc0  v0, CP0_ENTRYLO0
    nop
    jr ra
    nop
END(get_cp0_entrylo0)

LEAF(set_cp0_entrylo0)
    mtc0  a0, CP0_ENTRYLO0
    nop
    jr ra
    nop
END(set_cp0_entrylo0)

LEAF(get_cp0_entrylo1)
    mfc0  v0, CP0_ENTRYLO1
    nop
    jr ra
    nop
END(get_cp0_entrylo1)

LEAF(set_cp0_entrylo1)
    mtc0  a0, CP0_ENTRYLO1
    nop
    jr ra
    nop
END(set_cp0_entrylo1)

LEAF(get_cp0_pagemask)
    mfc0  v0, CP0_PAGEMASK
    nop
    jr ra
    nop
END(get_cp0_pagemask)

LEAF(set_cp0_pagemask)
    mtc0  a0, CP0_PAGEMASK
    nop
    jr ra
    nop
END(set_cp0_pagemask)

LEAF(get_sp_reg)
    add  v0, sp, zero
    nop
    jr ra
    nop
END(get_sp_reg)

LEAF(tlb_flush)
    mfc0  k1,CP0_ENTRYHI
    nop
    mtc0  a0,CP0_ENTRYHI
    nop
    tlbp
    nop
    nop
    nop
    nop
    mfc0  k0,CP0_INDEX
    nop
    bltz  k0,NOFOUND
    nop
    mtc0  zero,CP0_ENTRYHI
    nop
    mtc0  zero,CP0_ENTRYLO0
    nop
    mtc0  zero,CP0_ENTRYLO1
    nop
    tlbwi
    nop
    nop
    nop
    nop
NOFOUND:
    mtc0  k1,CP0_ENTRYHI
    nop
    j     ra
    nop
END(tlb_flush)

LEAF(tlb_flush_no_check)
    mtc0  zero,CP0_ENTRYHI
    nop
    mtc0  zero,CP0_ENTRYLO0
    nop
    mtc0  zero,CP0_ENTRYLO1
    nop
    mtc0  zero,CP0_PAGEMASK
    nop
    tlbwi   //CP0_INDEX has been set as invalid address    
    nop
    nop
    nop
    nop
    j     ra
    nop
END(tlb_flush_no_check)

LEAF(tlb_flush_all)
    li	    a3, 0
    mtc0	zero, CP0_PAGEMASK
    li	    a2, 32
    lui	    a1, 0x8000
    1:
    mtc0	a1, CP0_ENTRYHI
    mtc0	zero, CP0_ENTRYLO0
    mtc0	zero, CP0_ENTRYLO1
    mtc0	a3, CP0_INDEX
    addiu	a3, 1
    tlbwi
    bne	    a3, a2, 1b
    nop
END(tlb_flush_all)

//TO DO
NESTED(handle_tlb,0,sp)
    SAVE_CONTEXT(USER)

    RESTORE_CONTEXT(KERNEL)
    //RESTORE_CONTEXT_SP(KERNEL)

/*
    jal   print_sp_reg
    nop
*/
	// find the bad virtual address
	jal   handle_tlb_exception_helper
	nop	

    /*  TLB refill   */
/*
	lw	  t0, current_running
	lw 	  t0, TASK_PAGE_TABLE_BASE_OFFSET(t0)  // t0 = page table
	la	  t1, current_running
	lw	  t1, 0(t1)
	lw 	  t7, TASK_PID_OFFSET(t1)	           // t7 = pid	
	mfc0  t1, CP0_BADVADDR  	               // t1 = badvaddr	
    nop
	srl	  t1, 13
	sll	  t1, 13	                           // t1 = VPN2
	or	  t3, t1, t7 	                       // EntryHi
	mtc0  t3, CP0_ENTRYHI
    nop
	srl	  t1, 10	                           // t1 to be the offset in page table
	add	  t0, t0, t1
	lw	  t6, (t0)	                           // t6 = even page entry
	lw	  t5, 4(t0)   	                       // t5 = odd page entry
	mtc0  t6, CP0_ENTRYLO0   
    nop
	mtc0  t5, CP0_ENTRYLO1
    nop
	mtc0  zero, CP0_PAGEMASK
	nop
    tlbwr
    nop
	j     return_from_exception
	nop
*/

    SAVE_CONTEXT(KERNEL)
    //SAVE_CONTEXT_SP(KERNEL)
    RESTORE_CONTEXT(USER)
    nop
    mfc0 k1, CP0_EPC
    nop
    j     return_from_exception
    nop

END(handle_tlb)





