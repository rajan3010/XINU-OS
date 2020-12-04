#include<stdio.h>
#include<conf.h>
#include<i386.h>
#include<kernel.h>
#include<lock.h>
#include<proc.h>

int lock(int ldes1, int type, int priority){
    STATWORD ps;
    disable(ps);

    struct lockentry *lock_ptr;
    lock_ptr=&lck_tab[ldes1];

    struct pentry *pptr;
    pptr=&proctab[currpid];

    if(isbadlock(ldes1)||lock_ptr->lockstate==L_DELETED){
        kprintf("\nLOCK.C: Either a badlock, or lock deleted\n");
        restore(ps);
        return SYSERR;
    }
    if(type==READ){

        if(lock_ptr->lockstate==L_FREE){    /*If the lock is available then give it to the reader. peace out*/

            lock_ptr->lockstate=L_USED;
            lock_ptr->type[currpid]=READ;
            lock_ptr->exec_state[currpid]=EXEC;
            lock_ptr->prio[currpid]=priority;
            lock_ptr->admitted_proc[currpid]=1;
            pptr->acq_locks[ldes1]=1;

            restore(ps);
            return OK;

        }
        else if(isWrite(ldes1)){        /*If the lock is already held by a write lock , then the READ must have to wait*/

            //kprintf("\n Lock Write check entered \n");
            lock_ptr->type[currpid]=READ;
            lock_ptr->exec_state[currpid]=WAITING;
            lock_ptr->prio[currpid]=priority;
            lock_ptr->wait_time[currpid]=ctr1000;
            lock_ptr->waiting_proc[currpid]=1;
            lock_ptr->wait_q_len++;

            pptr->pstate=PRWAIT;
            pptr->wait_lock=ldes1;
            rampPrio(ldes1);

            resched();
            restore(ps);
            return OK;
        }
        else if(priority>=getHighestWritePrio(ldes1)){ /*If the lock is acquired by another READ and if the current process has READ priority higher than the highest waiting WRITE priority , then it can acquire the lock */
            lock_ptr->type[currpid]=READ;
            lock_ptr->exec_state[currpid]=EXEC;
            lock_ptr->prio[currpid]=priority;
            lock_ptr->admitted_proc[currpid]=1;

            pptr->acq_locks[ldes1]=1;

            restore(ps);
            return OK;
        }
        else{ /*Lock is locked ;P and the READ operation has a lesser priority than the highest prioroty WRITE operation and hence it needs to wait*/
            lock_ptr->type[currpid]=READ;
            lock_ptr->exec_state[currpid]= WAITING;
            lock_ptr->prio[currpid]=priority;
            lock_ptr->wait_time[currpid]=ctr1000;
            lock_ptr->waiting_proc[currpid]=1;
            lock_ptr->wait_q_len++;

            pptr->pstate=PRWAIT;
            pptr->wait_lock=ldes1;

            rampPrio(ldes1);
            resched();

            restore(ps);
            return OK;
        }
    }
    else if(type==WRITE){
        if(lock_ptr->lockstate==L_FREE){ /* If there is a free lock available instantly allocate it to WRITE*/
            lock_ptr->lockstate = L_USED;
            lock_ptr->type[currpid]=WRITE;
            lock_ptr->exec_state[currpid]=EXEC;
            lock_ptr->prio[currpid]=priority;
            lock_ptr->admitted_proc[currpid]=1;

            pptr->acq_locks[ldes1]=1;
            /*update priority wait func()*/

            restore(ps);
            return OK;
        }
        else{   /*If the lock is in use put the WRITE to waiting , as the lock cannot be shared and must be exclusive for WRITE */
            lock_ptr->type[currpid]=WRITE;
            lock_ptr->exec_state[currpid]=WAITING;
            lock_ptr->prio[currpid]=priority;
            lock_ptr->wait_time[currpid]= ctr1000;
            lock_ptr->waiting_proc[currpid]=1;
            lock_ptr->wait_q_len++;

            pptr->pstate=PRWAIT;
            pptr->wait_lock=ldes1;

            rampPrio(ldes1);
            /*resched*/
            resched();
            
            restore(ps);
            return OK;
        }
    }
    kprintf("\nLOCK.C: Some unknown error occured\n");
    restore(ps);
    return SYSERR;
}