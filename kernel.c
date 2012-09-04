#include "io.h"
#include "gdt.h"
//#include "types.h"
//#include "string.h"
//#include "debug.h"

void init(void)
{
    gdt_idt_init();
    clear();
    main();
}

void main(void)
{
    setfg(COLOR_GREEN | COLOR_LIGHT);
    setbg(COLOR_RED | COLOR_LIGHT);
    kprintf("test\n");
    resetcolor();
    kprintf("test\n");
}
