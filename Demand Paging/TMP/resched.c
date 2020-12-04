/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <paging.h>

unsigned long currSP;	/* REAL sp of current process */

/*------------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int	resched()
{
	STATWORD		PS;
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	register int i;
	unsigned int bs_id, pageth, fr, old_pid;
	unsigned long virtpgno;
	disable(PS);
	//kprintf("\nResched has entered");
	
	//shutdown();
	/* no switch needed if current process priority higher than next*/
	//shutdown();
	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		restore(PS);
		//kprintf("\n resched has left");
		//shutdown();
		return(OK);
	}
	
#ifdef STKCHK
	/* make sure current stack has room for ctsw */
	asm("movl	%esp, currSP");
	if (currSP - optr->plimit < 48) {
		kprintf("Bad SP current process, pid=%d (%s), lim=0x%lx, currently 0x%lx\n",
			currpid, optr->pname,
			(unsigned long) optr->plimit,
			(unsigned long) currSP);
		panic("current process stack overflow");
	}
	//shutdown();
#endif	

	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);

	}
	/* For convenience of shared memory access, write the pages of old process back into BS before rescheduling */
	for(fr=0; fr<NFRAMES; fr++){
		if(frm_tab[fr].fr_pid==currpid && frm_tab[fr].fr_type==FR_PAGE){
			virtpgno = frm_tab[fr].fr_vpno;

			if(bsm_lookup(currpid, virtpgno*NBPG, &bs_id, &pageth)!=SYSERR){
				if(!bsm_tab[bs_id].bs_isprivheap){
					//kprintf("\n Write operation is called for old process, frame: %d, PID:%d , PGOFF:%d , BS_ID:%d\n", fr, frm_tab[fr].fr_pid, pageth, bs_id);
					write_bs((char *)((FRAME0+fr)*NBPG), bs_id, pageth);
				}
			}
			else{
				//kprintf("\n BS lookup failed in resched.c while writing \n");
				kill(currpid);
			}
		}
	}
	/* remove highest priority process at end of ready list */

	// /kprintf("\n Old PID: %d", currpid);
	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
	//kprintf("\n New PID: %d", currpid);
	/* For convenience of shared processes, read the pages for the new process after reschedule from the BS*/
	for(fr=0; fr<NFRAMES; fr++){

		if(frm_tab[fr].fr_pid==currpid && frm_tab[fr].fr_type==FR_PAGE){
			//kprintf("\n Frame check passed \n");
			virtpgno = frm_tab[fr].fr_vpno;

			if(bsm_lookup(currpid, virtpgno*NBPG, &bs_id, &pageth)!=SYSERR){
				if(!bsm_tab[bs_id].bs_isprivheap){
					//kprintf("\n Read operation is called for new process, frame: %d, PID:%d , PGOFF:%d , BS_ID:%d\n", fr, frm_tab[fr].fr_pid, pageth, bs_id);
					read_bs((char *)((FRAME0+fr)*NBPG), bs_id, pageth);
				}
			}
			else{
				//kprintf("\n BS lookup faied in resched.c while reading \n");
				kill(currpid);
			}
		}
	}
#ifdef notdef
#ifdef	STKCHK
	if ( *( (int *)nptr->pbase  ) != MAGIC ) {
		kprintf("Bad magic pid=%d value=0x%lx, at 0x%lx\n",
			currpid,
			(unsigned long) *( (int *)nptr->pbase ),
			(unsigned long) nptr->pbase);
		panic("stack corrupted");
	}
	/*
	 * need ~16 longs of stack space below, so include that in check
	 *	below.
	 */
	if (nptr->pesp - nptr->plimit < 48) {
		kprintf("Bad SP pid=%d (%s), lim=0x%lx will be 0x%lx\n",
			currpid, nptr->pname,
			(unsigned long) nptr->plimit,
			(unsigned long) nptr->pesp);
		panic("stack overflow");
	}
#endif	/* STKCHK */
#endif	/* notdef */
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
#ifdef	DEBUG
	PrintSaved(nptr);
#endif
	
	ctxsw(&optr->pesp, optr->pirmask, &nptr->pesp, nptr->pirmask);

#ifdef	DEBUG
	PrintSaved(nptr);
#endif
	write_cr3(proctab[currpid].pdbr);	/* Write the pdbr value into the register befire resuming the old process */
	/* The OLD process returns here when resumed. */
	//kprintf("\nresched left");
	restore(PS);
	return OK;
}



#ifdef DEBUG
/* passed the pointer to the regs in the process entry */
PrintSaved(ptr)
    struct pentry *ptr;
{
    unsigned int i;
	
	kprintf("\nResched left through print");
    if (ptr->pname[0] != 'm') return;
    
    kprintf("\nSaved context listing for process '%s'\n",ptr->pname);
    for (i=0; i<8; ++i) {
	kprintf("     D%d: 0x%08lx	",i,(unsigned long) ptr->pregs[i]);
	kprintf("A%d: 0x%08lx\n",i,(unsigned long) ptr->pregs[i+8]);
    }
    kprintf("         PC: 0x%lx",(unsigned long) ptr->pregs[PC]);
    kprintf("  SP: 0x%lx",(unsigned long) ptr->pregs[SSP]);
    kprintf("  PS: 0x%lx\n",(unsigned long) ptr->pregs[PS]);
}
#endif


