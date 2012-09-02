SOURCES=boot.o kernel.o io.o string.o debug.o gdt.o gdt_flush.o idt_flush.o interrupt.o isr.o

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector
LDFLAGS=-Tlink.ld

all: $(SOURCES) link
link:
	ld $(LDFLAGS) -o kernel.bin $(SOURCES)
