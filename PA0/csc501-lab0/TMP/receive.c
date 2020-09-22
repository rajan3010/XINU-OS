/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */
SYSCALL	receive()
{
	STATWORD ps;    
	struct	pentry	*pptr, *currpptr;
	WORD	msg;

	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,6);
	}		
	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);

	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 6);
	}
	return(msg);
}
