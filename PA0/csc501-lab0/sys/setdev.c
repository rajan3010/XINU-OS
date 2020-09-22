/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	short	*nxtdev;
	struct pentry *currpptr;
	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,13);
	}	

	if (isbadpid(pid)){
		if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 13);
	}
		return(SYSERR);
	}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 13);
	}
	return(OK);
}
