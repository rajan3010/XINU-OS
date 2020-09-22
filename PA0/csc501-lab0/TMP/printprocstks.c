#include <stdio.h>
#include <conf.h>
#include <kernel.h>
#include<proc.h>
#include<lab0.h>
unsigned long *esp;

void printprocstks(int priority)
{
    struct pentry *process;
    int i;
    kprintf("\n\nTask print process stack");
    for(i=0;i< NPROC;i++){
        process=&proctab[i];

        if(process->pprio>priority && process->pstate != PRFREE){
            if(process->pstate==PRCURR){
                asm("mov %esp, esp");
                kprintf("\n************Currently executing process****************\n");  
                kprintf("\nCurrent stack pointer: 0x%x\n", esp);
            }
            kprintf("******************************************\n");
            kprintf("\nprocess name is :%s",process->pname);
            kprintf("\nprocess id is : %d", i);
            kprintf("\nprocess priority is: %d",process->pprio);     
            kprintf("\nThe stack base is :0x%x",process->pbase);
            kprintf("\nThe stack size of process is :%d",process->pstklen);
            kprintf("\nThe stack limit of the process is :0x%x", process->plimit);
            kprintf("\nThe saved stack pointer of the process is :0x%x", process->pesp);
            kprintf("\n******************************************\n"); 
        }

    }

}