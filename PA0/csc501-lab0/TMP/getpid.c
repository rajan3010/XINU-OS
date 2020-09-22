/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{	
	struct pentry *currpptr;	
	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,2);
	}
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr,2);
	}
	return(currpid);
}
