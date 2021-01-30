# XINU-OS

# Reader/ Writer Locks with Priority Inheritance

## 1. Introduction

- __Goal__

  - In this project we are going to implement readers/ writers locks as descibed in this README.
  - Additionally, we will implement a priority inheritance mechanism to prevent the priority inversion problem when using locks.

- __Specifications/ Challenges__
  
  - Reader/ writer locks are used to synchronize access to a shared data structure. A lock can be acquired for read or write operations.
  - A lock acquired for reading can be shared by other readers, but a lock acquired for writing must be exclusive.
  - A standard **semaphore** implementation for XINU has been provided (wait.c, signal.c, screate.c, sdelete.c, etc.) which must not be modified.
  -The semaphore implementation shouldn't be modified since they are used in rest of the kernel, eg. in device drivers.
  - However there are certain limitations in the existing semaphores.
  -  XINU semaphores do not distinguish between read accesses, which can co-exist, and write accesses, which must be exclusive.
  - Another 
