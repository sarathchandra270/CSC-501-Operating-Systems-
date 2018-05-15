/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*vgetmem(nbytes)
	unsigned nbytes;
{
	//kprintf("\n helo all");
	
	STATWORD ps;    
	struct	mblock	*p, *q, *leftover;

	disable(ps);
	struct mblock *vlist = proctab[currpid].vmemlist;
	if (nbytes==0 || vlist->mnext== (struct mblock *) NULL) {
		//kprintf("\n helo all");
		restore(ps);
		return( (WORD *)SYSERR);
	}
	//kprintf("\n me outside");
	nbytes = (unsigned int) roundmb(nbytes);
	for (q= vlist,p=vlist->mnext ;
	     p != (struct mblock *) NULL ;
	     q=p,p=p->mnext)
		 {
			 //kprintf("hello inside if %d\n",q->mlen);
		if ( q->mlen == nbytes) {
			q->mnext = p->mnext;
			restore(ps);
			return( (WORD *)p );
		} else if ( q->mlen > nbytes ) {
			//kprintf("hello inside else if\n");
			leftover = (struct mblock *)( (unsigned)p + nbytes );
			q->mnext = leftover;
			//leftover->mnext = p->mnext;
			q->mlen = q->mlen - nbytes;
			restore(ps);
			return( (WORD *)p );
		}
	}
	//kprintf("hello inside SYSERR\n");
	restore(ps);
	return( (WORD *)SYSERR );
	//return( SYSERR );
	
}


