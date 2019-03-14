#include "scanf.h"
#include "mm.h"

static char read_uart_ch_s(void)
{
    char ch = 0;
    unsigned char *read_port = (unsigned char *)(0xbfe48000 + 0x00);
    unsigned char *stat_port = (unsigned char *)(0xbfe48000 + 0x05);

    while ((*stat_port & 0x01))
    {
        ch = *read_port;
    }
    return ch;
}

#define INPUT_BUFFER_MAX_LENGTH 1000

static char Buffer_s[INPUT_BUFFER_MAX_LENGTH];

InputBuffer_s_t inputBuffer_s;

void init_InputBuffer_s(InputBuffer_s_t *p)
{
    p->buffer = Buffer_s;
    p->buffer_length = INPUT_BUFFER_MAX_LENGTH;
    p->input_length = 0;
    p->pointer = 0;
}

/*
int do_scanf(int *mem)
{
	//TODO:Use read_uart_ch() to complete scanf(), read input as a hex number.
	//Extending function parameters to (const char *fmt, ...) as printf is recommended but not required.
    InputBuffer_s_t *inputBuffer_ptr = &inputBuffer_s;
    init_InputBuffer_s(inputBuffer_ptr);

    while (1)
    {
        char ch = read_uart_ch_s();

		int i = 0, k = 0;

        if(ch != 0){
            printk("%c", ch);        
            *(inputBuffer_ptr->buffer + i) = ch;
            inputBuffer_ptr->input_length++;
            i++;
            if(ch == '\r'){
                printk("%c", *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer)); 
                printk("%c", *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1)); 
                printk("%d", atoi(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 2)); 

                if(*(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer) == '0' 
                && *(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 1) == 'x'
                && (k = atoi(inputBuffer_ptr->buffer + inputBuffer_ptr->pointer + 2)) >= 0
                && k <= VM_SIZE ){
                    *mem = k;
                    // break;
                    return '\r';
                }
            }            
        }
    }
}
*/
InputBuffer_s_t *inputBuffer_s_ptr = &inputBuffer_s;

static int index = 0;

int do_scanf(int *mem)
{
	//TODO:Use read_uart_ch() to complete scanf(), read input as a hex number.
	//Extending function parameters to (const char *fmt, ...) as printf is recommended but not required.
    // InputBuffer_s_t *inputBuffer_s_ptr = &inputBuffer_s;
    // static int i = 0;
    // init_InputBuffer_s(inputBuffer_s_ptr);

    while (1)
    {
        char ch = read_uart_ch_s();

		int k = 0;

        if(ch != 0){
            // printk("%c", ch);        
            *(inputBuffer_s_ptr->buffer + index) = ch;
            inputBuffer_s_ptr->input_length++;
            index++;
            
            if(ch == '\r'){
                // printk("%c", *(inputBuffer_s_ptr->buffer + inputBuffer_s_ptr->pointer)); 
                // printk("%c", *(inputBuffer_s_ptr->buffer + inputBuffer_s_ptr->pointer + 1)); 
                // printk("%d", atoi(inputBuffer_s_ptr->buffer + inputBuffer_s_ptr->pointer + 2)); 

                if(*(inputBuffer_s_ptr->buffer + inputBuffer_s_ptr->pointer) == '0' 
                && *(inputBuffer_s_ptr->buffer + inputBuffer_s_ptr->pointer + 1) == 'x'
                && (k = atoh(inputBuffer_s_ptr->buffer + inputBuffer_s_ptr->pointer + 2)) >= 0
                && k <= VM_SIZE ){
                    *mem = k;
                }
                inputBuffer_s_ptr->pointer = index;
            }
            return ch;            
        }
    }
}

// /*
// static void scanf(const char *fmt, ...)
// {

// }
// */

//????
/*
char buffer[100];
int inline_position = 0;

static char mread_uart_ch(void)
{
    char ch = 0;
    unsigned char *read_port = (unsigned char *)(0xbfe48000 + 0x00);
    unsigned char *stat_port = (unsigned char *)(0xbfe48000 + 0x05);

    while ((*stat_port & 0x01))
    {
        ch = *read_port;
    }
    return ch;
}

// void gethex(int *mem)
int do_scanf(int *mem)
{
    //Use read_uart_ch() to complete scanf(), read input as a hex number.
    //Extending function parameters to (const char *fmt, ...) as printf is recommended but not required.

    // read command from UART port
    // char buffer[100];
    // int inline_position=0;

    // vt100_move_cursor(1,1);
    // printk("                                            ");
    // vt100_move_cursor(1,1);
    // printk("gethex:");
    
    memset(buffer, 0, 100);

    while (1)
    {
        char ch = mread_uart_ch();
        if (!ch)
            continue;

        // if (ch == 127) //in qemu sim
        // {
        //     if (inline_position > 0)
        //     {
        //         inline_position--;
        //         // printk("\b");
        //         break;
        //     }
        // }
        // else if (ch == 8) //on board
        // {
        //     if (inline_position > 0)
        //     {
        //         inline_position--;
        //         // printk("\b");
        //         break;
        //     }
        // }
        else if (ch != 13) //
        {
            buffer[inline_position++]=ch;
            printk("%c",ch);
            break;
        }
        else //ch==13
        {
            buffer[inline_position++]='\r';
            inline_position = 0;
            break;
        }
    }

    *mem=htoi(buffer);
    return buffer[inline_position];
}
*/
