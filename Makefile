CC = mipsel-linux-gcc

all: clean createimage image asm1 asm2 asm3 asm4 asm5 # floppy

SRC_BOOT 	= ./arch/mips/boot/bootblock.S

SRC_ARCH	= ./arch/mips/kernel/entry.S ./arch/mips/kernel/syscall.S ./arch/mips/pmon/common.c 
SRC_DRIVER	= ./drivers/screen.c ./drivers/mac.c
SRC_INIT 	= ./init/main.c
SRC_INT		= ./kernel/irq/irq.c
#SRC_LOCK	= ./kernel/locking/lock.c
SRC_MM		= ./kernel/mm/memory.c
SRC_LOCK	= ./kernel/locking/lock.c 
SRC_SYNC    = ./kernel/locking/barrier.c ./kernel/locking/sem.c ./kernel/locking/cond.c
SRC_SCHED	= ./kernel/sched/sched.c ./kernel/sched/queue.c ./kernel/sched/time.c
SRC_SYSCALL	= ./kernel/syscall/syscall.c
#SRC_LIBS	= ./libs/string.c ./libs/printk.c
SRC_LIBS	= ./libs/string.c ./libs/printk.c ./libs/mailbox.c ./libs/scanf.c ./libs/bitmap.c

#SRC_TEST	= ./test/test.c
SRC_TEST	= ./test/test.c ./test/test_shell.c
#SRC_TEST2	= ./test/test_project2/test_scheduler1.c ./test/test_project2/test_scheduler2.c \
			  ./test/test_project2/test_lock2.c ./test/test_project2/test_sleep.c ./test/test_project2/test_timer.c 
#SRC_TEST2	= ./test/test_project2/test_scheduler1.c ./test/test_project2/test_scheduler2.c \
			  ./test/test_project2/test_lock1.c ./test/test_project2/test_sleep.c ./test/test_project2/test_timer.c 
#P3 : add SRC_TEST3
SRC_TEST3   = ./test/test_project3/test_barrier.c ./test/test_project3/test_condition.c \
			  ./test/test_project3/test_kill.c ./test/test_project3/test_sanguo.c ./test/test_project3/test_semaphore.c

SRC_TEST4_1	= ./test/test_project4_task1/process1.c ./test/test_project4_task1/process2.c
SRC_TEST4_2	= ./test/test_project4_task2/process1.c ./test/test_project4_task2/process2.c

SRC_TEST_NET = ./test/test_net/test_regs1.c  ./test/test_net/test_regs2.c ./test/test_net/test_regs3.c

SRC_IMAGE	= ./tools/createimage.c

SRC_FS		= ./kernel/fs/fs.c
SRC_TEST_FS = ./test/test_fs/test_fs.c

bootblock: $(SRC_BOOT)
	${CC} -G 0 -O2 -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800000 \
	      -N -o bootblock $(SRC_BOOT) -nostdlib -e main -Wl,-m -Wl,elf32ltsmip -T ld.script

##P3 ERROR VERSION
# main : $(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST2)
# 	${CC} -G 0 -O0 -Iinclude -Ilibs -Iarch/mips/include -Idrivers -Iinclude/os -Iinclude/sys -Itest -Itest/test_project2 -Itest/test_project3 \
# 	-fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800200 -N -o main \
# 	$(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_PROC) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST2) -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script		

##P3
# main : $(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST3)
# 	${CC} -G 0 -O0 -Iinclude -Ilibs -Iarch/mips/include -Idrivers -Iinclude/os -Iinclude/sys -Itest -Itest/test_project3 \
# 	-fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800200 -N -o main \
# 	$(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_PROC) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST3) -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script		

##P3 ERROR VERSION
# main : $(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST2) $(SRC_TEST3)
# 	${CC} -G 0 -O0 -Iinclude -Ilibs -Iarch/mips/include -Idrivers -Iinclude/os -Iinclude/sys -Itest -Itest/test_project2 -Itest/test_project3 \
# 	-fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800200 -N -o main \
# 	$(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_PROC) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST2) $(SRC_TEST3) -nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script		

##P4
# main : 	$(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SYNC) $(SRC_MM) $(SRC_SCHED) \
#         $(SRC_SYSCALL) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST3) $(SRC_TEST4_1) $(SRC_TEST4_2)
# 		${CC} -G 0 -O0 -Iinclude -Ilibs -Iarch/mips/include -Idrivers -Iinclude/os -Iinclude/sys \
# 		-Itest -Itest/test_project3 -Itest/test_project4_task1 -Itest/test_project4_task2\
# 		-fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800200 -N -o main \
# 		$(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SYNC) $(SRC_MM) $(SRC_SCHED) \
# 		$(SRC_SYSCALL) $(SRC_PROC) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST3) $(SRC_TEST4_1) $(SRC_TEST4_2) \
# 		-L. -lepmon \
# 		-nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script		

#P5-1
# main : 	$(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SYNC) $(SRC_MM) $(SRC_SCHED) \
#         $(SRC_SYSCALL) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST3) $(SRC_TEST4_1) $(SRC_TEST4_2) $(SRC_TEST_NET)
# 		${CC} -G 0 -O0 -Iinclude -Ilibs -Iarch/mips/include -Idrivers -Iinclude/os -Iinclude/sys \
# 		-Itest -Itest/test_project3 -Itest/test_project4_task1 -Itest/test_project4_task2 -Itest/test_net \
# 		-fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800200 -N -o main \
# 		$(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SYNC) $(SRC_MM) $(SRC_SCHED) \
# 		$(SRC_SYSCALL) $(SRC_PROC) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST3) $(SRC_TEST4_1) $(SRC_TEST4_2) $(SRC_TEST_NET)\
# 		-L. -lepmon \
# 		-nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script	

#P6
main : 	$(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SYNC) $(SRC_MM) $(SRC_SCHED) $(SRC_FS) \
        $(SRC_SYSCALL) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST3) $(SRC_TEST4_1) $(SRC_TEST4_2) $(SRC_TEST_NET) $(SRC_TEST_FS)
		${CC} -G 0 -O0 -Iinclude -Ilibs -Iarch/mips/include -Idrivers -Iinclude/os -Iinclude/sys \
		-Itest -Itest/test_project3 -Itest/test_project4_task1 -Itest/test_project4_task2 -Itest/test_net \
		-fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800200 -N -o main \
		$(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_SYNC) $(SRC_MM) $(SRC_SCHED) $(SRC_FS) \
		$(SRC_SYSCALL) $(SRC_PROC) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST3) $(SRC_TEST4_1) $(SRC_TEST4_2) $(SRC_TEST_NET) $(SRC_TEST_FS)\
		-L. -lepmon \
		-nostdlib -Wl,-m -Wl,elf32ltsmip -T ld.script	

createimage: $(SRC_IMAGE)
	gcc $(SRC_IMAGE) -o createimage

image: bootblock main
	./createimage --extended bootblock main

clean:
	rm -rf bootblock image createimage main *.o

floppy:
	sudo fdisk -l /dev/sdb
	sudo dd if=image of=/dev/sdb conv=notrunc

asm1:
	mipsel-linux-objdump -d main > kernel.txt
asm2:
	hexdump image > image.txt
asm3:
	hexdump disk > disk.txt
asm4:
	hexdump createimage > createimage.txt
asm5:
	hexdump bootblock > bootblock.txt
