#include<sched.h>

int sched_class;

void setschedclass(int sc){
    sched_class=sc;
}

int getschedclass(){
    return sched_class;
}