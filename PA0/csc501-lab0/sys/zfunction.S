        .text
        .global zfunction

zfunction:

        pushl %ebp
        movl %esp, %ebp

        movl 8(%ebp), %eax
        andl $0xfffc00ff, %eax
        sarl $8, %eax
        orl $0xff000000, %eax
        popl %ebp

        ret




        