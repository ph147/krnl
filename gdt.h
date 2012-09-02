#ifndef _GDT_H
#define _GDT_H

#include "types.h"

static void gdt_init(void);
static void set_gdt(int, uint32_t, uint32_t, uint8_t, uint8_t);
static void idt_init(void);
static void set_idt(int, uint32_t, uint16_t, uint8_t);
void gdt_idt_init(void);

struct gdt_descriptor_struct
{
    uint16_t size;
    uint32_t offset;
} __attribute__((packed));

typedef struct gdt_descriptor_struct gdt_descriptor_t;

struct gdt_struct
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t limit_flags;
    uint8_t base_high;
} __attribute__((packed));

typedef struct gdt_struct gdt_t;

struct idt_descriptor_struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

typedef struct idt_descriptor_struct idt_descriptor_t;

struct idt_struct
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

typedef struct idt_struct idt_t;

gdt_descriptor_t gdt_descr;
gdt_t gdt[5];

idt_descriptor_t idt_descr;
idt_t idt[256];

extern void gdt_flush(uint32_t);
extern void idt_flush(uint32_t);

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

#endif
