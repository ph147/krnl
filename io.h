#ifndef _IO_H
#define _IO_H

#include "types.h"

// TODO colors

#define COLS 80
#define ROWS 25
#define NORMAL_FONT 0x07
#define TXT_ADDRESS 0xb8000
#define STRING_BUF 12
#define TAB_WIDTH 8

extern uint8_t cursor_x;
extern uint8_t cursor_y;

void outb(uint16_t port, uint8_t value);
static void updateCursor(uint8_t c, uint8_t r);
static void advanceCursor(void);
void clearScreen(void);
static void scroll();
void putc(uint8_t c);
static void printHex(uint32_t n);
static void printDec(int32_t n);
static void printTab(void);
static void printNewline(void);
void puts(char *s);
void kprintf(char *s, ...);

#endif
