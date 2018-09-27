# The toolkits 

It can generate memory-bound and is split into two types of memory-bound workload according to memory usage:
* 1FixedMemTest generates heavy memory-bound workload
* 2FixedMemTest generates light memory-bound workload

FixedAccMem.c、FixedAccMem.h is the implementation for memory-bound workload.

# Usage of 1FixedMemTest 

1. run "make" command
2. transfer "enable\_test\_CAGMA" in the 1FixedMemTest from 0 to 1
3. run 1FixedMemTest to generate memory-bound workload

# Usage of 2FixedMemTest 

1. run "make" command
2. transfer "enable\_test\_CAGMA" in the 2FixedMemTest from 0 to 1
3. run 2FixedMemTest to generate memory-bound workload
