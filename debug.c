#include "debug.h"

// TODO
void infoRegisters()
{
    uint32_t reg;

    asm("nop" : "=a" (reg));
    kprintf("eax\t0x%x\n", reg);

    asm("nop" : "=b" (reg));
    kprintf("ebx\t0x%x\n", reg);

    asm("nop" : "=c" (reg));
    kprintf("ecx\t0x%x\n", reg);

    asm("nop" : "=d" (reg));
    kprintf("edx\t0x%x\n", reg);

    asm("pushfl");
    asm("movl (%%esp), %0" : "=r" (reg));
    asm("popfl");
    kprintf("eflags\t0x%x\n", reg);

    asm("movl %%esp, %0" : "=r" (reg));
    kprintf("esp\t0x%x\n", reg);

    asm("movl %%ebp, %0" : "=r" (reg));
    kprintf("ebp\t0x%x\n", reg);
}

static void printRaw(uint32_t n)
{
    while (n > 0)
    {
        putc(n & 0xff);
        n >>= 8;
    }
}

void cpuid()
{
    uint8_t i;
    uint32_t eax;
    uint32_t s[4];

    asm("cpuid"
            : "=b" (s[0]), "=d" (s[1]), "=c" (s[2])
            : "a" (0));

    for (i = 0; i < 3; ++i)
        printRaw(s[i]);

    asm("cpuid" : "=a" (eax) : "a" (0x80000000));
    if (eax < 0x80000004)
    {
        putc('\n');
        return;
    }

    putc(' ');
    for (eax = 0x80000002; eax < 0x80000005; ++eax)
    {
        asm("cpuid"
                : "=a" (s[0]), "=b" (s[1]), "=c" (s[2]), "=d" (s[3])
                : "a" (eax));
        infoRegisters();
        for (i = 0; i < 4; ++i)
            printRaw(s[i]);
    }
    putc('\n');
}
