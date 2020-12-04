#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

void create_pd(int pid){

    int frm_no;
    pd_t *pde;
    int i;

    if(get_frm(&frm_no)!=SYSERR){

        proctab[pid].pdbr=(frm_no+FRAME0)*NBPG;
        frm_tab[frm_no].fr_pid=pid;
	    frm_tab[frm_no].fr_type=FR_DIR;
	    frm_tab[frm_no].fr_status=FRM_MAPPED;
	    frm_tab[frm_no].fr_vpno=0;
	    frm_tab[frm_no].fr_refcnt=4;
        pde=proctab[pid].pdbr;
        for(i=0; i<NPTE; i++){

		        pde[i].pd_write=1; /* All the page table are writable*/
		        pde[i].pd_user=0;
		        pde[i].pd_pwt=0;
		        pde[i].pd_pcd=0;
		        pde[i].pd_acc=1;
		        pde[i].pd_mbz=0;
		        pde[i].pd_global=0;
		        pde[i].pd_avail=0;
                pde[i].pd_pres=0;
                pde[i].pd_base=0;
                /* The first four page tables are global hence we initialize in the following manner*/
		        if(i>=0&&i<4)
                        {
                            pde[i].pd_pres=1;
                            pde[i].pd_base=FRAME0+i;
                        }

                }
            }

    return OK;
}

/*create page table first time*/
void create_pt(int *frm_no, int pid){

    if(get_frm(frm_no)==SYSERR){
        kprintf("/n frame no error in create pt");
        //shutdown();
        return SYSERR;
    }
    
    /*Assign a page table to that frame*/
    frm_tab[*frm_no].fr_type=FR_TBL;
    frm_tab[*frm_no].fr_status=FRM_MAPPED;
    frm_tab[*frm_no].fr_pid=pid;

    return OK;
}

/*initialize the page tabel*/
void initialize_pt(int frm_no){
    int i;
    pt_t *pte;

    pte=(pt_t *)((frm_no+FRAME0)*NBPG);
    for(i=0; i<1024; i++){
        pte->pt_pres=0;
        pte->pt_write=0;
        pte->pt_user=0;
        pte->pt_pwt=0;
        pte->pt_pcd=0;
        pte->pt_acc=0;
        pte->pt_dirty=0;
        pte->pt_mbz=0;
        pte->pt_global=0;
        pte->pt_avail=0;
        pte->pt_base=0;
        pte++;
    }
}

