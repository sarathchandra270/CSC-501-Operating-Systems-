/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
 fr_map_t frm_tab[NFRAMES];
extern int page_replace_policy;
 
SYSCALL init_frm()
{
	STATWORD 	ps;
	disable(ps);
	int i;
	for(i=0; i<NFRAMES;i++)
	{
		frm_tab[i].fr_status = FRM_UNMAPPED;
		frm_tab[i].fr_pid = -1;
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
	STATWORD 	ps;
	disable(ps);
	int i;
	for(i=0;i<NFRAMES;i++)
	{
		if(frm_tab[i].fr_status==FRM_UNMAPPED)
		{
			*avail = i;
			restore(ps);
			return(OK);
		}
	}
	if(page_replace_policy == SC)
	{
		int f_sc;
		f_sc = getFrameBySC();
		if(f_sc == -1)
		{
			restore(ps);
			return(SYSERR);
		}
	}
	if(page_replace_policy == AGING)
	{
		int f_aging;
		f_aging = getFrameByAGING();
		if(f_aging == -1)
		{
			restore(ps);
			return(SYSERR);
		}
	}
	restore(ps);
	return(SYSERR);
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	STATWORD ps;
	disable(ps);
	int store =-1;
	int pageth= -1;
	int pid = frm_tab[i].fr_pid;
	if(frm_tab[i].fr_type == FR_TBL || frm_tab[i].fr_type == FR_DIR)
	{
		restore(ps);
		return SYSERR;
	}
	
	//if the requested frame is page, then free it.
	
	unsigned long addr = frm_tab[i].fr_vpno * NBPG;
	virt_addr_t *ptr = (virt_addr_t *)&addr;
	if((bsm_lookup(currpid, addr, &store, &pageth)) != SYSERR || store != -1 || pageth !=-1)
	{
		write_bs((i+FRAME0)*NBPG, store, pageth);
	}
	frm_tab[i].fr_status = FRM_UNMAPPED;
	frm_tab[i].fr_pid = -1;
	frm_tab[i].fr_vpno = 0;
	frm_tab[i].fr_refcnt = 0;
	frm_tab[i].fr_type = FR_PAGE;
	
	pd_t *pd = ptr->pd_offset*4 + proctab[pid].pdbr;
	pt_t *pt = ptr->pt_offset*4 + pd->pd_base;
	pt->pt_pres = 0;
	frm_tab[pd->pd_base - FRAME0].fr_refcnt--;
	if(frm_tab[pd->pd_base - FRAME0].fr_refcnt == 0)
	{
		pd->pd_pres = 0;
		frm_tab[pd->pd_base - FRAME0].fr_status = FRM_UNMAPPED;
		frm_tab[pd->pd_base - FRAME0].fr_pid = -1;
		frm_tab[pd->pd_base - FRAME0].fr_vpno = 0;
		frm_tab[pd->pd_base - FRAME0].fr_refcnt = 0;
		frm_tab[pd->pd_base - FRAME0].fr_type = FR_PAGE;
	}
	restore(ps);
	return OK;
}

int getFrameBySC()
{
	int i;
}

int getFrameByAGING()
{
	
}

