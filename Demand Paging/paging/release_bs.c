#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

  STATWORD ps;
  disable(ps);

  int i;    
  
  /* release the backing store with ID bs_id */
  if(isbadbs(bs_id)){
        restore(ps);
        return SYSERR;
  }

  /*If the backing store is already unmapped, no need to do anything, just return */
  if(bsm_tab[bs_id].bs_status==BSM_UNMAPPED){
        restore(ps);
        return OK;
  }
  
  /*for shared processes make sure the BS doesn't contain any process mappings before releasing*/
  for(i=0; i<NPROC; i++){
        if(bsm_tab[bs_id].bs_pid[i]==1){
              restore(ps);
              //kprintf("\n processes are still there in the BS");
              return SYSERR;
        }
  }
  free_bsm(bs_id);
   
   restore(ps);
   return OK;

}

