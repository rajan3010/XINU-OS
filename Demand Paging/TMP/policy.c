/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

extern int page_replace_policy;

/* Get frame using second chance*/
int get_frame_sc(struct policy_q **schead){
  
  struct policy_q *temp;
  unsigned long virtaddr,pdbr;
  //sc_curr= *schead; /* current pointer to start from where we had left last time*/
  temp= sc_curr;  
  int frame_no=-1, vpd_off, vpt_off, pid;
  pt_t *pte;
  pd_t *pde;
  virt_addr_t *vaddr;

  while(temp!= NULL){

    virtaddr=frm_tab[temp->frm_no].fr_vpno*NBPG;
    vaddr= (virt_addr_t *)&virtaddr;
    vpt_off=vaddr->pt_offset;
    vpd_off=vaddr->pd_offset;
    pid=frm_tab[temp->frm_no].fr_pid;
    pdbr=proctab[pid].pdbr;
    pde=pdbr+vpd_off*sizeof(pd_t);
    pte=(pt_t*)(pde->pd_base*NBPG+vpt_off*sizeof(pt_t)); /* Get the pte adress from the frame vpno to check whether it was accessed*/

    if(pte->pt_acc){   /*If frame access bit is set to 1, it is reset to 0 and moved to the next frame*/
      pte->pt_acc=0;
    }
    else{
      
      if(temp->next!=NULL){   /*If frame access bit is 0 the current pointer is set to the next frame and the frame is returned*/
        sc_curr=temp->next;
      }
      frame_no=temp->frm_no;
       return frame_no;
    }
    temp=temp->next;
  }
  sc_curr=*schead;
  frame_no=sc_curr->frm_no;
  sc_curr=sc_curr->next; /*Once one rotation is over we comeback to the front again and since this time the first frame woul have been set to 0 we set the curr pointer and return the frame*/
  return frame_no;

}

int get_frame_aging(struct policy_q **aginghead){

struct policy_q *temp;
temp= *aginghead;
int min=255;
int frame_no=-1, vpd_off, vpt_off, pid;
unsigned long virtaddr, pdbr;
pt_t *pte;
pd_t *pde;
virt_addr_t *vaddr; 

while(temp!=NULL){
  virtaddr=frm_tab[temp->frm_no].fr_vpno*NBPG;
  vaddr= (virt_addr_t *)&virtaddr;
  vpt_off=vaddr->pt_offset;
  vpd_off=vaddr->pd_offset;
  pid=frm_tab[temp->frm_no].fr_pid;
  pdbr=proctab[pid].pdbr;
  pde=pdbr+vpd_off*sizeof(pd_t);
  pte=(pt_t*)(pde->pd_base*NBPG+vpt_off*sizeof(pt_t)); /* Get the pte adress from the frame vpno to check whether it was accessed*/

  temp->age=temp->age/2; /*first decrease by half (= one bit shift to the right) the age of each page in the FIFO queue*/
  if(pte->pt_acc){
      temp->age+=128;   /*If a page has been accessed (i.e., pt_acc = 1), you add 128 to its age (255 is the maximum age)*/
      if(temp->age>255){
        temp->age=255;
      }
  }
  if(temp->age<min){
      min=temp->age;
      frame_no=temp->frm_no; /*page with the youngest age will be replaced first*/
  }
  temp=temp->next;
}
return frame_no;
}
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy)
{
  STATWORD ps;
  disable(ps);
  /* sanity check ! */
  if(policy!=SC && policy!=AGING){
    kprintf("\n Invalid Policy \n");
    restore(ps);
    return SYSERR;
  }
  page_replace_policy=policy;
  restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}

