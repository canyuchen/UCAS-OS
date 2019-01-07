#include "string.h"

int strlen(char *src)
{
	int i;
	for (i = 0; src[i] != '\0'; i++)
	{
	}
	return i;
}

void memcpy(uint8_t *dest, uint8_t *src, uint32_t len)
{
	for (; len != 0; len--)
	{
		*dest++ = *src++;
	}
}

void memset(void *dest, uint8_t val, uint32_t len)
{
	uint8_t *dst = (uint8_t *)dest;

	for (; len != 0; len--)
	{
		*dst++ = val;
	}
}

void bzero(void *dest, uint32_t len)
{
	memset(dest, 0, len);
}

int strcmp(char *str1, char *str2)
{
/*
	while (*str1 && *str2 && (*str1++ == *str2++))
	{
	};
*/
	while (*str1 && *str2 && (*str1 == *str2))
	{
		str1++;
		str2++;
	};

	if (*str1 == '\0' && *str2 == '\0')
	{
		return 0;
	}
/*
	if (*str1 == '\0' && *str2 == '\n')
	{
		return 2;
	}
*/
	if (*str1 == '\0')
	{
		return -1;
	}

	return 1;
}

char *strcpy(char *dest, char *src)
{
	char *tmp = dest;

	while (*src)
	{
		*dest++ = *src++;
	}

	*dest = '\0';

	return tmp;
}

