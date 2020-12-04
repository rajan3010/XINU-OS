#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

    STATWORD ps;
    disable(ps);
    //kprintf("\nget bs has entered\n");
  /* requests a new mapping of npages with ID map_id */
    if(isbadbs(bs_id)||isbadnpages(npages)||bsm_tab[bs_id].bs_isprivheap){
      restore(ps);
      kprintf("\n Invalid range get bs leaving\n");
      //shutdown();
      return SYSERR;
    }

    if((bsm_tab[bs_id].bs_status==BSM_MAPPED)&&(bsm_tab[bs_id].bs_npages !=-1)){
        restore(ps);
        //kprintf("\nAlready mapped get bs leaving \n");
        return bsm_tab[bs_id].bs_npages;
    }
    //bsm_tab[bs_id].bs_status = BSM_MAPPED;

    //kprintf("\n Nothing abnormal get_bs leaving \n");
    restore(ps);
    return npages;

}


