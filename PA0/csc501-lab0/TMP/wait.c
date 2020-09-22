/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL	wait(int sem)
{
	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr, *currpptr;

	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,26);
	}	
	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 26);
	}
		return(SYSERR);
	}
	
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 26);
	}
		return pptr->pwaitret;
	}
	restore(ps);
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 26);
	}
	return(OK);
}
