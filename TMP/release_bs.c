#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

  /* release the backing store with ID bs_id */
   STATWORD ps;
   disable(ps);

   if(bsm_tab[bs_id].bs_private == 1 && bsm_tab[bs_id].bs_private_pid == currpid)
   {
      free_bsm(bs_id);
      restore(ps);
      return OK;
   }

   restore(ps);
   return SYSERR;

}

