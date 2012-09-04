#include "isr.h"
#include "types.h"

void isr_handler(registers_t regs)
{
    puts("Kernel panic: ");
    switch (regs.int_no)
    {
        case 0x00:
            puts("Division by zero.\n");
            break;
        default:
            kprintf("Unhandled interrupt: 0x%x", regs.int_no);
    }
    if (regs.int_no < 32)
        asm volatile("hlt");
}
