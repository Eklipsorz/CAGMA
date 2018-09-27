
memAlloc.c is the main program for calculating CMA, determine and adjust memory in each period and queue.c is the implementation for queue storing the info of VM.

In each period, this program enqueue VM into one of two queues according to their memory usage. One queue stores the info of all VMs which needs more memory and another one stores the info of all VMs which doesn't need more memory. Finally, the system dequeue from these two qeues and adjust its memory according CMA.
