#include <stdio.h>

static unsigned long	*esp;
static unsigned long	*ebp;

void printtos()
{
    asm("movl %esp,esp");
	asm("movl %ebp,ebp");

    int c=0;

    kprintf("\ntop of the stack after entering the function is: 0x%08x", (ebp));
    kprintf("\ntop of stack before entering function: 0x%08x", (ebp+2));

    while(c<4 && esp<ebp){
        kprintf("\nThe content in stack pointer at [0x%08x] is 0x%08x", (esp+c), *(esp+c));
        c++;
    }

}