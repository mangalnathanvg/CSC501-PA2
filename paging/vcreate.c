/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	STATWORD ps;
	disable(ps);

	int newpid = create(procaddr, ssize, priority, name, nargs, args);

	int store;

	int bs_status = get_bsm(&store);

	if(bs_status == SYSERR)
	{
		//kprintf("\n get_bsm error in vcreate \n");
		restore(ps);
		return SYSERR;
	}

	proctab[newpid].store = store;
	proctab[newpid].vhpnpages = hsize;
	proctab[newpid].vhpno = VBASE;

	bsm_tab[store].bs_private = 1;
	bsm_tab[store].bs_private_pid = newpid; 
	bsm_map(newpid, VBASE, store, hsize);
	
	proctab[newpid].vmemlist = getmem(sizeof(struct mblock *));
	proctab[newpid].vmemlist->mnext = VBASE * NBPG;
	proctab[newpid].vmemlist->mlen = 0; 

	struct mblock *base;
	base = (unsigned int) ((BS_START_PAGE + (store * 256))* NBPG);
	base->mlen = hsize * NBPG;
	base->mnext = NULL;

	restore(ps);
	return newpid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
