#include "io.h"
#include "gdt.h"
//#include "types.h"
#include "string.h"
//#include "debug.h"

void init(void)
{
    gdt_idt_init();
    clearScreen();
    main();
}

void main(void)
{
    int i = 0x80000000;
    kprintf("%d\n", i);
}
