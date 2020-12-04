/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{

  STATWORD ps;
  disable(ps);
  
  //kprintf("\npage fault handler has entered");
  unsigned long fault_addr, pdbr;
  int vpno, frm_no, frm_no_page, bs, pageth, p, q, vp;
  virt_addr_t *vaddr;

  pt_t *pte;
  pd_t *pde;
  pdbr=proctab[currpid].pdbr;
  //pdbr=read_cr3();

  fault_addr=read_cr2(); /* Get the faulted address */
  vp=fault_addr/NBPG; /*Let vp be the virtual page number of the page containing the faulted address.*/

  //kprintf("\n fault address %x", fault_addr);
  vaddr=(virt_addr_t*)&fault_addr;
  p=vaddr->pd_offset; /*Let p be the upper ten bits of a*/
  q=vaddr->pt_offset; /*Let q be the bits [21:12] of a.*/

  pde=pdbr+(p*sizeof(pd_t)); /*Let pde point to the p-th page table.*/
  if(!pde->pd_pres){         /*If the p-th page table does not exist, obtain a frame for it and initialize it.*/
  //kprintf("\n The page table entry was not present for the given virtual address \n");
    if(create_pt(&frm_no, currpid)==SYSERR){
        kprintf("\nkill since pt cannot be created");
        //shutdown();
        kill(currpid);    /* If there arises a case where a frame for page table cannot be alloted, then the process must be terminated*/
        restore(ps);
        return SYSERR;
    }
    initialize_pt(frm_no);

    /* Mark the page table entry for that pt as present in pd and mark it as writable*/
    pde->pd_pres=1;
    pde->pd_write=1;
    pde->pd_user=0;
    pde->pd_pwt=0;
    pde->pd_pcd=0;
    pde->pd_acc=0;
    pde->pd_mbz=0;
    pde->pd_fmb=0;
    pde->pd_global=0;
    pde->pd_avail=0;
    pde->pd_base=FRAME0+frm_no;

  }
  /*page in the faulted page*/
  pte=(pt_t*)(pde->pd_base*NBPG+q*sizeof(pt_t));
  if(!pte->pt_pres){
    //kprintf("\n The given page wasn't present in the frames, have to lookup in BS\n");
    if(bsm_lookup(currpid,fault_addr,&bs,&pageth)==SYSERR)  /* Since the page isnt available in the page table, check whether it is available in the backing store*/
		{
      //kprintf("\n PFHANDLER: BSM lookup failed . Killing the process....\n");
     //shutdown();
      kill(currpid);
			restore(ps);
			return SYSERR;	
		}

      if(get_frm(&frm_no_page)==SYSERR){  /* get a free frame to write the page found in bs into the page table*/
          //kprintf("\nPFHandler: No free frame was found. Hence killing the process ...\n");
          //shutdown();
          kill(currpid);                   /* Kill the process if a free frame could not be found*/
          restore(ps);
          return SYSERR;
      }

      /*Update the page table entries once the page and frame have been found*/
      pte->pt_pres=1;
      pte->pt_write=1;
      pte->pt_base=FRAME0+frm_no_page;

      /*Update the frame reference count for the page table to indicate a new entry into the page table*/
      frm_tab[pde->pd_base-FRAME0].fr_refcnt++;

      /*Update the frame attributes for the new page frame*/
      frm_tab[frm_no_page].fr_status=FRM_MAPPED;
      frm_tab[frm_no_page].fr_type=FR_PAGE;
      frm_tab[frm_no_page].fr_pid= currpid;
      frm_tab[frm_no_page].fr_vpno=vp;
      //frm_tab[frm_no_page].fr_acc=1;  /*Used during page repllacement*/

      /*Insert frame into SC queue and Aging Queue*/
      insert_queue(&schead, frm_no_page, -1); /* For SC the aging is not considered hence it can be set to -1*/
      insert_queue(&aginghead, frm_no_page, 0); /* For Aging the age field is important and hence we set it as 0*/

      //kprintf("\n Pageg offset at Pint: %d, BS_ID:%d, FR_NO:%d", pageth, bs, FRAME0+frm_no_page);
      read_bs((char*)((FRAME0+frm_no_page)*NBPG),bs,pageth);
  }
  
  write_cr3(pdbr);
  restore(ps);
  //kprintf("\n pfint left");
  return OK;
}


