/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*vgetmem(nbytes)
	unsigned nbytes;
{
//almost same as getmem(){made changes to getmem()} except we have tpo get free mem blocl from process' private heap, thus use  mblock *memlist which is declared in proctab[] 
	STATWORD ps;

        struct  mblock  *p, *q, *leftover;
		int bs;
        disable(ps);
        //kprintf("\n vgetmem is entered\n");
        if (nbytes==0 || proctab[currpid].vmemlist->mnext== (struct mblock *) NULL ||nbytes>(MAXNPG*NBPG)) {

                kprintf("Out of bounds for vgetmem");
                restore(ps);
                return( (WORD *)SYSERR);
        }
        //shutdown();
        struct pentry *pptr= &proctab[currpid];
        nbytes = (unsigned int) roundmb(nbytes);
        //kprintf("\nnbytes=%d\n", nbytes);
         //p=pptr->vmemlist->mnext;
       //p != (struct mblock *) NULL;
        //shutdown();
        //for (q= &proctab[currpid].vmemlist,p=proctab[currpid].vmemlist->mnext ;p != (struct mblock *) NULL ;q=p,p=p->mnext){
        for (q= &pptr->vmemlist, p=pptr->vmemlist->mnext;p != (struct mblock *) NULL ;q=p,p=p->mnext){	
                        //shutdown();		
                        //kprintf("For loop entererd");
                        //kprintf("\np=%d\n", p);
                        //kprintf("\np_len initial %d", p->mlen);
                        //shutdown();
                if ( p->mlen == nbytes) {
                        //kprintf("\np_len is %d", p->mlen);
                        q->mnext = p->mnext;
                        //proctab[currpid].vmemlist=q;
                        //shutdown();
                        restore(ps);
                        return( (WORD *)p );
                } else if ( p->mlen > nbytes ) {
                        //kprintf("\np_len is %x", p->mlen);
                        leftover = (struct mblock *)( (unsigned)p + nbytes );
                        q->mnext = leftover;
                        leftover->mnext = p->mnext;
                        leftover->mlen = p->mlen - nbytes;
                        //kprintf("\nLeftover: %d\n", leftover->mlen);
                        //shutdown();
                        //proctab[currpid].vmemlist=leftover;
                        restore(ps);
                        return( (WORD *)p );
                }
		}
		//kprintf("\n Returning Error\n");
		//shutdown();
        restore(ps); 
        return( (WORD *)SYSERR );


}