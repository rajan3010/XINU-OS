#include<conf.h>
#include<kernel.h>
#include<proc.h>
#include<lock.h>
#include<stdio.h>
#include<i386.h>

int ldelete(int lockdescriptor){
    STATWORD ps;
    disable(ps);

    lck_tab[lockdescriptor].lockstate=L_DELETED;
    int i;
    for(i=0; i<NPROC; i++){
        if(lck_tab[lockdescriptor].exec_state[i]==EXEC){
            lck_tab[lockdescriptor].exec_state[i]=RELEASED;
            lck_tab[lockdescriptor].prio[i]=-1;
            lck_tab[lockdescriptor].admitted_proc[i]=0;
            if(proctab[i].pinh!=0){
                lchprio(i, proctab[i].pprio_bckup);
                if(proctab[i].wait_lock!=-1){
                    rampPrio(proctab[i].wait_lock);
                }
                ready(i,1);
            }
        }
        else if(lck_tab[lockdescriptor].exec_state[i]==WAITING){
            lck_tab[lockdescriptor].exec_state[i]=RELEASED;
            lck_tab[lockdescriptor].prio[i]=-1;
            lck_tab[lockdescriptor].waiting_proc[i]=0;
            proctab[i].pwaitret = DELETED;
            if(proctab[i].pinh!=0){
                lchprio(i, proctab[i].pprio_bckup); /* No need to ramp the priorities since we are going to release all the processes from the waiting list*/
                ready(i,1);
            }             
        }      
    }

    restore(ps);
    return OK;
} 