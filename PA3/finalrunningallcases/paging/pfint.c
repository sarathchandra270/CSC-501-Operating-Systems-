/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
 
int replacearray[1024];
int front = -1;
int rear = -1;
int ref = -1;
SYSCALL pfint()
{
	STATWORD ps;
	disable(ps);
	//kprintf("\n hello came pfint");
	unsigned long a = read_cr2();
	
	//kprintf("\n hello came pfint after RED %lu",a);
	//int vp = a & 0xfffff000;
	int store = -1;
	int pageth = -1;
	//check assigning with &
	virt_addr_t *ptr = (virt_addr_t *)&a;
	//kprintf("\n hello came pfint 2");
	if((bsm_lookup(currpid, a, &store, &pageth)) == SYSERR)
	{
		//kprintf("\n hello came pfint SYERRRRRR 1");
		kprintf(" Address is not a valid address\n");
		kill(currpid);
		restore(ps);
		return SYSERR;
	}
	//kprintf("\n hello came pfint 3");
	if(store == -1 || pageth ==-1)
	{
		//kprintf("\n hello came pfint SYERRRRRR 2");
		kprintf(" Address is not a valid address\n");
		kill(currpid);
		restore(ps);
		return SYSERR;
	}
	//kprintf("\n hello came pfint4");
	//kprintf("\n hello came pfint SYERRRRRR 3");
	pd_t *pd = proctab[currpid].pdbr + (ptr->pd_offset * 4);
	//kprintf("\n hello came pfint SYERRRRRR 4");
	if(pd->pd_pres == 0)
	{
		//kprintf("\n hello came pfint 1000");
		//kprintf("\n hello came pfint SYERRRRRR 4");
		int f_no;
		f_no = createPageTable(currpid);
		//kprintf("hello if of pfint %d\n",f_no);
		if(f_no==-1)
		{
			kprintf("Error in creating page table in pfint\n");
			kill(currpid);
			restore(ps);
			return SYSERR;
		}
		pd->pd_pres = 1;
		pd->pd_write = 1;
		pd->pd_pcd = 0;
		pd->pd_acc = 0;
		pd->pd_global = 0;
		pd->pd_pwt = 0;
		pd->pd_mbz = 0;
		pd->pd_user = 1;
		pd->pd_fmb = 0;
		pd->pd_avail = 0;
		pd->pd_base = FRAME0 + f_no;
	
	pt_t *pt = pd->pd_base * NBPG + (ptr->pt_offset * 4);
		
		//kprintf("\n hello came pfint SYERRRRRR 5");
		int f_num = -1;
		get_frm(&f_num);
		//kprintf("hello if of pfint %d\n",f_num);
		if(f_num==-1)
		{
			kprintf("Error in creating page in pfint\n");
			kill(currpid);
			restore(ps);
			return SYSERR;
		}
		//initialize values for frame
		frm_tab[f_num].fr_status = FRM_MAPPED;
		frm_tab[f_num].fr_pid = currpid;
		frm_tab[f_num].fr_type = FR_PAGE;
		frm_tab[f_num].fr_vpno = a/NBPG;
		frm_tab[f_num].fr_counter += 1;
		//frm_tab[pd->pd_base - FRAME0].fr_refcnt++;
		read_bs(((f_num + FRAME0)*NBPG),store,pageth);
		
		//initialize page 
		pt->pt_pres =1;
		pt->pt_acc = 1;
		pt->pt_write = 1;
		pt->pt_base = f_num + FRAME0;
		replaceinsert(f_num);
		
		//kprintf("PT ACC is %d\n",pt->pt_acc);
	}
	else
	{
		pt_t *pt = pd->pd_base * NBPG + (ptr->pt_offset * 4);
		int f_num = -1;
		get_frm(&f_num);
		//kprintf("\n hello came pfint  else %d", f_num);
		if(f_num==-1)
		{
			kprintf("Error in creating page in pfint\n");
			kill(currpid);
			restore(ps);
			return SYSERR;
		}
		//kprintf("\n hello came pfint 999999999");
		//initialize values for frame
		//kprintf("BEfore:%d\n",frm_tab[f_num].fr_vpno);
		frm_tab[f_num].fr_status = FRM_MAPPED;
		frm_tab[f_num].fr_pid = currpid;
		frm_tab[f_num].fr_type = FR_PAGE;
		frm_tab[f_num].fr_vpno = a/NBPG;
		frm_tab[f_num].fr_counter += 1;
		frm_tab[pd->pd_base - FRAME0].fr_refcnt++;
		read_bs(((f_num + FRAME0)*NBPG),store,pageth);
		//kprintf("AFTER:%d\n",frm_tab[f_num].fr_vpno);
		//initialize page 
		pt->pt_pres =1;
		pt->pt_acc = 1;
		replaceinsert(f_num);
		pt->pt_write = 1;
		pt->pt_base = f_num + FRAME0;
		//kprintf("PT ACC is %d\n",pt->pt_acc);
	
	}
	//kprintf("\n hello came pfint outside");
	//write_cr3(proctab[currpid].pdbr);
	restore(ps);
	return OK;
}

int createPageTable(int pid)
{
	int f =-1;
	pt_t *page_ptr;
	int j;
	get_frm(&f);
	if(f == -1)
	{
		return SYSERR;
	}
		frm_tab[f].fr_status = FRM_MAPPED;
		frm_tab[f].fr_pid = pid;
		frm_tab[f].fr_type = FR_TBL;
		frm_tab[f].fr_refcnt++;
		
		page_ptr = (pt_t *)((FRAME0+f) * NBPG);
		
		for(j=0;j<1024;j++)
		{
			page_ptr->pt_pres = 0;
			page_ptr->pt_write = 1;
			page_ptr->pt_user = 0;
			page_ptr->pt_pwt = 0;
			page_ptr->pt_pcd = 0;
			page_ptr->pt_acc = 0;
			page_ptr->pt_dirty = 0;
			page_ptr->pt_mbz = 0;
			page_ptr->pt_global = 0;
			page_ptr->pt_avail = 0;
			//putting frame number since address is 32 bit but pt_base is 20bit
			page_ptr->pt_base = 0; 
			page_ptr++;
		}
	return f;
}

void replaceinsert(int frame)
{
	if( rear == -1 && ref == -1)
	{
		rear = ref = 0;
	}
	else
	{
		rear++;
	}
	//kprintf("entered insert\n");
	replacearray[rear] = frame;
}
