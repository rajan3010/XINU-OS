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
        For example, let us assume that the scheduler uses a random value with the exponential distribution of λ = 0.1, and there are three processes A, B, and C, whose priorities are 10, 20, and 30, respectively. When rescheduling happens, if a random value is less than 10, process A will be chosen by the scheduler. If the random value is between 10 and 20, the process B will be chosen. If the random value is no less than 20, process C will be chosen. The probability that a process is chosen by the scheduler follows the exponential distribution. As shown in the figure below, the ratio of processes A (priority 10), B (priority 20), and C (priority 30) to be chosen is 0.63 : 0.23 : 0.14. It can be mathematically calculated by the cumulative distribution function F(x; λ) = 1 - e -λx .
        ```
    
        ![exponential](https://github.com/rajan3010/XINU-OS/tree/master/Process%20Scheduling/csc501-lab1/images/expo_schedule.png)