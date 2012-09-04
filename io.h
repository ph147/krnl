#ifndef _IO_H
#define _IO_H

#include "types.h"

#define COLOR_BLACK   0x0
#define COLOR_BLUE    0x1
#define COLOR_GREEN   0x2
#define COLOR_CYAN    0x3
#define COLOR_RED     0x4
#define COLOR_MAGENTA 0x5
#define COLOR_BROWN   0x6
#define COLOR_GRAY    0x7

#define COLOR_LIGHT   0x8

#define COLS 80
#define ROWS 25
#define NORMAL_FONT 0x07
#define TXT_ADDRESS 0xb8000
#define STRING_BUF 12
#define TAB_WIDTH 8

extern uint8_t cursor_x;
extern uint8_t cursor_y;
extern uint8_t active_color;

void outb(uint16_t port, uint8_t value);
static void updateCursor(uint8_t c, uint8_t r);
static void advanceCursor(void);
void clear(void);
static void scroll();
void putc(uint8_t c);
static void printHex(uint32_t n);
static void printDec(int32_t n);
static void printTab(void);
static void printNewline(void);
void puts(char *s);
void kprintf(char *s, ...);
void setfg(uint8_t color);
void setbg(uint8_t color);
void resetcolor(void);

#endif
