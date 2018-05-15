#include<kernel.h>
#include<stdio.h>
#include<proc.h>
/***************************************************
For each existing process with lower priority than the parameter, print the stack base, stack size, stacklimit, and stack pointer. 
Also, for each process, include the process name, the process id and the process priority.
****************************************************/

unsigned long *esp;

void printprocstks(int priority)
{
	struct pentry *p;
	int i;
	printf("\n \n");
	printf("void printprocstks(int pripority)\n");
	for(i=0;i<NPROC;i++)
	{
		p=&proctab[i];
		if(p->pprio>priority)
		{
			printf("Process [%s] \n", p->pname);
			printf("\t pid: %d \n", i);
			printf("\t priority: %d \n", p->pprio);
			printf("\t base: 0x%08x \n", p->pbase);
			printf("\t limit: 0x%08x \n", p->plimit);
			printf("\t len: %d \n", p->pstklen);
			if(p->pstate==PRCURR)
			{
				asm("mov %esp,esp");
			}
			else 
			{
				esp=(unsigned long*)p->pesp;
			}
			printf("\t pointer: 0x%08x \n", esp);

		}
	}
	printf("\n");
}
