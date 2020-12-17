#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
    STATWORD ps;
    disable(ps);

    if(bs_id <0 || bs_id >= TOTAL_BS_COUNT)
    {
      kprintf("\n Backing Store does not exist \n");
      restore(ps);
      return SYSERR;
    }

    if(npages < 1 || npages > 256)
    {
      kprintf("\n Backing Store Page limit exceeded! \n");
      restore(ps);
      return SYSERR;
    }

    if(bsm_tab[bs_id].bs_private==0 || (bsm_tab[bs_id].bs_private == 1 && bsm_tab[bs_id].bs_private_pid == currpid))
    {
       if(bsm_tab[bs_id].bs_status == BSM_MAPPED)
       {
         restore(ps);
         return bsm_tab[bs_id].bs_available;
       }
       else{
         restore(ps);
         return npages;
       }
    }

    restore(ps);
    return SYSERR;

}


