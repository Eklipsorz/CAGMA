# The toolkits 

It can generate disk-bound workload with some memory-bound task. (The number of disk-bound task is more than the number of memory-bound task) 

The main points for the toolkits are followings:
* FixedAccDisk.c、FixedAccDisk.h is the implementation for disk-bound task
* FixedAccMem.c、FixedAccMem.h is the implementation for memory-bound task

# Usage

1. run "make" command
2. transfer "enable\_test\_CAGMA" in 3FixedDiskTest from 0 to 1
3. run 3FixedDiskTest to generate diskbound workload
