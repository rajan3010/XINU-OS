#define NUM_LOCKS 50

#define	L_FREE	'\01'		/* this lock is free		*/
#define	L_USED	'\02'		/* this lock is used		*/
#define L_DELETED '\03'     /* this lock does not exist */

#define EXEC '\04'          /* The process is currently executing in this lock */
#define WAITING '\05'       /* The process is currently waiting for this lock */
#define RELEASED '\06' /*This process has been released*/

#define READ '\07' 
#define WRITE '\08'
#define NOTALLOC '\09'

/* Create a lock table similar to semaphore table in sem.h*/
struct lockentry {
    char lockstate; /* L_FREE or L_USED */
    int type[50]; /* READ or WRITE lock maintained for each process, since READ lock can be shared*/
    int exec_state[50]; /* EXEC or WAITING :the execution state for each lock, maintained for each process since lock can be shared for READ*/
    int prio[50]; /* maximum process priority amomgst the process waiting for this lock */
    int wait_time[50]; /* wait time for each process in the lock, to compare incase the priorites are equal */
    int admitted_proc[50]; /*set as 1 for all processes that hold the lock*/
    int waiting_proc[50]; /*set as 1 for all the proc that will be in the waiting queue for this lock*/
    int wait_q_len; /*To maintain the size of the wait q*/
};

#define isbadlock(x) (x>50 || x<0)
extern struct lockentry lck_tab[];
extern int lck_ind; /* For maintaining the lock number */
extern unsigned long ctr1000;

void linit();
int lcreate();
int isWrite(int);
int getHighestWritePrio(int);
int getMaxWaitProc(int);
int calcMaxPrio(int);
int releaseall(int, ...);
int releaseLock(int, int);
void rampPrio(int);
