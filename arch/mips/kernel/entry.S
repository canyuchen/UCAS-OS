#include "asm.h"

.equ    KERNEL, 0
.equ    USER,   156

.macro STI
.endm

.macro CLI
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
//.macro SAVE_CONTEXT (%offset)
    // TODO save context
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
    //jr ra
.endm

.macro RESTORE_CONTEXT offset
//.macro RESTORE_CONTEXT (%offset)
    // TODO restore context
    lw $31, current_running
    lw $0 , ($31)
    lw $1 , 4($31)  
    lw $2 , 8($31)  
    lw $3 , 12($31)
    lw $4 , 16($31)
    lw $5 , 20($31)
    lw $6 , 24($31)
    lw $7 , 28($31)
    lw $8 , 32($31)
    lw $9 , 36($31)
    lw $10, 40($31)
    lw $11, 44($31)
    lw $12, 48($31)
    lw $13, 52($31)
    lw $14, 56($31)
    lw $15, 60($31)
    lw $16, 64($31)
    lw $17, 68($31) 
    lw $18, 72($31)
    lw $19, 76($31)
    lw $20, 80($31)
    lw $21, 84($31)
    lw $22, 88($31)
    lw $23, 92($31)
    lw $24, 96($31)
    lw $25, 100($31)
    lw $26, 104($31)
    lw $27, 108($31)
    lw $28, 112($31)
    lw $29, 116($31)
    lw $30, 120($31)
    lw $31, 124($31)
/*
    lw $31, current_running
    //addiu $31, $31, 320
    lw $31, 320($31)
*/
    //lw ra,  320(current_running)
    //jr ra
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
