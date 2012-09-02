#include "types.h"

int strcmp(const char *str1, const char *str2)
{
    while (*str1 == *str2)
    {
        if (*str1 == '\0')
            return 0;
        ++str1;
        ++str2;
    }
    return *str1 < *str2 ? -1 : 1;
}

void *memchr(void *ptr, int value, size_t num)
{
    size_t i;
    for (i = 0; i < num; ++i)
    {
        if (*((unsigned char *) ptr) == (unsigned char) value)
            return ptr;
        ++ptr;
    }
    return NULL;
}

void *memset(void *ptr, int value, size_t num)
{
    size_t i;
    for (i = 0; i < num; ++i)
    {
        *((unsigned char *) ptr) = (unsigned char) value;
        ++ptr;
    }
}

void *memcpy(void *dest, const void *source, size_t num)
{
    size_t i;
    for (i = 0; i < num; ++i)
    {
        *((uint8_t *) dest) = *((uint8_t *) source);
        ++dest;
        ++source;
    }
}

size_t strlen(const char *source)
{
    size_t len = 0;
    while (*source++)
        ++len;
    return len;
}
