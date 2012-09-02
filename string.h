#ifndef _STRING_H
#define _STRING_H

#include "types.h"

int strcmp(const char *str1, const char *str2);
void *memchr(void *ptr, int value, size_t num);
void *memset(void *ptr, int value, size_t num);
void *memcpy(void *dest, const void *source, size_t num);
size_t strlen(const char *source);

#endif
