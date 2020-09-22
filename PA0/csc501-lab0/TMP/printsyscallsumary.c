#include <stdio.h>
#include <conf.h>
#include <kernel.h>
#include<proc.h>
#include<lab0.h>

int IS_SYSCALL_TRACE=0;

void syscallsummary_start(){

    kprintf("\n\nSyscall summary will start tracing \n");

    IS_SYSCALL_TRACE=1;
    int procno;
    int syscall;
    for(procno=0; procno < NPROC; procno++){
        struct pentry *process;
        process= &proctab[procno];

        for(syscall=0; syscall<27; syscall++){
            process->sc_start_time[syscall]=0;
            process->sc_end_time[syscall]=0;
            process->sc_counter[syscall]=0;

        }
    }
}

void syscallsummary_stop(){

    kprintf("\n\nSyscall summary will stop tracing \n");

    IS_SYSCALL_TRACE=0;

}

char syscall_name[27][50]={"freemem", "chprio", "getpid", "getprio", "gettime", "kill", "receive", "recvclr", "recvtim", "resume", "scount", "sdelete", "send", "setdev", "setnok", "screate", "signal", "signaln", "sleep", "sleep10", "sleep100", "sleep1000", "sreset", "stacktrace", "suspend", "unsleep", "wait"};
void printsyscallsummary(){

    struct pentry *process;
    kprintf("\nprint call summary");
    int proc=0, syscall=0;
    long exec_time=0;

    for(proc=0; proc<NPROC; proc++){
        process=&proctab[proc];
        if(strcmp(process->pname,"")!=0){
            kprintf("\n\n***********************************************");
            kprintf("\nProcess Name: %s", process->pname);
            kprintf("\n***********************************************");
            for(syscall=0;syscall<27;syscall++){
                exec_time=(process->sc_end_time[syscall]-process->sc_start_time[syscall])/(process->sc_counter[syscall]);
                if(process->sc_counter[syscall]>0 && exec_time>0){
                    kprintf("\n%s\tfrequency:%d\taverage execution time:%d\n", syscall_name[syscall], process->sc_counter[syscall], exec_time);
                }
                
            }
        }

    }


}
