# XINU-OS

# Reader/ Writer Locks with Priority Inheritance

## 1. Introduction

- **Goal**

  - In this project we are going to implement readers/ writers locks as descibed in this README.
  - Additionally, we will implement a priority inheritance mechanism to prevent the priority inversion problem when using locks.

- **Specifications/ Challenges**
  
  - Reader/ writer locks are used to synchronize access to a shared data structure. A lock can be acquired for read or write operations.
  - A lock acquired for reading can be shared by other readers, but a lock acquired for writing must be exclusive.
  - A standard **semaphore** implementation for XINU has been provided (wait.c, signal.c, screate.c, sdelete.c, etc.) which must not be modified.
  -The semaphore implementation shouldn't be modified since they are used in rest of the kernel, eg. in device drivers.
  - However there are certain limitations in the existing semaphores.
  - XINU semaphores do not distinguish between read accesses, which can co-exist, and write accesses, which must be exclusive.
  - Another problem with XINU's semaphores occurs when a semaphore is deleted at a time when it has processes waiting in the queue. In such a situation, *sdelete* awakens all the waiting processes by moving them from the semaphore queue to the ready list. As a result, a process that is waiting for some event to occur will be awakened, even though the event has not yet occurred.
  - Yet another problem that occurs due to the interactions between process synchronization and process scheduling is priority inversion. Priority inversion occurs when a higher priority thread is blocked waiting on a lock (or a semaphore) held by a lower priority thread. This can lead to erroneous system behavior, especially in real time systems.
  - Task here is to extend the avaialble semaphore implementation to handle these limitations.

- **Solution**
  - There are many solutions in the [literature](https://en.wikipedia.org/wiki/Priority_inversion) to solve the problem of priority inversion.
  - I have chosen priority inheritance protocol for solving this problem of priority inversion.

## 2. Interfaces to Implement

- **Basic Locks**
  
  - We will be implementing the entire readers/ writer lock system. This includes code or functions to 
    - Initialize - ```linit()``` from ```sysinit()``` in ```initialize.c```
    - Create and destroy - ```lcreate``` and ```ldelete```
    - acquire a lock and release multiple locks (```lock``` and ```releaseall```) 

-  **Implementation**

