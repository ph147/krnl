.extern isr_handler

.macro isr_noerr no
    .global isr\no
    isr\no:
        cli
        pushl $0
        pushl $\no
        jmp isr_stub
.endm

.macro isr_err no
    .global isr\no
    isr\no:
        cli
# TODO ????
        pushl $0
# TODO ????
        pushl $\no
        jmp isr_stub
.endm

isr_noerr 0
isr_noerr 1
isr_noerr 2
isr_noerr 3
isr_noerr 4
isr_noerr 5
isr_noerr 6
isr_noerr 7
isr_err 8
isr_noerr 9
isr_err 10
isr_err 11
isr_err 12
isr_err 13
isr_err 14
isr_noerr 15
isr_noerr 16
isr_noerr 17
isr_noerr 18
isr_noerr 19
isr_noerr 20
isr_noerr 21
isr_noerr 22
isr_noerr 23
isr_noerr 24
isr_noerr 25
isr_noerr 26
isr_noerr 27
isr_noerr 28
isr_noerr 29
isr_noerr 30
isr_noerr 31

isr_stub:
    pusha
    movw %ds, %ax

    push %eax
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    call isr_handler

    pop %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    popa
    add $8, %esp
    sti
    iret
