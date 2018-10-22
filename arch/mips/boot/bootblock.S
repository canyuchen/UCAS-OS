/*
.text
.global main

main:
	# use your own code in here!
	# 1) call BIOS print string "It's bootblock!
	# 2) call BIOS read kernel in SD card.
	# 3) jump to kernel.
# 4) while(1) --> stop here
stop:
	j stop
.data

msg: .ascii "It's a bootloader...\n"

# 1. PMON read SD card function address
# read_sd_card();
read_sd_card: .word 0x8007b1cc

# 2. PMON print string function address
# printstr(char *string)
printstr: .word 0x8007b980

# 3. PMON print char function address
# printch(char ch)
printch: .word 0x8007ba00

# 4. kernel address (move kernel to here ~)
kernel : .word 0xa0800200

# 5. kernel main address (jmp here to start kernel main!)
kernel_main : .word 0xa0800200
*/

.text
.global main

main:
	# 1) task1 call BIOS print string "It's bootblock!"
	#lw $4, msg
	la  $a0, msg
	#jal printstr
    jal 0x8007b980
	# 2) task2 call BIOS read kernel in SD card and jump to kernel start
	
	li  $a0, 0xa0800200
	li  $a1, 0x200
	#li  $a2, 0x90
	#li  $a2, 0x1f4
	#li  $a2, 0x200

	lw  $a2, 0xa08001f0
	sll $a2, $a2, 9

	#lw  $a2, 0xa08001f0

	jal 0x8007b1cc
	jal 0xa0800200	

# while(1) --> stop here
stop:
	j stop

.data

#os_size: .word 0x1f4
#os_size: .word 0x200
#os_size: .word 0xa08001f0
os_sec_num: .word 0xa08001f0

msg: .ascii "It's bootloader!\n"

# 1. PMON read SD card function address
# read_sd_card();
read_sd_card: .word 0x8007b1cc

# 2. PMON print string function address
# printstr(char *string)
printstr: .word 0x8007b980

# 3. PMON print char function address
# printch(char ch)
printch: .word 0x8007ba00

# 4. kernel address (move kernel to here ~)
kernel: .word 0xa0800200

# 5. kernel main address (jmp here to start kernel main!)
kernel_main: .word 0xa0800200