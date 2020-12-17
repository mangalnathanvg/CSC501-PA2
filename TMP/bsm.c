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
    STATWORD ps;
    disable(ps);

    int i = 0, j=0;

    for(i = 0; i < TOTAL_BS_COUNT; i++)
    {
        bsm_tab[i].bs_status = BSM_UNMAPPED;

        for(j = 0; j < NPROC; j++)
        {
            bsm_tab[i].bs_vpno[j] = -1;
            bsm_tab[i].bs_npages[j] = 0;
        }
        bsm_tab[i].bs_private = 0;
        bsm_tab[i].bs_private_pid = INVALID_PID;
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
    STATWORD ps;
    disable(ps);
    int i;
    for(i = 0; i < TOTAL_BS_COUNT; i++)
    {
        if(bsm_tab[i].bs_status == BSM_UNMAPPED)
        {
            *avail = i;
            restore(ps);
            return OK;
        }
    }

    restore(ps);
    return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
    STATWORD ps;
    disable(ps);
 
    bsm_tab[i].bs_status = BSM_UNMAPPED;
    bsm_tab[i].bs_private_pid = INVALID_PID;
    bsm_tab[i].bs_private = 0;
    
    bsm_tab[i].bs_available = 0;

    int j;
    for(j = 0; j < NPROC; j++)
    {
        bsm_tab[i].bs_vpno[j] = -1;
        bsm_tab[i].bs_npages[j] = 0;
    }

    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
    STATWORD ps;
    disable(ps);

    int i;
    for(i = 0; i < TOTAL_BS_COUNT; i++)
    {
        if(bsm_tab[i].bs_status == BSM_MAPPED && bsm_tab[i].bs_npages[pid] > 0)
        {
           *store = i;

           *pageth = (int) (vaddr / NBPG) - bsm_tab[i].bs_vpno[pid];

           restore(ps);
           return OK; 
        }
    }

    restore(ps);
    return SYSERR;
    
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
    STATWORD ps;
    disable(ps);

    if(bsm_tab[source].bs_status == BSM_UNMAPPED)
    {
        bsm_tab[source].bs_status = BSM_MAPPED;
        bsm_tab[source].bs_available = npages;
    }

    bsm_tab[source].bs_npages[pid] = npages;
    bsm_tab[source].bs_vpno[pid] = vpno;

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
    int store, pageth;
    int bs_status = bsm_lookup(pid, (vpno * NBPG), &store, &pageth);
    
    if(bs_status == SYSERR)
    {
        restore(ps);
        return SYSERR;
    }

    bsm_tab[store].bs_vpno[pid] = -1;
    bsm_tab[store].bs_npages[pid] = 0;

    restore(ps);
    return OK;
}


