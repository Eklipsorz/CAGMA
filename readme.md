# Background

In the past, when a computer runs out memory, it enables virtual memory to extend disk to memory. This bring a good number of disk-bound requests for accessing the disk, while the performance of the computer is degrade by accessing the disk. Fortunately, these requests cannot affect another computer connected via network or by a similar way. In virtualization platform, however, these requests may degrade another computer or virtual machine (VM) in this platform because each VM shared the disk on physical machine (i.e., disk contention).

My advicer and I propose an dynamic memory management for Xen virtualization platforms, called critical amount guaranteed memory allocation (CAGMA)[1], to expand or shrink the allocated memory of each virtual machine (VM) dynamically with a guaranteed amount of available memory. 

This project consists of VM build tools, which can quickly create a VM image and install OS into that, an implementation of CAGMA and a set of benchmark tools for assessing the capabilities of CAGMA.

# CAGMA

Under CAGMA, a critical memory amount is calculated for each VM periodically and at the time a swapping event is occurred or virtual memory is enabled. The allocated memory of each VM is then adjusted according to its critical memory amount so that the number of I/O requests generated for virtual memory could be reduced greatly and the performance degradation could be prevented.

# The benchmark tools

Our proposed CAGAM has been implemented in Xen 4.2.2 and a series of experiments have been conducted for which some encouraging results were obtained.

# The todo list
1. 


# References
1. Jun Wu and Shou-Liang Sun. "A Dynamic Memory Allocation Approach for Virtualization Platforms." Big Data Security on Cloud (BigDataSecurity), IEEE International Conference on High Performance and Smart Computing (HPSC), and IEEE International Conference on Intelligent Data and Security (IDS), 2017 IEEE 3rd International Conference on. IEEE, 2017.
