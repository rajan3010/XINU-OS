# XINU-OS

# Exponential and Linux Like Process Scheduling

## 1. Introduction

 - **Objective**
   - The objective of this assignment is to get familiar with the concepts of process management, including process priorities, scheduling, and context switching.

## 2. **Reading**
   - The Xinu source code for functions in sys/, especially those related to process creation (create.c), scheduling (resched.c, resume.c suspend.c), termination (kill.c), changing priority (chprio.c), system initialization (initialize.c), and other related utilities (e.g , ready.c), etc.

## 3. **What To Do**
   - In this assignment, I implemented two new scheduling policies, which avoid the starvation problem in process scheduling. At the end of this assignment, I was able to explain the advantages and disadvantages of the two new scheduling policies.
   - The default scheduler in Xinu schedules processes based on the highest priority. Starvation occurs when two or more processes that are eligible for execution have different priorities.
   - The process with the higher priority gets to execute first, resulting in processes with lower priorities never getting any CPU time unless process with the higher priority ends.
   - The two scheduling policies that you need to implement, as described as follows, should address this problem.
   - Note that for each of them, we need to consider how to handle the NULL process, so that this process is selected to run when and only when there are no other ready processes.
   - For Linux-like scheduling policies, the value of a valid process priority is an integer between 0 to 99, where 99 is the highest priority.

   - **Exponential Distribution Scheduler**
     - The first scheduling policy is the exponential distribution scheduler. This scheduler chooses the next process based on a random value that follows the exponential distribution.
     - When a rescheduling occurs, the scheduler generates a random number with the exponential distribution, and chooses a process with the lowest priority that is greater than the random number. If the random value is less than the lowest priority in the ready queue, the process with the lowest priority is chosen.
     - If the random value is no less than the highest priority in the ready queue, the process with the largest priority is chosen. When there are processes having the same priority, they should be scheduled in a round-robin way.

        ```
        For example, let us assume that the scheduler uses a random value with the exponential distribution of λ = 0.1, and there are three processes A, B, and C, whose priorities are 10, 20, and 30, respectively. 
        When rescheduling happens, if a random value is less than 10, process A will be chosen by the scheduler. If the random value is between 10 and 20, the process B will be chosen. If the random value is no less than 20, process C will be chosen. 
        wThe probability that a process is chosen by the scheduler follows the exponential distribution. As shown in the figure below, the ratio of processes A (priority 10), B (priority 20), and C (priority 30) to be chosen is 0.63 : 0.23 : 0.14. It can be mathematically calculated by the cumulative distribution function F(x; λ) = 1 - e -λx .
        ```
    
        ![exponential](./images/expo_schedule.png)
    
    
     - In order to implement an exponential distribution scheduler, I implmented my own function expdev, which generates random numbers that follow the exponential distribution.

        ```c
        double expdev (double lambda){
           double dummy;
           do
              dummy=(double) rand() / RAND_MAX;
           while (dummy==0.0);
           return -log(dummy) /lambda
        }
        ``` 
     - XINU doesn't provide any math library, so I implemented log() by myself. For log() implementation, I used [Taylor Series](http://en.wikipedia.org/wiki/Taylor_series), where n=20 gave a reasonable approximation.
     - On the other hand, srand() and rand() functions are offered in lib/libxc/rand.c by Xinu.

       ```c
       double log(double x);
       double pow(double x, int y);
       double expdev(double lambda);
       ```
     - Comment: As the value of λ, use 0.1. The mean of the exponential distribution is 1/λ, so it is 10 for λ = 0.1. Since the priority ranges from 0 to 99, 10 can be meaningful. The λ value may be changed for a different scheduling property, but your code will be tested for λ = 0.1.
   - **Linux-like Scheduler (based loosely on the Linux kernel 2.2)**
     - This scheduling algorithm loosely emulates the Linux scheduler in the 2.2 kernel. We consider all the processes "conventional processes" and use the policies of the SCHED_OTHER scheduling class within the 2.2 kernel.
     - In this algorithm, the scheduler divides CPU time into continuous epochs. In each epoch, every existing process is allowed to execute up to a given time quantum, which specifies the maximum allowed time for a process within the current epoch.
     - The time quantum for a process is computed and refreshed at the beginning of each epoch. If a process has used up its quantum during an epoch, it cannot be scheduled until another epoch starts. For example, a time quantum of 10 allows a process to only execute for 10 ticks (10 timer interrupts) within an epoch. On the other hand, a process can be scheduled many times as long as it has not used up its time quantum.
     - For example, a process may invoke sleep before using up its quantum, but still be scheduled after waking up within an epoch. The scheduler ends an epoch and starts a new one when all the runnable processes (i.e., processes in the ready queue) have used up their time.
     - The rules for computing the time quantum for a process are as follows. At the beginning of a new epoch, the scheduler computes the time quantum for **every** existing processes, including the blocked ones. As a result, a blocked process can start in the new epoch when it becomes runnable.
     - For a process that has never executed or used up its time quantum in the previous epoch, its new quantum value is set to its process priority (i.e., quantum = priority ). For a process that has not used up its quantum in the previous epoch, the scheduler allows half of the unused quantum to be carried over to the new epoch. Suppose for each process there is a variable counter describing how many ticks are left from its previous quantum, then the new quantum value is set to floor(counter / 2) + priority . For example, a counter of 5 and a priority of 10 produce a new quantum value of 12. Any new processes created in the middle of an epoch have to wait until the next epoch to be scheduled. Any priority changes in the middle of an epoch, e.g., through create() and chprio() , only take effect in the next epoch.
     - To schedule processes during each epoch, the scheduler introduces a goodness value for each process. This goodness value is essentially a dynamic priority that is updated whenever the scheduler is invoked (**hint**: consider when the scheduler could be invoked). For a process that has used up its quantum, its goodness value is 0.
     - For a runnable process, the scheduler considers both the priority and the remaining quantum (recorded by counter ) in computing the goodness value: goodness = counter + priority (**hint**: when you implement this, you need to consider the rule that any priority changes only take effect in the next epoch).
     - Examples for how processes should be scheduled:
       - If there are processes P1, P2, P3 with priority 10, 20, 15, the initial time quantum for each process would be P1 = 10, P2 = 20, and P3 =15, so the maximum CPU time for an epoch would be 10 + 20 + 15 = 45. The possible schedule for two epochs would be P2, P3, P1, P2, P3, P1, but not: P2, P3, P2, P1, P3, P1.
       - If P1 yields in the middle of its execution (e.g., by invoking sleep ) in the first epoch, the time quantum for each process in the second epoch could be P1 = 12, P2 = 20, and P3 = 15, thereby the maximum CPU time would be 12 + 20 + 15 = 47.
   - **Other Implementation Details**
       - ```void setschedclass (int sched_class)``` - This function should change the scheduling type to either EXPDISTSCHED or LINUXSCHED.
       - ```int getschedclass()```- This function should return the scheduling class, which should be either EXPDISTSCHED or LINUXSCHED.
       - Each of the scheduling class should be defined as a constant:
          ```c
          #define EXPDISTSCHED 1
          #define LINUXSCHED 2
          ```
       - Modifications were done to ```create.c```, ```resched.c```, ```resume.c```, ```suspend.c```, ```ready.c```, ```proc.h```, ```kernel.h```, etc.
## 4. Test Cases
  - **For Exponential Distribution Scheduler:**
    - Three processes A, B, and C are created with priorities 10, 20, 30. In each process, we keep increasing a variable, so the variable value is proportional to CPU time. Note that the ratio should be close to 0.63 : 0.23 : 0.14, as the three processes are scheduled based on the exponential distribution. You are expected to get similar results as follows:
      ```
      Start...A
      Start...B
      Start...C

      Test Result: A = 982590, B = 370179, C = 206867
      ```
   - For Linux-like Scheduler:
     - Three processes A,B and C are created with priorities 5, 50 and 90. In each process, we print a character ('A', 'B', or 'C') at a certain time interval for LOOP times ( LOOP = 50 ). The main process also prints a character ('M') at the same time interval. You are expected to get similar results as follows:

       ```
       MCCCCCCCCCCCCCBBBBBBBMMMACCCCCCCCCCCCCBB
       BBBBBMMMACCCCCCCCCCCCBBBBBBBMMMACCCCCCCC
       CCCCBBBBBBBMMBBBBBBBMMMABBBBBBBMMMABBBBB
       BBMMMBMMAMMMAMMMMMAMMMAMMMAMMMMMMAMMAMMM
       MMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
       ```