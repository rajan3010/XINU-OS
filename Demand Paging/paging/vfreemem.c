/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

//extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
//very similar to freemem() {made changes to freemem itself} except using process' priv heapwhich is accessed using proctab[].vmemlist
	STATWORD ps;
        struct  mblock  *p, *q;
        unsigned top;

        struct mblock *process_mem_block=proctab[currpid].vmemlist;
        if (size==0 || ((unsigned)block)<((unsigned) &end))
                return(SYSERR);
        if (block < (4096*NBPG) || size == 0){
		kprintf("vfreemem: there is an issue with deallocating blocks\n");
		return SYSERR;
	}
        size = (unsigned)roundmb(size);
        disable(ps);
        for( p=process_mem_block->mnext,q= process_mem_block;
             p != (struct mblock *) NULL && p < block ;
             q=p,p=p->mnext )
                ;
        if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= process_mem_block) ||
            (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
                restore(ps);
                return(SYSERR);
        }
        if ( q!= process_mem_block && top == (unsigned)block )
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
	return(OK);
}

