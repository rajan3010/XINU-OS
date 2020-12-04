/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	STATWORD(ps);
	disable(ps);

	int avail_bs;
	if(get_bsm(&avail_bs)==SYSERR||isbadhsize(hsize)){

			restore(ps);
			return SYSERR;
	}

	int pid=create(procaddr, ssize, priority, name, nargs, args);

	if(pid!=SYSERR){

		if(bsm_map(pid, BASE_VPNO, avail_bs, hsize)!=SYSERR){

			proctab[pid].store=avail_bs; /* keep a track of the bs_id for the process*/
			proctab[pid].vhpno=BASE_VPNO;   /* keep a track of the virtual page number for the process*/
			proctab[pid].vhpnpages=hsize;
			proctab[pid].vmemlist->mnext=4096*NBPG; /*Used during vgetmem*/

			//kprintf("\nBS ID at Vcreate %d\n", avail_bs);
			struct mblock *bs=BACKING_STORE_BASE+(avail_bs*BACKING_STORE_UNIT_SIZE);
			bs->mlen=NBPG*hsize;
			//kprintf("\nThe bs block mem size is : %d\n", bs->mlen);
			bs->mnext=NULL;
			//proctab[pid].vmemlist->mnext=bs; 

			bsm_tab[avail_bs].bs_isprivheap = 1; /* make the bs private so that it cannot be shared since it contains a virtual heap*/
			restore(ps);
			return pid;
		}
	}
	restore(ps);
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
