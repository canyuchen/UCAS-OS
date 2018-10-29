#include "asm.h"

.equ    KERNEL, 0
.equ    USER,   156
.equ    MODE_POSITION_OFFSET, 324

//start interrupt
.macro STI
    mfc0    k0, CP0_STATUS
    li      k1, 0x1
    or      k0, k1
    mtc0    k0, CP0_STATUS
.endm

//close interrupt
.macro CLI 
    mfc0    k0, CP0_STATUS
    li      k1, 0xfffffffe
    and     k0, k1
    mtc0    k0, CP0_STATUS
.endm


LEAF(asm_start)
    mtc0    zero, CP0_STATUS
    mtc0    zero, CP0_WATCHLO
    mtc0    zero, CP0_WATCHHI

    mfc0    t0, CP0_CONFIG
    nop
    and     t0, ~0x7
    ori     t0, 0x2
    mtc0    t0, CP0_CONFIG

    jr  ra
END(asm_start)

.macro SAVE_CONTEXT offset
    // TODO save context
    // k0,k1 deal with operating system & exception handling retention 
    // These two registers will not be used during normal operation. 
    // so these two registers needn't be saved

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

NESTED(do_scheduler, 0, ra)
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
exception_handler_begin:
    // TODO close interrupt
    // jmp exception_handler[i] which decided by CP0_CAUSE
    // Leve2 exception Handler.
    //.set  noat
    .set  mips32

    CLI //close interrupt

    mfc0  k0, CP0_CAUSE
    andi  k0, k0, CAUSE_EXCCODE   /* k0 = {exc_code,00} */

    la    k1, exception_handlers
    add   k0, k0, k1
    lw    k0, 0(k0)
    j     k0     /* exception_handlers[exc_code](); */
exception_handler_end:
END(exception_handler_entry)

NESTED(handle_others,0,sp)
    /* SAVE_CONTEXT(KERNEL) */
    SAVE_CONTEXT(USER)

    mfc0  k0, CP0_CAUSE         /* Read Cause register for IP bits */
    //mfc0  k1, CP0_STATUS      /* and Status register for IM bits */
    andi  k0, k0, CAUSE_IPL     /* Keep only IP bits from Cause */
    //and   k0, k0, k1          /* and mask with IM bits */
    clz   k0, k0                /* Find first bit set, IP7..IP0; k0 = 16..23 */
                                /* The CLZ instruction counts the number of leading 
                                 * zeros in a word. Scan rs and write into rd.
                                */
    xori  k0, k0, 0x17          /* 16..23 => 7..0 */
    add   a0, k0, $0

    mfc0  k0, CP0_CAUSE
    andi  k0, k0, CAUSE_EXCCODE /* k0 = {exc_code,00} */
    srl   k0, k0, 2
    add   a1, k0, $0

    //jal   c_handle_others

    mfc0  k0, CP0_CAUSE         /* Read Cause register for IP bits */
    nop
    andi  k1, k0, CAUSE_IPL     /* Keep only IP bits from Cause */
    xor   k0, k0, k1
    mtc0  k0, CP0_CAUSE
    nop

    //LEAVE_CRITICAL
    /* RESTORE_CONTEXT(KERNEL) */
    RESTORE_CONTEXT(USER)
    j     return_from_exception
    nop
END(handle_others)

NESTED(handle_int, 0, sp)
    //.set  noat
    .set  mips32
    // interrupt handler
    // Leve3 exception Handler.

    /* TODO: timer_irq */
    /* read int IP and handle clock interrupt or just call do_nothing */
    SAVE_CONTEXT(USER)

    mfc0  k0, CP0_CAUSE
    nop
    andi  k0, k0, CAUSE_IPL
    clz   k0, k0
    xori  k0, k0, 0x17

    addiu k1, zero, 7
    beq   k0, k1, irq_timer
    nop

    jal   clear_int

int_finish:
    RESTORE_CONTEXT(USER)
    j     return_from_exception
    nop

irq_timer:
    // TODO clock interrupt handler.
    //      scheduler, time counter in here
    jal   reset_timer

    jal   clear_int

    lw    k1, time_elapsed
    addi  k1, k1, 15
    sw    k1, time_elapsed

    /* Round-robin scheduling: Save current_running before preempting */
    jal   do_scheduler
    nop

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

    /* TODO:end */
END(handle_int)

NESTED(handle_syscall, 0, sp)
    // system call handler
    SAVE_CONTEXT(USER)
    // RESTORE_CONTEXT(KERNEL)

    add   a3,a2,$0
    add   a2,a1,$0
    add   a1,a0,$0
    add   a0,v0,$0

    jal   system_call_helper

    //LEAVE_CRITICAL
    RESTORE_CONTEXT(USER)
    j     return_from_exception
END(handle_syscall)

NESTED(handle_other, 0, sp)
    // other exception handler
END(handle_other)

LEAF(reset_timer)
    li    a0, TIMER_INTERVAL
    mtc0  $0, CP0_COUNT
    mtc0  a0, CP0_COMPARE
    jr    ra
END(reset_timer)

LEAF(first_entry)
    //LEAVE_CRITICAL
    j     int_finish
END(first_entry)

/* return CP0_STATUS*/
LEAF(get_cp0_status)
  mfc0  v0, CP0_STATUS
  jr    ra
END(get_cp0_status)

/* set CP0_STATUS*/
LEAF(set_cp0_status)
  mtc0  a0, CP0_STATUS
  jr    ra
END(set_cp0_status)

LEAF(return_from_exception)
	STI
	eret
END(return_from_exception)

/* TODO: Test current_running->nested_count, clobbering only eflags */
.macro TEST_NESTED_COUNT
  lw      k1, current_running
  lw      k1, NESTED_COUNT(k1)
.endm
