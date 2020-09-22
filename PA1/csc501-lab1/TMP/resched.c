/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include<sched.h>
#include<math.h>

int procs, newproc, exp, max_goodness;

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	optr=&proctab[currpid];

	switch(sched_class){

		case EXPDISTSCHED:
			exp=(int)expdev(0.1);
			
			if(optr->pstate == PRCURR){
				optr->pstate=PRREADY;
				insert(currpid, rdyhead, optr->pprio);
			}

			if(exp>lastkey(rdytail)){
				currpid=getlast(rdytail);
			}
			else if(exp<firstkey(rdyhead)){
				currpid=getfirst(rdyhead);
			}
			else{
				procs=q[rdyhead].qnext;
				while(q[procs].qkey<=exp){
					procs=q[procs].qnext;
				}
				procs=q[procs].qprev;
				if(procs==NULLPROC){
					currpid=dequeue(NULLPROC);
				}
				else{
					currpid=dequeue(procs);
				}

			}

			nptr=&proctab[currpid];
			nptr->pstate=PRCURR;
			#ifdef	RTCLOCK
				preempt = QUANTUM;		/* reset preemption counter	*/
			#endif
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			break;
		
		case LINUXSCHED:

			max_goodness=0;

			/* reset the counter and goodness values at the end of each epoch or for initial NULL process*/
			if(preempt<=0||currpid==NULLPROC){
				optr->counter=0;
				optr->goodness=0;
			}

			/* Set the new quantum value for the old process*/
			optr->counter=preempt;

			/*Calculate the priority value for the old process to update the goodness*/
			int old_prio=optr->goodness-optr->counter;

			optr->goodness= old_prio+optr->counter;

			/*Iterate through the processess to pick the process with the highest goodness value*/
			procs=q[rdyhead].qnext;
			while(procs!=q[rdytail].qnext){
				if(proctab[procs].goodness>max_goodness){
					max_goodness=proctab[procs].goodness;
					newproc = procs;
				}
				procs=q[procs].qnext;
			}

			//New processess will be waiting till the next epoch since their goodness value and counter is initialized as 0 in initialize.c

			if((max_goodness==0)&&(optr->counter==0||optr->goodness==0||currpid==NULLPROC)){
				//Indicates start of a new epoch, where the process have exhausted their quantum and their goodness reset.
				//Also deals with NULL process
				//The processes in the ready list are initialized with their new quantum and goodness values.
				int i;
				struct pentry *p;
				for(i=0; i<NPROC; i++){
					p=&proctab[i];
					if(p->pstate!=PRFREE){
						p->counter=(p->counter)/2+p->pprio;
						p->goodness=p->counter+p->pprio;
					}
				}

				/* Insert current process into the ready queue like in default scheduling*/

				if (optr->pstate == PRCURR) {
					optr->pstate = PRREADY;
					insert(currpid,rdyhead,optr->pprio);
					}	
				currpid=dequeue(NULLPROC);
				nptr=&proctab[currpid];
				nptr->pstate= PRCURR;
				#ifdef	RTCLOCK
					preempt = QUANTUM;		/* reset preemption counter	*/
				#endif
	
				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
				return OK;
			}
			else if(optr->goodness>0&&optr->goodness>=max_goodness&&optr->pstate==PRCURR){
				//If current executing process has the maximum non-zero goodness value then no context switching is necessary
				//the greater than equal to in optr->goodness>=max_goodness ensures that round robin is executed in case of equal goodness
				//It removes the process from the end and during insertion it inserts in the beginning does ensuring round robin 
				preempt=optr->counter;
				return OK;

			}
			else if((max_goodness>0)&&(optr->pstate!=PRCURR||optr->goodness<max_goodness||optr->counter==0)){
				//If old process wants to step down though it has goodness and quantum
				//If the old process isn't the current process anymore
				//If old process has a goodness less than max goodness
				//If old process has exhausted it's quantum time

				if(optr->pstate==PRCURR){	
					optr->pstate=PRREADY;
					insert(currpid, rdyhead, optr->pprio);
				}
				
				currpid=dequeue(newproc);
				nptr = &proctab[currpid];
				nptr->pstate=PRCURR;
				preempt=nptr->counter;
				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

				return OK;
			}
			else{
				return SYSERR;
			}
			break;

		default:
			/* no switch needed if current process priority higher than next*/
			if ( ( (optr= &proctab[currpid])->pstate == PRCURR) && (lastkey(rdytail)<optr->pprio)) {
					return(OK);
					}
	
			/* force context switch */

			if (optr->pstate == PRCURR) {
				optr->pstate = PRREADY;
				insert(currpid,rdyhead,optr->pprio);
					}

			/* remove highest priority process at end of ready list */

			nptr = &proctab[ (currpid = getlast(rdytail)) ];
			nptr->pstate = PRCURR;		/* mark it currently running	*/
			#ifdef	RTCLOCK
				preempt = QUANTUM;		/* reset preemption counter	*/
			#endif
	
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
			/* The OLD process returns here when resumed. */
			break;
	}


	return OK;
}