/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include<lab0.h>
#include<proc.h>

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
extern	struct	sentry	semaph[];
struct pentry *currpptr;

	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,10);
	}	
	if (isbadsem(sem) || semaph[sem].sstate==SFREE){
		if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 10);
		}
		return(SYSERR);
	}
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 10);
	}
	return(semaph[sem].semcnt);
}
