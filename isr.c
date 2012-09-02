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
            puts("Unhandled interrupt: 0x");
            printHex(regs.int_no);
            putc('\n');
    }
    if (regs.int_no < 32)
        asm volatile("hlt");
}
