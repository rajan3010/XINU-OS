/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */
SYSCALL	setnok(int nok, int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr, *currpptr;

	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,14);
	}	

	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 14);
	}
		return(SYSERR);
	}
	pptr = &proctab[pid];
	pptr->pnxtkin = nok;
	restore(ps);

	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 14);
	}
	return(OK);
}
