#include<stdio.h>
#include<conf.h>
#include<i386.h>
#include<kernel.h>
#include<lock.h>
#include<proc.h>

void linit(){
    int i;

    lck_ind=-1; /* Initialize it with -1 so that it starts with 0 in the lcreate implementation */

    for(i=0; i<NUM_LOCKS; i++){
        //kprintf("\n Iteration %d\n", i);  
        lck_tab[i].lockstate=L_DELETED;
        lck_tab[i].wait_q_len=0;
    }

}
int isWrite(int ldes1){
	int i ;
	for (i= 0; i < NPROC; i++)
	{
		if(lck_tab[ldes1].exec_state[i]==EXEC && lck_tab[ldes1].type[i]==WRITE){
			return 1;
		}
	}
	return 0;
}

int getHighestWritePrio(int ldes1){
    int ret=-1;

    int i;

    for(i=0; i< NPROC; i++){
        if(lck_tab[ldes1].type[i]==WRITE && lck_tab[ldes1].exec_state[i]==WAITING && lck_tab[ldes1].prio[i]>ret){
                ret=lck_tab[ldes1].prio[i];
        }
    }

    return ret;
}

int getMaxWaitProc(int ldes){
    int max_prio=-1;
    int max_prio_pid=-1;

    int i;

    //kprintf("\n wait_q_len: %d \n", lck_tab[ldes].wait_q_len);
    if(lck_tab[ldes].wait_q_len){
    for(i=0; i< NPROC; i++){
        if(lck_tab[ldes].waiting_proc[i]){
            if(lck_tab[ldes].prio[i]>max_prio){
                max_prio=lck_tab[ldes].prio[i];
                max_prio_pid=i;  
            }
            else if(lck_tab[ldes].prio[i]==max_prio){   /* If priorities are equal then check their waiting times */
                if((lck_tab[ldes].wait_time[i]-lck_tab[ldes].wait_time[max_prio])>1000){ 
                    max_prio=lck_tab[ldes].prio[i];
                    max_prio_pid=i;
                }
                else if((lck_tab[ldes].wait_time[i]-lck_tab[ldes].wait_time[max_prio])<1000){ /*If the waiting times are within 1s check whether it is WRITE, which must be given higher priority */
                    if(lck_tab[ldes].type[i]==WRITE){
                        max_prio=lck_tab[ldes].prio[i];
                        max_prio_pid=i;
                    }
                }
            }
        }
    }
    }
    return max_prio_pid;
}
int calcMaxPrio(int ldes1){
    int max=-1;
    int prio, i;
    struct lockentry *lptr;
    struct pentry *pptr;
    lptr=&lck_tab[ldes1];

    //kprintf("wait_q_len: %d", lptr->wait_q_len);
    if(lptr->wait_q_len){
        for(i=0; i<NPROC; i++){
            if(lptr->waiting_proc[i]){
                if(proctab[i].pinh!=0){  /*If pinh has been modified, then the modified pinh must be considered */
                    prio=proctab[i].pinh;
                }
                else{                    /*Else the original priority must be considered*/
                    prio=proctab[i].pprio_bckup;
                }

                if(prio>max){   
                    max=prio;
                }
            }
        }
    }

    return max;
}

void rampPrio(int ldes1){

    struct lockentry *lptr;
    lptr=&lck_tab[ldes1];

    int i, max_wait_prio, prio;

    max_wait_prio=calcMaxPrio(ldes1);

    //kprintf("\nMAx_Prio: %d", max_wait_prio);
    for(i=0; i< NPROC; i++){
        if(lptr->admitted_proc[i]){
            if(lptr->wait_q_len){
                if(proctab[i].pinh!=0){
                    prio=proctab[i].pinh;
                }
                else{
                    prio=proctab[i].pprio_bckup;
                }

                if(max_wait_prio>prio){
                    proctab[i].pinh=max_wait_prio;
                    /*Change the priorities*/
                    lchprio(i, proctab[i].pinh);
                    /* Once this process in the current lock is ramped up, let's now do it transitively for the locks this process is being held in wait */
                    if(proctab[i].wait_lock!=-1){
                        rampPrio(proctab[i].wait_lock);
                        }
                }
                else if(max_wait_prio>proctab[i].pprio_bckup){
                    proctab[i].pinh=max_wait_prio;
                    /*Change priorities*/
                    lchprio(i, proctab[i].pinh);
                    /* Once this process in the current lock is ramped up, let's now do it transitively for the locks this process is being held in wait */
                    if(proctab[i].wait_lock!=-1){
                        rampPrio(proctab[i].wait_lock);
                        }
                }
                else{
                    if(proctab[i].pinh!=0){
                        proctab[i].pinh=0;
                    /*Change the priorities */
                    lchprio(i, proctab[i].pprio_bckup);
                    /* Once this process in the current lock is ramped up, let's now do it transitively for the locks this process is being held in wait */
                    if(proctab[i].wait_lock!=-1){
                        rampPrio(proctab[i].wait_lock);
                        }                    
                    }
                }
            }
            else{
                if(proctab[i].pinh!=0){
                    proctab[i].pinh=0;
                    /*Change the priorities */
                    lchprio(i, proctab[i].pprio_bckup);
                    /* Once this process in the current lock is ramped up, let's now do it transitively for the locks this process is being held in wait */
                    if(proctab[i].wait_lock!=-1){
                        rampPrio(proctab[i].wait_lock);
                        }
                }
            }

        }
    }
    
}