#include<stdio.h>
#include<conf.h>
#include<i386.h>
#include<kernel.h>
#include<proc.h>
#include<lock.h>

int lcreate(){
    STATWORD ps;
    disable(ps);
    
    int i, j;
    /*if(isbadlock(lck_ind)){
        kprintf("\n LCREATE: An error occurred, the lock seems to be invalid");
        restore(ps);
        return SYSERR;
    }*/


    for(i=0; i<NUM_LOCKS; i++){

        lck_ind=lck_ind+1;

        if(lck_ind>NUM_LOCKS){
            lck_ind=0;
        }
        if(lck_tab[lck_ind].lockstate==L_DELETED){
            
            lck_tab[lck_ind].lockstate=L_FREE;
            //kprintf("\nNPROC=%d\n", NPROC);
            //dshutdown();
            for(j=0; j<NPROC; j++){
                
                lck_tab[lck_ind].type[j]=NOTALLOC;
                lck_tab[lck_ind].exec_state[j]=RELEASED;
                lck_tab[lck_ind].prio[j]=0; /*Initialize the waiting priority for that lock */      
                lck_tab[lck_ind].wait_time[j]=0; /*Initialize the wait time for that process for that locck */
                lck_tab[lck_ind].admitted_proc[j]=0; /*Initialize the acquired process value for each lock as 0 */
                lck_tab[lck_ind].waiting_proc[j]=0;  /*Initialize the waiting process value for each lock as 0 */       
            }
            restore(ps);
            return lck_ind;
        }
    }
    kprintf("\n No free lock has been found \n");
    return SYSERR;
}