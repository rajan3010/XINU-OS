/* freemem.c - freemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <stdio.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  freemem  --  free a memory block, returning it to memlist
 *------------------------------------------------------------------------
 */
SYSCALL	freemem(struct mblock *block, unsigned size)
{
	STATWORD ps;    
	struct	mblock	*p, *q;
	struct pentry *currpptr;
	unsigned top;

	if(IS_SYSCALL_TRACE==1){
		currpptr=&proctab[currpid];
		syscall_count_timer_start(currpptr,0);
	}
	if (size==0 || (unsigned)block>(unsigned)maxaddr
	    || ((unsigned)block)<((unsigned) &end)){
		if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr,0);
	}
		return(SYSERR);}
	size = (unsigned)roundmb(size);
	disable(ps);
	for( p=memlist.mnext,q= &memlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &memlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		restore(ps);
		if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr,0);
	}
		return(SYSERR);
	}
	if ( q!= &memlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	restore(ps);
	if(IS_SYSCALL_TRACE==1){
		syscall_end_timer(currpptr,0);
	}
	return(OK);
}
