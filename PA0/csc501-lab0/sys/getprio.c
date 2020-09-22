/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
SYSCALL getprio(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr, *currpptr;
	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,3);
	}

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr,3);
	}
		return(SYSERR);
	}
	restore(ps);
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr,3);
	}
	return(pptr->pprio);
}
