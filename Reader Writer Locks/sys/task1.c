#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }

int mystrncmp(char* des,char* target,int n){
    int i;
    for (i=0;i<n;i++){
        if (target[i] == '.') continue;
        if (des[i] != target[i]) return 1;
    }
    return 0;
}


void reader3 (char *msg, int lck)
{
        int     ret;
        int i;

        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, READ, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock\n", msg);
        for(i=0;i<10;i++){
            kprintf("%s ",msg);
        }
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void writer3 (char *msg, int lck)
{
        int i;

        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, sleep 10s\n", msg);
        for(i=0;i<10;i++){
            kprintf("%s ",msg);
        }
        sleep (10);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void lockTest ()
{
        int     lck;
        int     rd1, rd2;
        int     wr1;
        int prio_c;

        kprintf("\nTest the basic priority inheritence: Locks\n");
        lck  = lcreate ();

        rd1 = create(writer3, 2000, 20, "reader3", 2, "A", lck);
        rd2 = create(reader3, 2000, 30, "reader3", 2, "B", lck);
        wr1 = create(writer3, 2000, 40, "writer3", 2, "C", lck);

        kprintf("-start A, then sleep 1s.\n");
        resume(rd1);
        sleep (1);
        prio_c=getprio(rd1);
        kprintf("\n Prio of writer before rd1: %d", prio_c);
        kprintf("-start C, then sleep 1s.\n");
        resume(wr1);
        sleep (1);
        prio_c=getprio(rd1);
        kprintf("\n Prio of writer after rd1: %d", prio_c);
	    //assert (getprio(wr1) == 25, "Test 3 failed");

        kprintf("-start B, then sleep 1s.\n");
        resume (rd2);
	    sleep (1);
	//assert (getprio(wr1) == 30, "Test 3 failed");
	//shutdown();
        sleep (8);
}

void reader_s(char *msg, int sem)
{

        int     ret;
        int i;

        kprintf ("  %s: to acquire semaphore\n", msg);
        wait(sem);
        kprintf ("  %s: acquired semaphore\n", msg);
        for(i=0;i<10;i++){
            kprintf("%s ",msg);
        }
        kprintf ("  %s: to release semaphore\n", msg);
        signal(sem);
}

void writer_s(char *msg, int sem)
{


        int i;

        kprintf ("  %s: to acquire semaphore\n", msg);
        wait(sem);
        kprintf ("  %s: acquired semaphore, sleep 10s\n", msg);
        for(i=0;i<10;i++){
            kprintf("%s ",msg);
        }
        sleep (10);
        kprintf ("  %s: to release semaphore\n", msg);
        signal(sem);
}

void semaphoreTest ()
{
 
        int     sem;
        int     rd1, rd2;
        int     wr1;
        int prio_c;

        kprintf("\nAn equivalent comparison with Semaphore\n");
        sem  = screate (1);

        rd1 = create(writer_s, 2000, 20, "reader_s_3", 2, "A", sem);
        rd2 = create(reader_s, 2000, 30, "reader_s_3", 2, "B", sem);
        wr1 = create(writer_s, 2000, 40, "writer_s_3", 2, "C", sem);

        kprintf("-start A, then sleep 1s.\n");
        resume(rd1);
        sleep (1);
        prio_c=getprio(rd1);
        kprintf("\n Prio of writer before rd1: %d", prio_c);
        kprintf("-start C, then sleep 1s.\n");
        resume(wr1);
        sleep (1);
        prio_c=getprio(rd1);
        kprintf("\n Prio of writer after rd1: %d", prio_c);
	    //assert (getprio(wr1) == 25, "Test 3 failed");

        kprintf("-start B, then sleep 1s.\n");
        resume (rd2);
	    sleep (1);
	//assert (getprio(wr1) == 30, "Test 3 failed");
	//shutdown();
        sleep (8);
}

int main(){
    kprintf("Priority Inversion with Locks\n");
    lockTest();
    kprintf("Priority Inversion with Semaphores\n");
    semaphoreTest();
    shutdown();
}
