#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
#include <mem.h>

void insert_queue(struct policy_q ** qhead, int frm_no, int age){
	struct policy_q *new_frm,*temp;
	new_frm = (struct policy_q *)getmem(sizeof(struct policy_q));
	/*p->acc_bit = acc;*/
    new_frm->frm_no = frm_no;
    new_frm->age=age; /*We set this as -1 while calling the queue struct for SC*/
	new_frm->next = NULL;
    
	/*While inserting the first element into the queue*/
	if(*qhead==NULL){   
		*qhead = new_frm;
		sc_curr=*qhead; /*Initialize sc_curr for SC policy with the header of the queue */
		return; 
    	}
    
	temp = *qhead;
    while(temp->next != NULL){
		temp = temp->next;
	}

   	temp->next = new_frm;
}

int remove_queue(struct policy_q **qhead, int frm_no){

        struct policy_q *temp, *prev_frm;
        if(*qhead==NULL){
			return SYSERR;			
		}

        
	temp = *qhead;
	prev_frm = *qhead;

        /* removing first node */
        if((*qhead)->frm_no == frm_no){
            *qhead = (*qhead)->next;
            freemem((struct mblock *)temp, sizeof(struct policy_q));
            return frm_no;
        }

	while(temp != NULL && temp->frm_no != frm_no){
		prev_frm = temp;
		temp = temp->next;
	}
	if(temp->frm_no == frm_no){
		prev_frm->next = temp->next;
		freemem((struct mblock *)temp, sizeof(struct policy_q));
        return frm_no;
	}
        return SYSERR;
}