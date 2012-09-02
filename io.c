#include "io.h"
#include "types.h"

uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

void outb(uint16_t port, uint8_t value)
{
    asm("outb %1, %0" :: "d" (port), "a" (value));
}

static void updateCursor(uint8_t c, uint8_t r)
{
    uint16_t tmp;
    tmp = r * COLS + c;
    outb(0x3D4,14);
    outb(0x3D5,tmp >> 8);
    outb(0x3D4,15);
    outb(0x3D5,tmp);
}

void clearScreen(void)
{
    uint8_t i, j;
    uint16_t *video = (uint16_t *) TXT_ADDRESS;
    for (i = 0; i < ROWS; ++i)
        for (j = 0; j < COLS; ++j)
            video[i*COLS+j] = (NORMAL_FONT<<8) | ' ';
    updateCursor(cursor_x, cursor_y);
}

static void scroll()
{
    uint8_t i, j;
    uint16_t *video = (uint16_t *) TXT_ADDRESS;
    for (i = 0; i < COLS; ++i)
        video[(ROWS-1)*COLS+i] = (NORMAL_FONT<<8) | ' ';
    for (i = 0; i < ROWS-2; ++i)
    {
        for (j = 0; j < COLS; ++j)
        {
            video[i*COLS+j] = video[(i+1)*COLS+j];
        }
    }
}

static void advanceCursor()
{
    if (cursor_x == COLS-1)
        printNewline();
    else
        ++cursor_x;
    updateCursor(cursor_x, cursor_y);
}

static void printNewline()
{
    cursor_x = 0;
    if (cursor_y == ROWS-1)
        scroll();
    else
        ++cursor_y;
    updateCursor(cursor_x, cursor_y);
}

// TODO edge case
static void printTab()
{
    putc(' ');
    while ((cursor_x % TAB_WIDTH))
        putc(' ');
}

void putc(uint8_t c)
{
    uint16_t *video = ((uint16_t *) TXT_ADDRESS);
    uint16_t tmp = (NORMAL_FONT<<8) | c;

    if (c == '\n')
        printNewline();
    else if (c == '\t')
        printTab();
    else
    {
        video[cursor_y*COLS + cursor_x] = tmp;
        advanceCursor();
    }
}

void printDec(uint32_t n)
{
    uint8_t *s;
    uint8_t buf[STRING_BUF + 1];

    buf[STRING_BUF] = '\0';
    s = buf + STRING_BUF;

    if (!n)
    {
        puts("0");
        return;
    }
    while (n > 0)
    {
        --s;
        *s = '0' + (n % 10);
        n /= 10;
    }
    puts(s);
}

void printHex(uint32_t n)
{
    uint8_t *s;
    uint8_t tmp;
    uint8_t buf[STRING_BUF + 1];

    buf[STRING_BUF] = '\0';
    s = buf + STRING_BUF;

    if (!n)
    {
        puts("0");
        return;
    }
    while (n > 0)
    {
        --s;
        tmp = n % 16;
        if (tmp < 10)
            *s = '0' + tmp;
        else
            *s = 'a' + tmp - 10;
        n /= 16;
    }
    puts(s);
}

void puts(char *s)
{
    while (*s)
    {
        putc(*s);
        s++;
    }
}
