#include "gdt.h"
//#include "string.h"

static void gdt_init(void)
{
    gdt_descr.size = sizeof(gdt_t)*5 - 1;
    gdt_descr.offset = (uint32_t) &gdt;
    set_gdt(0, 0, 0, 0, 0); // Null segment
    set_gdt(1, 0, 0xffffffff, 0x9a, 0xc0); // Kernel code
    set_gdt(2, 0, 0xffffffff, 0x92, 0xc0); // Kernel data
    set_gdt(3, 0, 0xffffffff, 0xfa, 0xc0); // User code
    set_gdt(4, 0, 0xffffffff, 0xf2, 0xc0); // User data
    gdt_flush((uint32_t) &gdt_descr);
}

static void
set_gdt(int n, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt[n].limit_low = limit & 0xffff;
    gdt[n].base_low = base & 0xffff;
    gdt[n].base_mid = (base >> 16) & 0xff;
    gdt[n].access = access;
    gdt[n].limit_flags = (limit >> 16) & 0xf;
    gdt[n].limit_flags |= flags & 0xf0;
    gdt[n].base_high = (base >> 24) & 0xff;
}

static void idt_init(void)
{
    idt_descr.limit = sizeof(idt_t)*256 - 1;
    idt_descr.base = (uint32_t) &idt;
    set_idt(0, (uint32_t) isr0, 0x08, 0x8e);
    set_idt( 1, (uint32_t)  isr1, 0x08, 0x8e);
    set_idt( 2, (uint32_t)  isr2, 0x08, 0x8e);
    set_idt( 3, (uint32_t)  isr3, 0x08, 0x8e);
    set_idt( 4, (uint32_t)  isr4, 0x08, 0x8e);
    set_idt( 5, (uint32_t)  isr5, 0x08, 0x8e);
    set_idt( 6, (uint32_t)  isr6, 0x08, 0x8e);
    set_idt( 7, (uint32_t)  isr7, 0x08, 0x8e);
    set_idt( 8, (uint32_t)  isr8, 0x08, 0x8e);
    set_idt( 9, (uint32_t)  isr9, 0x08, 0x8e);
    set_idt(10, (uint32_t) isr10, 0x08, 0x8e);
    set_idt(11, (uint32_t) isr11, 0x08, 0x8e);
    set_idt(12, (uint32_t) isr12, 0x08, 0x8e);
    set_idt(13, (uint32_t) isr13, 0x08, 0x8e);
    set_idt(14, (uint32_t) isr14, 0x08, 0x8e);
    set_idt(15, (uint32_t) isr15, 0x08, 0x8e);
    set_idt(16, (uint32_t) isr16, 0x08, 0x8e);
    set_idt(17, (uint32_t) isr17, 0x08, 0x8e);
    set_idt(18, (uint32_t) isr18, 0x08, 0x8e);
    set_idt(19, (uint32_t) isr19, 0x08, 0x8e);
    set_idt(20, (uint32_t) isr20, 0x08, 0x8e);
    set_idt(21, (uint32_t) isr21, 0x08, 0x8e);
    set_idt(22, (uint32_t) isr22, 0x08, 0x8e);
    set_idt(23, (uint32_t) isr23, 0x08, 0x8e);
    set_idt(24, (uint32_t) isr24, 0x08, 0x8e);
    set_idt(25, (uint32_t) isr25, 0x08, 0x8e);
    set_idt(26, (uint32_t) isr26, 0x08, 0x8e);
    set_idt(27, (uint32_t) isr27, 0x08, 0x8e);
    set_idt(28, (uint32_t) isr28, 0x08, 0x8e);
    set_idt(29, (uint32_t) isr29, 0x08, 0x8e);
    set_idt(30, (uint32_t) isr30, 0x08, 0x8e);
    set_idt(31, (uint32_t) isr31, 0x08, 0x8e);
    idt_flush((uint32_t) &idt_descr);
}

static void set_idt(int n, uint32_t offset, uint16_t sel, uint8_t flags)
{
    idt[n].offset_low = offset & 0xffff;
    idt[n].selector = sel;
    idt[n].zero = 0;
    idt[n].type_attr = flags;
    idt[n].offset_high = (offset >> 16) & 0xffff;
}

void gdt_idt_init(void)
{
    gdt_init();
    idt_init();
}
