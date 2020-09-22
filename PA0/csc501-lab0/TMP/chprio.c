/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr, *currpptr;
	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,1);
	}
	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(IS_SYSCALL_TRACE==1){
			syscall_end_timer(currpptr,1);
		}
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr,1);
	}
	return(newprio);
}
