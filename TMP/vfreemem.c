/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
	STATWORD ps;    
	struct	mblock	*p, *q, *vmemlist;
	unsigned top;

	vmemlist = proctab[currpid].vmemlist;

	if(size == 0 || size > PPBS * NBPG)
	{
		kprintf("\n Problem in vfreemem - size \n");
		return SYSERR;
	}

	if((unsigned)block < (unsigned) VBASE * NBPG || block > (unsigned)((proctab[currpid].vhpno + proctab[currpid].vhpnpages) * NBPG))
	{
		//kprintf("\n Problem with block \n");
		return SYSERR;
	}

	size = (unsigned)roundmb(size);
	disable(ps);
	for( p= vmemlist->mnext,q= vmemlist; p != (struct mblock *) NULL && p < block ; q=p,p=p->mnext )
		;
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= vmemlist) || (p!=NULL && (size+(unsigned)block) > (unsigned)p )) 
	{
		//kprintf("\n Problem with top \n");
		restore(ps);
		return(SYSERR);
	}
	if ( q!= vmemlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	restore(ps);
	return(OK);
}
