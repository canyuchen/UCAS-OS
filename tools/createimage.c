#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMAGE_FILE "./image"
#define ARGS "[--extended] [--vm] <bootblock> <executable-file> ..."

#define SECTOR_SIZE 512
#define OS_SIZE_LOC 2
#define BOOT_LOADER_SIG_OFFSET 0x1fe
#define BOOT_LOADER_SIG_1 0x55
#define BOOT_LOADER_SIG_2 0xaa
#define BOOT_MEM_LOC 0x7c00
#define OS_MEM_LOC 0x1000

/* structure to store command line options */
static struct
{
    int vm;
    int extended;
} options;

Elf32_Phdr * read_exec_file(FILE **exec_file_ptrr, char *filename_ptr, Elf32_Ehdr **ehdr_ptrr)
{
	*exec_file_ptrr = fopen(filename_ptr, "rb");
	if(*exec_file_ptrr == NULL)
	{
		printf("open fail\n");
		exit(1);
	}	
	Elf32_Phdr *phdr_ptr;
	phdr_ptr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr));
	*ehdr_ptrr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
	fread(*ehdr_ptrr, sizeof(Elf32_Ehdr), 1, *exec_file_ptrr);
	fseek(*exec_file_ptrr, (*ehdr_ptrr)->e_phoff, 0);
	fread(phdr_ptr, sizeof(Elf32_Phdr), 1, *exec_file_ptrr);
	return phdr_ptr;
}

void write_bootblock(FILE **image_file_ptrr, FILE *boot_file_ptr, Elf32_Ehdr *boot_ehdr_ptr, Elf32_Phdr *boot_phdr_ptr)
{
	int i = 1;
	void *buffer = calloc(512, sizeof(i));
	fseek(boot_file_ptr, boot_phdr_ptr->p_offset, 0);
	fread(buffer, boot_phdr_ptr->p_filesz, 1, boot_file_ptr);
	rewind(*image_file_ptrr);
	fwrite(buffer, 512, 1, *image_file_ptrr);
	fseek(*image_file_ptrr, 512L, 0);
	free(buffer);
	return;	
}

void write_kernel(FILE **image_file_ptrr, FILE *kernel_file_ptr, Elf32_Ehdr *kernel_ehdr_ptr, Elf32_Phdr *kernel_phdr_ptr)
{
	int i = 1;
	void *buffer = calloc(kernel_phdr_ptr->p_filesz, sizeof(i));
	fseek(kernel_file_ptr, kernel_phdr_ptr->p_offset, 0);
	fread(buffer, kernel_phdr_ptr->p_filesz, 1, kernel_file_ptr);
	fseek(*image_file_ptrr, 512L, 0);
	fwrite(buffer, kernel_phdr_ptr->p_filesz, 1, *image_file_ptrr);
	free(buffer);
	return;	
}

int count_kernel_sectors(Elf32_Ehdr *kernel_header_ptr, Elf32_Phdr *kernel_phdr_ptr)
{
	int i;
	i = kernel_header_ptr->e_phnum * kernel_phdr_ptr->p_filesz;
	if(i % 512 == 0)
		return i >> 9;
	else return (i >> 9) + 1;
}

void record_kernel_sectors(FILE **image_file_ptrr, Elf32_Ehdr *kernel_ehdr_ptr, Elf32_Phdr *kernel_phdr_ptr, int num_sec)
{
	//int os_size = kernel_ehdr_ptr->e_phnum * kernel_phdr_ptr->p_filesz;
	//fseek(*image_file_ptrr, 0x1f0L, 0);
	//fseek(*image_file_ptrr, 0x200L, 0);
	fseek(*image_file_ptrr, 0x1f0L, 0);
	//fputc(num_sec , *image_file_ptrr);
	fwrite(&num_sec, sizeof(int), 1, *image_file_ptrr);
	//fputc(itoa(os_size), *image_file_ptrr);
	return;
}

void extended_opt(Elf32_Phdr *boot_phdr_ptr, int k_phnum, Elf32_Phdr *kernel_phdr_ptr, int num_sec)
{	
	printf("Number of disk sectors:%d\n", 1 + num_sec);
	printf("bootblock image info\n");
	printf("\tsectors:  1\n");
	printf("\toffset of segment in the file: 0x%X\n", boot_phdr_ptr->p_offset);
	printf("\tthe image\'s vatural address of segment in memory: 0x%X\n", boot_phdr_ptr->p_vaddr);
	printf("\tthe file image size of segment:  0x%X\n", boot_phdr_ptr->p_filesz); 
	printf("\tthe memory image size of segment:  0x%X\n", boot_phdr_ptr->p_memsz);
	printf("\tthe size of write to the OS image:  0x%X\n", boot_phdr_ptr->p_filesz);
	printf("kernel image info\n");
	printf("\tsectors:  %d\n", num_sec);
	printf("\toffset of segment in the file: 0x%X\n", kernel_phdr_ptr->p_offset);
	printf("\tthe image\'s vatural address of segment in memory: 0x%X\n", kernel_phdr_ptr->p_vaddr);
	printf("\tthe file image size of segment:  0x%X\n", kernel_phdr_ptr->p_filesz); 
	printf("\tthe memory image size of segment:  0x%X\n", kernel_phdr_ptr->p_memsz);
	printf("\tthe size of write to the OS image:  0x%X\n", kernel_phdr_ptr->p_filesz);
	return;
}

int main(int argc, char *argv[])
{
	FILE **image_file_ptrr, *boot_file_ptr, *kernel_file_ptr;
	Elf32_Ehdr *boot_ehdr_ptr, *kernel_ehdr_ptr;
	Elf32_Phdr *boot_phdr_ptr, *kernel_phdr_ptr;
	int num_sec = 0;

	image_file_ptrr = malloc(sizeof(FILE *));
	if(argc != 4 && argv[1]!="--extended") return 1;

	boot_phdr_ptr    = read_exec_file(&boot_file_ptr, argv[2], &boot_ehdr_ptr);
	kernel_phdr_ptr  = read_exec_file(&kernel_file_ptr, argv[3], &kernel_ehdr_ptr);
	*image_file_ptrr = fopen("image", "wb");
	num_sec          = count_kernel_sectors(kernel_ehdr_ptr, kernel_phdr_ptr);

	write_bootblock(image_file_ptrr, boot_file_ptr, boot_ehdr_ptr, boot_phdr_ptr);
	write_kernel(image_file_ptrr, kernel_file_ptr, kernel_ehdr_ptr, kernel_phdr_ptr);

	record_kernel_sectors(image_file_ptrr, kernel_ehdr_ptr, kernel_phdr_ptr, num_sec);
	extended_opt(boot_phdr_ptr, kernel_ehdr_ptr->e_phnum, kernel_phdr_ptr, num_sec);

	fclose(*image_file_ptrr);
	fclose(boot_file_ptr);
	fclose(kernel_file_ptr);
	free(image_file_ptrr);

	return 0;
}