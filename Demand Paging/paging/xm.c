/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  STATWORD ps;
  disable(ps);

  if(isbadbs(source)||isbadnpages(npages)||isbadvirtaddr(virtpage)||(npages>bsm_tab[source].bs_npages && bsm_tab[source].bs_npages>-1)){
      restore(ps);
      return SYSERR;
  }
  
  bsm_map(currpid, virtpage, source, npages);

  //kprintf("\nxmmap successful");
  restore(ps);
  return OK;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{ 
  STATWORD ps;
  disable(ps);

  if(isbadvirtaddr(virtpage)){
    restore(ps);
    return SYSERR;  
  }
  
  bsm_unmap(currpid, virtpage, -1);
  restore(ps);
  return OK;
}
