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

   - Create a lock: ```int lcreate (void)```- Creates a lock and returns a lock descriptor that can be used in further calls to refer to this lock. THis call should return **SYSERR** if there are no available entries in the lock table. The number of locks allowed is NLOCKS, which you should define in ```lock.h``` to be 50
   - Destroy a lock: ```int ldelete(int lockdescriptor)``` - Deletes the lock identified by the descripto ```lockdescriptor```. Explained further in "Lock Deletion" section.
   - Acquisiton of a lock for read/ write: ```int lock (int ldes1, int type, int priority)``` - This call is explained below ("Wait on locks with Priority").
   - Simultaneous release of multiple locks: ```int releaseall (int numlocks, int ldes1, ..., int ldesN)```

- **Lock Deletion**

  - As mentioned in one of the drawbacks with XINU semaphores, the way XINU handles ```sdelete``` may have undesirable effects if a semaphore is deleted while a process or processes are waiting on it.
  - Examining the code for wait and sdelete, you will notice that ```sdelete``` readies processes waiting on a semaphore being deleted. So they will return from wait with OK

  - Edge cases
    - Lock system is implemented such that waiting on a lock will return a new constant DELETED instead of OK when returning due to deleted lock.
    - This will indicate to the user that the lock was deleted and not unlocked. As before, any calls to lock() after the lock is deleted should return SYSERR
    - There is also another subtle but important point to note
    ```
    Consider the following scenario.Let us say that there are three processes A, B,
    and C.Let A create a lock with descriptor=X .Let A and B use X to synchronize among themselves. Now, let us assume that A
    deletes the lock X. But B does not know about that. If, now, C tries to create a lock, there is a chance that it gets the same lock descriptor as that of X (lock descriptors are limited and hence can be reused). When B waits on X the next time, it should get a SYSERR. It should not acquire the lock C has now newly created, even if this lock has the same id as that of the previous one. You have to find a way to implement this facility, in addition to the DELETED issue above
    ```
- **Locking Policy**

  - no readers should be kept waiting unless
    - a writer has already obtained the lock
    - there is a higher priority writer already waiting for the lock
  - Hence, when a writer or the last reader releases a lock, the lock should be next given to a process having the highest waiting priority for the lock.
  - In the case of equal waiting priorities, the lock will be given to the process that
has the longest waiting time (in milliseconds) on the lock.
  - If the waiting priorities are equal and the waiting time difference is within 1
second, writers should be given preference to acquire the lock over readers.
  - In any case, if a reader is chosen to have a lock, then all the
other waiting readers having priority not less than that of the highest-priority waiting writer for the same lock should also be admitted.

- **Wait on Locks with Priority**

  - This call allows a process to wait on a lock with priority. The call will have the form as below, where priority is any integer priority value (including negative values, positive values and zero)
    ```
    int lock (int ldes1, int type, int priority)
    ```
  - Thus when a process waits, it will be able to specify a wait priority. Rather than simply enqueuing the process at the end of the queue, the lock() call should now insert the process into the lock's wait list according to the wait priority.
  - Please note that the wait priority is different from a process's scheduling priority specified in the create(..) system call. A larger value of the priority parameter means a higher priority.