/* Reverse a string, Page 62 */
void reverse(char *s)
{
    int c, i, j;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* Convert an integer to an ASCII string, base 16 */
void itohex(uint32_t n, char *s)
{
    int i, d;

    i = 0;
    do {
        d = n % 16;
        if (d < 10)
            s[i++] = d + '0';
        else
            s[i++] = d - 10 + 'a';
    } while ((n /= 16) > 0);
    s[i++] = 0;
    reverse(s);
}

/* Convert an integer to an ASCII string, Page 64 */
void itoa(uint32_t n, char *s)
{
    int i;

    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    s[i++] = 0;
    reverse(s);
}

/* Convert an ASCII string (like "234") to an integer */
uint32_t atoi(char *s)
{
    int n;
    for (n = 0; *s >= '0' && *s <= '9'; n = n * 10 + *s++ - '0');
    return n;
}

uint32_t atoh(char *s)
{
    int n;
    for (n = 0; (*s >= '0' && *s <= '9') || (*s >= 'a' && *s <= 'f'); ){
		if(*s >= '0' && *s <= '9'){
			n = n * 16 + *s++ - '0';
		}
		else if(*s >= 'a' && *s <= 'f'){
			n = n * 16 + *s++ - 'a';
		}
	}
    return n;
}



inline int is_hex_char(char c)
{
	return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f' );
}

int htoi(char *s)
{
  int n;

  n = 0;
  while(is_hex_char(*s))
  {
	if(('0' <= *s && *s <= '9') )
	{
		n = n*16 + *s++ - '0';
	}else//('a' <= c && c <= 'f' )
	{
		n = n*16 + *s++ - 'a' + 10;		
	}
  }
  return n;
}

/*
*copyright@nciaebupt 转载请注明出处
*原型：char *strpbrk(const char *s1, const char *s2);
*用法：#include <string.h>
*功能：在字符串s1中寻找字符串s2中任何一个字符相匹配的第一个字符的位置，
*   空字符NULL不包括在内。
*说明：返回指向s1中第一个相匹配的字符的指针，如果没有匹配字符则返回空指针NULL。
*使用C函数库中的strpbrk
*/
/*
#include <cstdio>
#include <cstring>
 
int main(int args,char ** argv)
{
    char str[] = "This is a sample string";
    char keys[] = "aeiou";
    printf("Vowels in '%s' : ",str);
    char * pch;
    pch = strpbrk(str,keys);
    while(pch != NULL)
    {
        printf("%c ",*pch);
        pch = strpbrk(pch + 1,keys);
    }
    getchar();
    return 0;
}
*/

/*
*copyright@nciaebupt 转载请注明出处
*原型：char *strpbrk(const char *s1, const char *s2);
*用法：#include <string.h>
*功能：在字符串s1中寻找字符串s2中任何一个字符相匹配的第一个字符的位置，
*   空字符NULL不包括在内。
*说明：返回指向s1中第一个相匹配的字符的指针，如果没有匹配字符则返回空指针NULL。
*自己实现strpbrk
*/
/*
#include <cstdio>
#include <cstring>
*/

char * strpbrk(const char * string,const char * control)
{
    const unsigned char *str = (const unsigned char *)string;
    const unsigned char *ctrl = (const unsigned char *)control;
    unsigned char map[32];
    int count;
    /*clear the map*/
    memset(map,0,32*sizeof(unsigned char));
    /*set bit in the control map*/
    while(*ctrl)
    {
        map[*ctrl >> 3] |= (0x01 << (*ctrl & 7));
        ctrl++;
    }
    /*search control in str*/
    while(*str)
    {
        if((map[*str >> 3] & (1 << (*str & 7))))
            return((char *)str);
        str++;
    }
    return NULL;
 
}

/*
int main(int args,char ** argv)
{
    char str[] = "This is a sample string";
    char keys[] = "aeiou";
    printf("Vowels in '%s' : ",str);
    char * pch;
    pch = strpbrk(str,keys);
    while(pch != NULL)
    {
        printf("%c ",*pch);
        pch = _strpbrk(pch + 1,keys);
    }
    getchar();
    return 0;
}
*/

uint32_t strspn(const char *s, const char *accept)
{
    const char *p = s;
    const char *a;
    uint32_t count = 0;

    for (; *p != '\0'; ++p) {
        for (a = accept; *a != '\0'; ++a) {
            if (*p == *a)
                break;
        }
        if (*a == '\0')
            return count;
        ++count;
    }
    return count;
}

char *strchr(const char *s, int c)
{
    if(s == NULL)
    {
        return NULL;
    }

    while(*s != '\0')
    {
        if(*s == (char)c )
        {
            return (char *)s;
        }
        s++;
    }
    return NULL;
}

char *strrchr(const char *s, int c)
{
    if(s == NULL)
    {
        return NULL;
    }

    char *p_char = NULL;
    while(*s != '\0')
    {
        if(*s == (char)c)
        {
            p_char = (char *)s;
        }
        s++;
    }

    return p_char;
}

/*
#include<stdio.h>
#include<string.h>
*/
//根据函数原型实现strtok()函数
char* myStrtok_origin(char* str_arr,const char* delimiters,char **temp_str)
{
    //定义一个指针来指向待分解串
    char*b_temp;
    /*
    * 1、判断参数str_arr是否为空，如果是NULL就以传递进来的temp_str作为起始位置；
    * 若不是NULL，则以str为起始位置开始切分。
    */
    if(str_arr == NULL)
    {
        str_arr =*temp_str;
    }
    //2、跳过待分解字符串
    //扫描delimiters字符开始的所有分解符
    str_arr += strspn(str_arr, delimiters);
    //3、判断当前待分解的位置是否为'\0'，若是则返回NULL，否则继续
    if(*str_arr =='\0')
    {
        return NULL;
    }
    /*
    * 4、保存当前的待分解串的指针b_temp，调用strpbrk()在b_temp中找分解符，
    * 如果找不到，则将temp_str赋值为待分解字符串末尾部'\0'的位置，
    * b_temp没有发生变化；若找到则将分解符所在位置赋值为'\0',
    * b_temp相当于被截断了，temp_str指向分解符的下一位置。
    */
    b_temp = str_arr;
    str_arr = strpbrk(str_arr, delimiters);
    if(str_arr == NULL)
    {
        *temp_str = strchr(b_temp,'\0');
    }
    else
    {
        *str_arr ='\0';
        *temp_str = str_arr +1;
    }
    //5、函数最后部分无论找没找到分解符，都将b_temp返回。
    return b_temp;
}

//使用myStrtok来简化myStrtok_origin函数
/*
char* myStrtok(char* str_arr,const char* delimiters)
{
    static char *last;
    return myStrtok_origin(str_arr, delimiters,&last);
}
*/

char* strtok(char* str_arr,const char* delimiters)
{
    static char *last;
    return myStrtok_origin(str_arr, delimiters,&last);
}

/*
int main(void)
{
    char buf[]="hello@boy@this@is@heima";
    //1、使用myStrtok_origin()函数
    char*temp_str = NULL;
    char*str = myStrtok_origin(buf,"@",&temp_str);
    while(str)
    {
        printf("%s ",str);
        str = myStrtok_origin(NULL,"@",&temp_str);
    }
    //2、使用myStrtok()函数
    char*str1 = myStrtok(buf,"@");
    while(str1)
    {
        printf("%s ",str1);
        str1 = myStrtok(NULL,"@");
    }
    return0;
}
*/
