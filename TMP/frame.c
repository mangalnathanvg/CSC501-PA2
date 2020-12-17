/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
  STATWORD ps;
  disable(ps);
  // Frames are UNMAPPED initially and not allocated to any process. Each frame corresponds to a page
  int i = 0;
  for(i = 0; i < NFRAMES; i++)
  {
    frm_tab[i].fr_status = FRM_UNMAPPED;
    frm_tab[i].fr_pid = INVALID_PID;
    frm_tab[i].fr_vpno = 0;
    frm_tab[i].fr_refcnt = 0;
    frm_tab[i].fr_type = FR_PAGE;
    frm_tab[i].fr_dirty = 0;
  }

  restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
  STATWORD ps;
  disable(ps);

  // Get the first frame that is UNMAPPED.
  int i = 0;
  for(i = 0; i < NFRAMES; i++)
  {
      if(frm_tab[i].fr_status == FRM_UNMAPPED)
      {
        *avail = i;
        restore(ps);
        return OK;
      }
  }
 
  // Perform replacement if no free frame was found. Find frame to evict
  // int select_frame = frame_replacement();

  // // Allocate frame only if it is valid
  // if(select_frame != SYSERR)
  // {
  //   // Evict selected frame
  //     free_frm(select_frame);

  //   // Reallocate selected frame
  //     *avail = select_frame;
  //     restore(ps);
  //     return OK;
  // }

  restore(ps);
  return SYSERR;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{

  kprintf("To be implemented!\n");
  return OK;
}



