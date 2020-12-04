#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>
#include <proc.h>
#include <paging.h>

SYSCALL read_bs(char *dst, bsd_t bs_id, int page) {

  /* fetch page page from map map_id
     and write beginning at dst.
  */
  //kprintf("\nread bs entered\n");
   STATWORD ps;
   disable(ps);
   //kprintf("\nBS ID at pagefault %d\n", bs_id);
   //kprintf("\nnpages and readbs %d\n", page);
   /* Check bounds for BS and pages per BS*/
   if(isbadbs(bs_id)||isbadnpages(page)){
      kprintf("\nBad bs being called\n");
         restore(ps);
         return SYSERR;
   }
   //kprintf("\n Pageg offset at read: %d, BS_ID:%d", page, bs_id);
   void * phy_addr = BACKING_STORE_BASE + bs_id*BACKING_STORE_UNIT_SIZE + page*NBPG;
   //kprintf("\nREAD_BS. BS_ID:%d, PAGEOFF:%d, physical_addr: %c\n", bs_id, page, phy_addr);
   bcopy(phy_addr, (void*)dst, NBPG);
   //kprintf("\n read successful \n");
   restore(ps);
   return OK;
}


