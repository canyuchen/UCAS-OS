/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *                    string library like clib("string.h")
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#ifndef INCLUDE_STRING_H_
#define INCLUDE_STRING_H_

#include "type.h"

void memcpy(uint8_t *dest, uint8_t *src, uint32_t len);

void memset(void *dest, uint8_t val, uint32_t len);

void bzero(void *dest, uint32_t len);

int strcmp(char *str1, char *str2);

char *strcpy(char *dest, char *src);

char *strcat(char *dest, char *src);

int strlen(char *src);

/* Reverse a string, Page 62 */
void reverse(char *s);

/* Convert an integer to an ASCII string, base 16 */
void itohex(uint32_t n, char *s);

/* Convert an integer to an ASCII string, Page 64 */
void itoa(uint32_t n, char *s);

/* Convert an ASCII string (like "234") to an integer */
uint32_t atoi(char *s);

inline int is_hex_char(char c);

int htoi(char *s);

uint32_t atoh(char *s);

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

char * strpbrk(const char * string,const char * control);

uint32_t strspn(const char *s, const char *accept);

char *strchr(const char *s, int c);

char *strrchr(const char *s, int c);

char* strtok(char* str_arr,const char* delimiters);

#endif
