#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

#define DEFAULT_LOCK_PRIO 20

void sem_fun (char *msg, int sem)
{	
        kprintf ("%s:to acquire sem\n", msg);
	wait(sem);
	kprintf ("%s:acquired sem\n", msg);
        sleep (2);
        kprintf ("%s:to release sem\n", msg);
	signal(sem);
}

void lock_fun (char *msg, int lck)
{
        kprintf ("%s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("%s: acquired lock\n", msg);
        sleep (2);
        kprintf ("%s: to release lock\n", msg);
        releaseall (1, lck);
}

void new_proc(char* msg) {
	kprintf("  Process %s running.\n", msg);
	sleep(2);
	kprintf("  Process %s finished running.\n", msg);
}

void sem_make(int sem)
{
	
		int srd1,swr1,swr2;
	srd1 = create(sem_fun, 2000, 50, "ProcessA", 2,"ProcessA", sem);
	swr1 = create(new_proc, 2000, 70, "ProcessB", 1,"ProcessB");
	swr2 = create(sem_fun, 2000, 90, "ProcessC", 2,"ProcessC",sem);
	
	resume(srd1);
	sleep(1);
	resume(swr1);
	sleep(1);	
	resume(swr2);
	sleep(1);
}
void lock_make(int lck)
{
	int rd1,wr1,wr2;
		rd1 = create(lock_fun, 2000, 50, "ProcessA", 2,"ProcessA",lck);
		wr1 = create(new_proc, 2000, 70, "ProcessB", 1,"ProcessB");
		wr2 = create(lock_fun, 2000, 90, "ProcessC", 2, "ProcessC", lck);
		resume(rd1);
		sleep(1);
		resume(wr1);
		sleep(1);	
		resume(wr2);
		sleep(1);
	
}


int main()
{
	int sem;
	int lck;
	sem = screate(1);
	sem_make(sem);
	sleep(5);
	lck=lcreate();
	lock_make(lck);
	
	return 0;
}
