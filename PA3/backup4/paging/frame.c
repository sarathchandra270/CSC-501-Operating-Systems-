/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

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

  kprintf("To be implemented!\n");
  return OK;
}

int getFrameBySC()
{
	int i;
}

int getFrameByAGING()
{
	
}

