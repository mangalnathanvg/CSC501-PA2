/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>  // For PDBR of process

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{

  STATWORD ps;
  disable(ps);

  // Retrieve the address where page fault occured.
  virt_addr_t *page_fault_addr;

 // Page fault address stored in CR2 Register.
  unsigned long pg_fault_addr = read_cr2();

  page_fault_addr = (virt_addr_t*)&pg_fault_addr;

  // Page Directory entry for current process
  pd_t *proc_page_direntry;

  //Retrieving the page directory entry for the current process using translation algorithm
  proc_page_direntry = (proctab[currpid].pdbr + (page_fault_addr->pd_offset * 4));

  // Check if any page table exists in page directory - Case (a) 
  if(!proc_page_direntry->pd_pres)
  {
    // If page table not present, then create a page table.
    int frame_number;

    // Request a frame for allocating page table.
    get_frm(&frame_number);

    // Update status of frames
    frm_tab[frame_number].fr_status = FRM_MAPPED;
    frm_tab[frame_number].fr_pid = currpid;
    frm_tab[frame_number].fr_type = FR_TBL;

    // Set starting address of page table to new frame address.
    pt_t *new_page_table = ((FRAME0 + frame_number) * NBPG);

    // Initialize new page table with 1024 entries (Each entry is 4 bytes)
    int i = 0;
    for(i = 0; i < NFRAMES; i++)
    {
		  new_page_table[i].pt_pres = 0;
		  new_page_table[i].pt_write = 0;
		  new_page_table[i].pt_user = 0;
		  new_page_table[i].pt_pwt = 0;
		  new_page_table[i].pt_pcd = 0;
		  new_page_table[i].pt_acc = 0;
		  new_page_table[i].pt_dirty = 0;
		  new_page_table[i].pt_mbz = 0;
		  new_page_table[i].pt_global = 0;
		  new_page_table[i].pt_avail = 0;
		  new_page_table[i].pt_base = 0;
    }

    // Updating status of page directory.
			proc_page_direntry->pd_pres = 1;
			proc_page_direntry->pd_write = 1;
			proc_page_direntry->pd_user = 0;
			proc_page_direntry->pd_pwt = 0;
			proc_page_direntry->pd_pcd = 0;
			proc_page_direntry->pd_acc = 0;
			proc_page_direntry->pd_mbz = 0;
			proc_page_direntry->pd_fmb = 0;
			proc_page_direntry->pd_global = 0;
			proc_page_direntry->pd_avail = 0;
			proc_page_direntry->pd_base = FRAME0 + frame_number;

  }

  // Getting page table entry.
  pt_t *proc_page_tableentry = (proc_page_direntry->pd_base * NBPG) + (page_fault_addr->pt_offset * 4);

  // Check if page is present. - Case (b)

    int frame_page;

    // Retrieve a frame to allocate to the page.
    get_frm(&frame_page);

    // Update status of page.
    proc_page_tableentry->pt_pres = 1;
    proc_page_tableentry->pt_write = 1;
    proc_page_tableentry->pt_base = FRAME0 + frame_page;

    // Update status of frame.
    frm_tab[proc_page_direntry->pd_base - FRAME0].fr_refcnt += 1;
    frm_tab[frame_page].fr_status = FRM_MAPPED;
    frm_tab[frame_page].fr_type = FR_PAGE;
    frm_tab[frame_page].fr_pid = currpid;
    frm_tab[frame_page].fr_vpno = pg_fault_addr / NBPG;

    // Insert the allocated frame to the queue depending on the page replacement policy.
    //insert_frame_to_queue(frame_page);
    int store, pageth;

    // Search for backing store id and page for faulted address.
    int bs_status = bsm_lookup(currpid, pg_fault_addr, &store, &pageth);
    
    if(bs_status != SYSERR)
    {
      // Store the page from backing store into the free frame.
      read_bs((char*) ((FRAME0 + frame_page) * NBPG), store, pageth);
    }
    else{
      if(currpid != NULLPROC)
      {
        kill(currpid);
      }
    }

  // Update process pdbr
  write_cr3(proctab[currpid].pdbr);
  
  restore(ps);
  return OK;
}


