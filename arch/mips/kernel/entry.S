#include "asm.h"

.equ    KERNEL, 0
.equ    USER,   156
.equ    MODE_POSITION_OFFSET, 324

.macro STI
    //start interrupt
    mfc0    k0, CP0_STATUS
    li      k1, (STATUS_CU0 | 0x1)
    or      k0, k1
    mtc0    k0, CP0_STATUS
.endm

.macro CLI 
    //close interrupt
    mfc0    k0, CP0_STATUS
    li      k1, (STATUS_CU0 | 0x1)
    or      k0, k1
    xori    k0, 0x1
    mtc0    k0, CP0_STATUS
.endm

LEAF(asm_start)
    mtc0    zero, CP0_STATUS
    mtc0    zero, CP0_WATCHLO
    mtc0    zero, CP0_WATCHHI

    mfc0    t0, CP0_CONFIG
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
    nop

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
    SAVE_CONTEXT(KERNEL)
    jal     scheduler
    RESTORE_CONTEXT(KERNEL)
    jr      ra
END(do_scheduler)

.global exception_handler_begin
.global exception_handler_end

NESTED(exception_handler_entry, 0, sp)   
exception_handler_begin:
    // TODO close interrupt
    // jmp exception_handler[i] which decided by CP0_CAUSE
    // Leve2 exception Handler.
    
exception_handler_end:
END(exception_handler_entry)

NESTED(handle_int, 0, sp)
    // interrupt handler
    // Leve3 exception Handler.

END(handle_int)

NESTED(handle_syscall, 0, sp)
    // system call handler
END(handle_syscall)

NESTED(handle_other, 0, sp)
    // other exception handler
END(handle_other)

/*
.align	2
.globl  save_pcb
.ent	save_pcb
.type	save_pcb, @function
save_pcb:
    lw $26, current_running
    sw $0 , ($26)
    sw $1 , 4($26)  
    sw $2 , 8($26)  
    sw $3 , 12($26)
    sw $4 , 16($26)
    sw $5 , 20($26)
    sw $6 , 24($26)
    sw $7 , 28($26)
    sw $8 , 32($26)
    sw $9 , 36($26)
    sw $10, 40($26)
    sw $11, 44($26)
    sw $12, 48($26)
    sw $13, 52($26)
    sw $14, 56($26)
    sw $15, 60($26)
    sw $16, 64($26)
    sw $17, 68($26) 
    sw $18, 72($26)
    sw $19, 76($26)
    sw $20, 80($26)
    sw $21, 84($26)
    sw $22, 88($26)
    sw $23, 92($26)
    sw $24, 96($26)
    sw $25, 100($26)
    //sw $26, 104($26)
    sw $27, 108($26)
    sw $28, 112($26)
    sw $29, 116($26)
    sw $30, 120($26)
    sw $31, 124($26)
*/
