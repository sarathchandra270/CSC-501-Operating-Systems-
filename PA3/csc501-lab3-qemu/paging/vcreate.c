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
	STATWORD 	ps;
	disable(ps);
	
	int bs = -1;
	get_bsm(&bs);
	if(bs==-1)
	{
		restore(ps);
		return SYSERR;
	}
int pid = create(procaddr,ssize,priority,name,nargs,args);
	if(pid==SYSERR)
	{
		restore(ps);
		return SYSERR;
	}
	bsm_tab[bs].bs_status = BSM_MAPPED;
	bsm_tab[bs].bs_priv = 1;
	bsm_tab[bs].bs_pid[pid] = 1;
	bsm_tab[bs].bs_npages[pid] = hsize;
	proctab[pid].store = bs;
	proctab[pid].vhpno = 4096;
	proctab[pid].vhpnpages = hsize;
	proctab[pid].bs_pmap[bs] =1;
		
	proctab[pid].vmemlist->mlen = hsize * NBPG;
	proctab[pid].vmemlist->mnext = 4096*4096;
	restore(ps);
	return pid;
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
