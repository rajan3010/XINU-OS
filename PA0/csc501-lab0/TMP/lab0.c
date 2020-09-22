#include <stdio.h>
#include <conf.h>
#include <kernel.h>
#include<proc.h>
#include<lab0.h>
long ctr1000;

void syscall_count_timer_start(struct pentry *process, int syscallid){
    process->sc_counter[syscallid]+=1;
    process->sc_start_time[syscallid]+=ctr1000;
}

void syscall_end_timer(struct pentry *process, int syscallid){
    process->sc_end_time[syscallid]+=ctr1000;
}