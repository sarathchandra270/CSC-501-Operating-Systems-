#include<kernel.h>
#include<stdio.h>
#include<proc.h>

extern int flag;

char calls_names[27][30]={"freemem","chprio","getpid","getprio","gettime","kill","receive","recvclr","recvtim","resume","scount","sdelete","send","setdev","setnok","screate","signal","signaln","sleep","sleep10","sleep100","sleep1000","sreset","stacktrace","suspend","unsleep","wait"};

void printsyscallsummary()
{
	struct  pentry *p;
	int i,j;
	printf("\n");
	printf("void printsyscallsummery()\n");
	for(i=1;i<NPROC;i++)
	{
		p=&proctab[i];
		if(strcmp(p->pname,"") != 0){
			printf("Process [pid:%d] \n", i);
		}

		for(j=0;j<27;j++)
		{
			if(p->calls[j]>0){
	
			printf("\t sys_%s, ", calls_names[j]);
			printf("count: %d, ",p->calls[j]);
			printf("average execution time: %d(ms) \n", (p->calls_time[j])/p->calls[j]);
			}	
		}
			
	}
}

void syscallsummary_start()
{
	int i,j;
	flag=1;
	struct pentry *p;
	for(i=0;i<NPROC;i++)
	{
		for(j=0;j<27;j++)
		{
			p->calls[j]=0;
			p->calls_time[j]=0;
			p->initial_time[j]=0;
		}
	}
}

void syscallsummary_stop()
{
	flag=0;
}
