#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int my_printf(char *fmt,...)
{
    char *p;
    va_list ap;
    
    //获得第一个实际参数的起始地址
    va_start(ap,fmt);
    
    //分析fmt指向的字符串
    for(p = fmt; *p;p ++)
    {
        if(*p == '%')
        {
            p ++;
            switch(*p)
            {
            //整形十进制数
            case 'd':
                printf("%d",va_arg(ap,int));
                break;

            //字符
            case 'c':
                //变参传递char类型变量时，编译器在
                //编译的时候将其提升为int类型
                printf("%c",va_arg(ap,int));
                break;

            //字符串
            case 's':
                //地址占用4个字节
                printf("%s",(char *)va_arg(ap,int));
                break;

            //浮点数
            case 'f':
                //变参传递float类型变量时，编译器在
                //编译的时候将其提升为double类型
                printf("%f",va_arg(ap,double));
                break;
            // %
            case '%':
                putchar('%');
                break;
            }
        
        }else{
            putchar(*p);
        }
    }

    //将ap赋值为NULL
    va_end(ap);

    return 0;
}

int main(int argc, const char *argv[])
{
    int a = 123;
    char b = 'c';
    float c = 12.38;
    char buf[] = "hello my_printf";

    my_printf("a = %d b = %c buf = %s c = %f.n",a,b,buf,c);

    return 0;
}