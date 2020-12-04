/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{

  STATWORD(ps);
  disable(ps);

  int i;
  for(i=0; i<NFRAMES; i++){
      frm_tab[i].fr_status=FRM_UNMAPPED;
      frm_tab[i].fr_pid=-1;
      frm_tab[i].fr_vpno=0;
      frm_tab[i].fr_refcnt=0;
      frm_tab[i].fr_type=FR_PAGE;
      frm_tab[i].fr_dirty=0;

  }

  /*Initialize the SC and Aging queues for replacement policy*/	
  schead = NULL;
  aginghead= NULL;
  sc_curr=NULL;
  restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
  STATWORD ps;
  disable(ps);
  
  /*Get a free frame*/
  int i, frame_no=-1;
  for(i=0; i<NFRAMES; i++){
    if(frm_tab[i].fr_status==FRM_UNMAPPED){
        *avail=i;
        restore(ps);
        return OK;
    }
  }

  /*If a free frame is not available, implement replacement policy to get the new frame*/
  switch(grpolicy()){
    case AGING:
      frame_no=get_frame_aging(&aginghead);
      break;
    default:
      frame_no=get_frame_sc(&schead);
      break;

  }
  /*check if the returned frame is a valid frame*/
  if(frame_no==-1){
      kprintf("\n the returned frame is not valid");
      //shutdown();
      restore(ps);
      return SYSERR;
  }
  else{
      
    //kprintf("\n The evicted frame is: %d", FRAME0+frame_no+1);
    free_frm(frame_no); /*release the selected frame */
    //shutdown();
    *avail=frame_no;
  }
  restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
  STATWORD ps;
  disable(ps);

  int bs, pid, pd_offset, pt_offset, pageth, pt_ind;
  pd_t *pde; /* Page Directory Entry*/
  pt_t *pte; /*Page Table Entry */
  unsigned long pdbr, vaddr;

  /* Release the frame only if it is a page. Releasing a PD or PT might result in unexpected outputs */
  if(frm_tab[i].fr_type==FR_PAGE){
    
    pid=frm_tab[i].fr_pid;
    pdbr=proctab[pid].pdbr; 

    vaddr=frm_tab[i].fr_vpno*NBPG;
    pd_offset=frm_tab[i].fr_vpno/1024;
    pt_offset=frm_tab[i].fr_vpno&1023;
    pde=pdbr+(pd_offset*sizeof(pd_t));
    pte=pde->pd_base*NBPG+(pt_offset*sizeof(pt_t));
    pt_ind=pde->pd_base-FRAME0;

    /*Remove the frame entries from the queues of SC and aging*/
    if(remove_queue(&schead, i) == SYSERR||remove_queue(&aginghead, i)==SYSERR){
			restore(ps);
			return SYSERR;
		}
    
    /*Write the page back to BS*/
    // kprintf("\nFREE_FRM:\n");
    if(bsm_lookup(pid, vaddr, &bs, &pageth)!=SYSERR){
        write_bs((i+FRAME0)*4096,bs,pageth);
    }
    else{
      //kprintf("\nBS lookup failed. Write unsuccessful");
    }

  

  frm_tab[i].fr_refcnt--;
  pte->pt_pres=0; /* Mark the page table as absent*/

  if(!frm_tab[pt_ind].fr_refcnt)
		{
			pde->pd_pres=0; /* Mark page table entry as not present in page directory once all the pages have been written off to conserve frames*/
		
			frm_tab[pt_ind].fr_status=FRM_UNMAPPED;
			frm_tab[pt_ind].fr_pid=-1;
			frm_tab[pt_ind].fr_vpno=4096;
			frm_tab[pt_ind].fr_type=FR_PAGE;
		}
  }
  restore(ps);
  return OK;
}



