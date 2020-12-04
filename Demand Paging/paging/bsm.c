/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
    STATWORD(ps);
    disable(ps);

    int i, j;

    for(i=0; i<NBACKINGSTORE; i++){

        bsm_tab[i].bs_status=BSM_UNMAPPED;
        bsm_tab[i].bs_npages=-1;
        bsm_tab[i].bs_sem=-1;
        bsm_tab[i].bs_isprivheap=0;
        bsm_tab[i].bs_proc_count=0;

        /*Assuming the BS is shared unless private heap is allocated, we initialize all the process segments*/
        for(j=0; j<NPROC; j++){
            bsm_tab[i].bs_pid[j]=-1;
            bsm_tab[i].bs_vpno[j]=4096; /* The starting address of BS*/
        }
    }

    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{

    STATWORD(ps);
    disable(ps);
    int i;
    for(i=0; i<NBACKINGSTORE; i++){
        if(bsm_tab[i].bs_status==BSM_UNMAPPED){
            *avail=i;
            restore(ps);
            return OK;
        }
    }
    //kprintf("\nBacking store could not be allocated, already full!");
    restore(ps);
    return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
    /*restore the default values*/
    STATWORD(ps);
    disable(ps);
    if(isbadbs(i)){
        restore(ps);
        kprintf("\n FREE_BSM: Bad BS");
        return SYSERR;
    }
    /*The BSM can just be unmapped and the private heap can be released into shared*/
    bsm_tab[i].bs_status=BSM_UNMAPPED;
    bsm_tab[i].bs_isprivheap=0;
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
    STATWORD(ps);
    disable(ps);

    if(isbadpid(pid)){
        restore(ps);
        kprintf("\n BSM_LOOKUP: Bad BS");
        return SYSERR;  
    }
    int i;
    int pgno=((unsigned long)vaddr)/NBPG; //Using unsigned long as a precaution so that shifting bits doesnt change the sign
    //int pgno=((unsigned long)vaddr);
    //kprintf("\n pageno for %x is %x", vaddr, pgno);

    for(i=0; i<NBACKINGSTORE;i++){
        if(bsm_tab[i].bs_pid[pid] && bsm_tab[i].bs_vpno[pid]!=-1){
            int bs_lower=bsm_tab[i].bs_vpno[pid];
            int bs_upper=bsm_tab[i].bs_vpno[pid]+bsm_tab[i].bs_npages;
            //kprintf("\n bs upper limit is %d \n", bs_upper);
            //kprintf("\n bs lower limit is %d \n", bs_lower);
            //kprintf("\n page number is %d \n", pgno);
            if(pgno<bs_upper && pgno>=bs_lower){
                *pageth=pgno-bsm_tab[i].bs_vpno[pid];
                *store=i;
                restore(ps);
                return OK;
            }
        }
    }
    //kprintf("\n BSM_LOOKUP, PID:%d \n", pid);
    //shutdown();
    //kprintf("\nEither no mapping found or pagae out of bounds\n");
    //shutdown();
    restore(ps);
    return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{

    STATWORD(ps);
    disable(ps);
    /*We make sure we return call if it is a private heap, to ensure the process isn't shared*/
    if(isbadnpages(npages)||isbadbs(source)||isbadpid(pid)||bsm_tab[source].bs_isprivheap){
        kprintf("\nBSM_MAP: Boundary conditions failed\n");
        restore(ps);
        return SYSERR;
    }
    /*If it is not a private heap, then the same BS can be shared with one or more processes*/
    bsm_tab[source].bs_status=BSM_MAPPED;
    bsm_tab[source].bs_pid[pid]=1;
    bsm_tab[source].bs_vpno[pid]=vpno;
    bsm_tab[source].bs_proc_count++;
    bsm_tab[source].bs_sem=1;   

    /*For shared BS the first process inializes the npages for that BS. This works for a virtual heap too because only a single process runs*/
    if(bsm_tab[source].bs_npages==-1){
        bsm_tab[source].bs_npages=npages; 
    }
    //kprintf("\n BSM_MSP, PID:%d, BS_ID:%d", pid, source);
    restore(ps);
    return OK;

}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
    STATWORD ps;
    disable(ps);

    int i, pageth;
    int bs_index=-1;
    unsigned long virtpgno;

    if(isbadpid(pid)){
        //kprintf("\n BSM_UNMAP: Bad BS");
        restore(ps);
        return SYSERR;
    }

    /*Search for the dirty page and write of to the BSM during unmap. Extremely important for shared process or else the overwritten pages cannot be access by other shared processes when one process is killed*/
    
    for(i=0;i<NFRAMES;i++){
        if(frm_tab[i].fr_pid==pid && frm_tab[i].fr_type==FR_PAGE){
            virtpgno = frm_tab[i].fr_vpno;
            if (bsm_lookup(pid, virtpgno*NBPG, &bs_index, &pageth)==SYSERR){
                //kprintf("\n bsm lookup failed during unmap");
                restore(ps);
                return SYSERR;
            }
            else{
                //kprintf("\n Writing back to BS on unmap, frame: %d, PID:%d , PGOFF:%d , BS_ID:%d\n", i, frm_tab[i].fr_pid, pageth, bs_index);
                write_bs((char *)((FRAME0+i)*NBPG), bs_index, pageth);
            }
        }
    }
    if(isbadbs(bs_index)){
        restore(ps);
        return SYSERR;
    }

    bsm_tab[bs_index].bs_pid[pid]=-1;
    bsm_tab[bs_index].bs_vpno[pid]=4096;

    bsm_tab[bs_index].bs_proc_count--; /* To keep a count of the number of processes still using that BS*/
    //kprintf("\npid in bsm unmap %d", pid);
    /*Once there are no more processes using the BS , unmap the BS*/
    if(bsm_tab[bs_index].bs_proc_count==0){
        bsm_tab[bs_index].bs_npages=-1;
        bsm_tab[bs_index].bs_sem=-1;
        free_bsm(bs_index);
    }

    restore(ps);
    return OK;
}


