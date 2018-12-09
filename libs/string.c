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

	if (*str1 == '\0' && *str2 == '\n')
	{
		return 2;
	}

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

