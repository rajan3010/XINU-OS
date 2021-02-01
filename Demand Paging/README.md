## 1. Introduction
  - Demand paging is a method of mapping a large address space into a relatively small amount of physical memory. It allows a program to
  use an address space that is larger than the physical memory, and access non-contiguous sections of the physical memory in a contiguous
  way. 
  - Demand paging is accomplished by using a "backing store" (usually disk) to hold pages of memory that are not currently in use.

## 2. Goal
  - The goal of this project is to implement the following system calls and their supporting infrastructure.

## 3. System Calls

  - **```SYSCALL xmmap (int virtpage, bsd_t source, int npages) ```**
    - Much like its Unix counterpart (see man ```mmap```), it maps a source file ("backing store" here) of size ```npages``` pages to the virtual page ```virtpage``` . A process may call this multiple times to map data structures, code, etc.
  
  - **```SYSCALL vcreate (int *procaddr, int ssize, int hsize, int priority, char *name, int nargs, long args)```**
    - This call will create a new Xinu process. The difference from ```create()``` is that the process' heap will be private and exist in its virtual memory.
    - The size of the heap (in number of pages) is specified by the user through ```hsize``` .
    - ```create()``` should use ```xmmap()``` . be left (mostly) unmodified. Processes created with ```create()``` should not have a private heap, but should still be able to use ```xmmap()```
    - **```WORD *vgetmem (int nbytes)```**
      - Much like ```getmem()``` , ```vgetmem()``` will allocate the desired amount of memory if possible. The difference is that ```vgetmem()``` will get the memory from a process' private heap located in virtual memory. ```getmem()``` still allocates memory from the regular Xinu kernel heap.
    - **```SYSCALL srpolicy (int policy)```**
      - This function will be used to set the page replacement policy to Second-Chance (SC) or Aging (AGING). You can declare constant SC as 3 and AGING as 4 for this purpose.
    - **```SYSCALL vfreemem (block_ptr, int size_in_bytes)```**

