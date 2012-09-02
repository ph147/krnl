#include "io.h"
#include "gdt.h"
//#include "types.h"
//#include "string.h"
//#include "debug.h"

void init(void)
{
    clearScreen();
    gdt_idt_init();
    main();
}

void main(void)
{
    int n = 2/0;
    puts("test");
}
