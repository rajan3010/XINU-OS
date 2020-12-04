
#include <stdio.h>
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>

int releaseall(int numlocks, ...){
    STATWORD ps;
    disable(ps);
    int i=0;
    unsigned long *a;
    
    a=(unsigned long *)(&numlocks)+(numlocks);
    int release_lcks[NUM_LOCKS];
    //int i=0;
    int j;
    int numlocks_cpy;

    numlocks_cpy=numlocks;

    while(numlocks_cpy>0){
        release_lcks[i]=*a--;
        i++;
        numlocks_cpy--;
    }

    for(j=0; j<numlocks; j++){
        if(releaseLock(currpid, release_lcks[j])==-1){
            kprintf("\n Lock %d is not allocated for PID: %d and hence cannot be released ", release_lcks[j], currpid);
        }
    }

    restore(ps);
    return OK;
}
int releaseLock(int pid, int ldes){
    //kprintf("\release lock entered\n");
    //kprintf("\nReleasing lock: %d", ldes);
    struct lockentry *lock_ptr;
    lock_ptr=&lck_tab[ldes];

    struct pentry *pptr;
    pptr=&proctab[pid];

    int iter,j,max_wait_proc;
    int read_flag=0;

    if(lock_ptr->admitted_proc[pid]!=1){ /*This process does not hold the lock*/
        return -1;
    }
    //kprintf("\n checkpoint1 \n");
    //kprintf("\nPID: %d \n", pid);
    //kprintf("\nType: %d \n",lock_ptr->type[pid]);
    //kprintf("\nRead Type: %d \n",READ); 
    if(lock_ptr->type[pid]==READ){
           //kprintf("\n checkpoint2 \n"); 
            lock_ptr->lockstate==L_FREE; /* Release the lock */
            lock_ptr->type[pid]=NOTALLOC;
            lock_ptr->exec_state[pid]=RELEASED;
            lock_ptr->prio[pid]=-1;
            lock_ptr->admitted_proc[pid]=0;
            pptr->acq_locks[ldes]=0;
            if(pptr->wait_lock!=-1){ /*If the removed process is in the wait queue of another lock, restore the priority and recalculate the priorites for the other lock */
                if(pptr->pinh!=0){
                    lchprio(pid, pptr->pprio_bckup);
                    if(pptr->wait_lock!=-1){
                        rampPrio(pptr->wait_lock);
                        resched();
                    }
                }
            }

            max_wait_proc=getMaxWaitProc(ldes); /*Get the process with the maximum priority in the waiting queue */
            //kprintf("\nmax wait: %d\n", max_wait_proc);
            //kprintf("\n checkpoint3 \n");
            for(iter=0; iter< NPROC; iter++){    /*Check if there is another READ process in the lock */
            //kprintf("\nNPROC=%d", NPROC);
                if(lock_ptr->type[iter]==READ && lock_ptr->exec_state[iter]==EXEC && iter!=pid){
                    //kprintf("\n second read also there\n");
                    read_flag=1;
                    break;
                }
            }
    //shutdown();
    //kprintf("\n checkpoint4 \n");
            if(lock_ptr->wait_q_len){
            if(read_flag){  /*If there is another READ process*/
                if(lock_ptr->type[max_wait_proc]==READ){ /*Let the maximum priority waiting process access the lock only if it is a READ operation, since WRITE is exclusive*/
                    lock_ptr->lockstate=L_USED;
                    lock_ptr->type[max_wait_proc]=READ;
                    lock_ptr->exec_state[max_wait_proc]=EXEC;
                    lock_ptr->admitted_proc[max_wait_proc]=1;
                    proctab[max_wait_proc].acq_locks[ldes]=1;

                    lock_ptr->wait_time[max_wait_proc]=0;
                    lock_ptr->waiting_proc[max_wait_proc]=0;
                    lock_ptr->wait_q_len--;

                    /*pptr->pstate=PRREADY;*/
                    proctab[max_wait_proc].wait_lock=-1;
                    rampPrio(ldes); /*Ramp the priorities as the waiting list has now been changed*/
                    ready(max_wait_proc, RESCHNO);
                    resched();
                }
            }
            else{   /*If there isn't another READ operation in the lock, then give the lock to the next maximum priroity waiting process regardless of whether it is READ or WRITE*/
                lock_ptr->lockstate=L_USED;
                lock_ptr->exec_state[max_wait_proc]=EXEC;
                lock_ptr->admitted_proc[max_wait_proc]=1;
                proctab[max_wait_proc].acq_locks[ldes]=1;

                lock_ptr->wait_time[max_wait_proc]=0;
                lock_ptr->waiting_proc[max_wait_proc]=0;
                lock_ptr->wait_q_len--;

                /*pptr->pstate=PRREADY;*/
                proctab[max_wait_proc].wait_lock=-1;
                rampPrio(ldes);  /*Ramp the priorities as the waiting list has now been changed*/
                ready(max_wait_proc, RESCHNO);
                resched();

            }
            }
    }
    else if(lock_ptr->type[pid]==WRITE){
            //kprintf("\n checkpoint5 \n");
            lock_ptr->lockstate==L_FREE; /* Release the lock */
            lock_ptr->type[pid]=NOTALLOC;
            lock_ptr->exec_state[pid]=RELEASED;
            lock_ptr->prio[pid]=-1;
            lock_ptr->admitted_proc[pid]=0;
            pptr->acq_locks[ldes]=0;
            if(pptr->wait_lock!=-1){ /*If the removed process is in the wait queue of another lock, restore the priority and recalculate the priorites for the other lock */
                if(pptr->pinh!=0){
                    lchprio(pid, pptr->pprio_bckup);
                    rampPrio(pptr->wait_lock);
                    resched();
                }
            }
            //kprintf("\n checkpoint6 \n");
            max_wait_proc=getMaxWaitProc(ldes); /*Get the process with the maximum priority in the waiting queue */
            //kprintf("\n max_wait: %d \n", max_wait_proc);
/*After releasing the lock, give the lock to the next maximum priority process waiting in the queue*/
            lock_ptr->lockstate=L_USED;
            lock_ptr->exec_state[max_wait_proc]=EXEC;
            lock_ptr->admitted_proc[max_wait_proc]=1;
            proctab[max_wait_proc].acq_locks[ldes]=1;

            lock_ptr->wait_time[max_wait_proc]=0;
            lock_ptr->waiting_proc[max_wait_proc]=0;/*Remove the max prio process from the waiting queue*/
            lock_ptr->wait_q_len--;

            /*pptr->pstate=PRREADY;*/
            proctab[max_wait_proc].wait_lock=-1;
            rampPrio(ldes);  /*Ramp the priorities as the waiting list has now been changed*/
            ready(max_wait_proc, RESCHNO);
            resched();
    }
    //kprintf("\release lock left\n");
    return 0;
}