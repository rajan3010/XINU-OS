/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include<lab0.h>
#include<proc.h>

extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */
    struct	pentry *currpptr;
	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,4);
	}
    if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr,4);
	}
    return OK;
}
