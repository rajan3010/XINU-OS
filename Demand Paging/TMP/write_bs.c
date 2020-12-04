#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mark.h>
#include <bufpool.h>
#include <paging.h>

int write_bs(char *src, bsd_t bs_id, int page) {

  /* write one page of data from src
     to the backing store bs_id, page
     page.
  */
   STATWORD ps;
   disable(ps);

   /* check bounds for bs and page limit per bs*/
   if(isbadbs(bs_id)||isbadnpages(page)){
      restore(ps);
      return SYSERR;
   }

   //kprintf("\n Pageg offset at write: %d, BS_ID:%d", page, bs_id);
   char * phy_addr = BACKING_STORE_BASE + bs_id*BACKING_STORE_UNIT_SIZE + page*NBPG;
   bcopy((void*)src, phy_addr, NBPG);
   //kprintf("\n write successfull \n");
   restore(ps);
   return OK;
}

