/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
	int wait_ldes;
	int i;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}

	for(i=0; i<NUM_LOCKS; i++){
		if(pptr->acq_locks[i]){	/*If the process has acquired any lock release it*/
			releaseLock(pid, i);
		}
	}
	//kprintf("\nCheckpoint KIll\n");
	//kprintf("\nWait Lock: %d\n",pptr->wait_lock);
	if(pptr->wait_lock!=-1){	/*If the process is waiting on any lock removeit from the queue and ramp the prioirites*/
		//kprintf("\nKILL.C: process waitlist check entered\n");
		wait_ldes=pptr->wait_lock;
		lck_tab[wait_ldes].wait_time[pid]=0;
		lck_tab[wait_ldes].waiting_proc[pid]=0;
		lck_tab[wait_ldes].wait_q_len--;

		rampPrio(wait_ldes);
		pptr->wait_lock=-1;
	}
	restore(ps);
	return(OK);
}
