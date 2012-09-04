#include "io.h"
#include "gdt.h"
//#include "types.h"
//#include "string.h"
//#include "debug.h"

void init(void)
{
    gdt_idt_init();
    clearScreen();
    main();
}

void main(void)
{
    kprintf("%d\n", -1);
}
