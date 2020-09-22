/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
extern long zfunction(long);
extern void printsegaddress();
extern void printtos();
extern void printprocstks(int);
extern void printcallsummary();
/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int prX;
void halt();

prch(c)
char c;
{
	int i;
	sleep(5);	
}
int main()
{	
	long input= 2864434397;
	long output=zfunction(input);

	kprintf("\n\nHello World, Xinu lives\n\n");
	kprintf("\nThe input is %x", input);
	kprintf("\n\nThe transformed number is %x\n\n", output);

	printsegaddress();
	printtos();
	printprocstks(15);
	
	syscallsummary_start();
	kprintf("starting system call");
	resume(prX = create(prch,2000,20,"proc X",1,'A'));
	
	sleep(10);
	syscallsummary_stop();
	printsyscallsummary();
	return 0;
}
