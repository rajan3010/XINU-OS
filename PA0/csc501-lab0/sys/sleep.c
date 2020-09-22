/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep(int n)
{
	STATWORD ps;
	struct pentry *currpptr; 

	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,18);
	}	
   
	if (n<0 || clkruns==0){
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 18);
	}
		return(SYSERR);
		}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 18);
	}
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr, 18);
	}
	return(OK);
}
