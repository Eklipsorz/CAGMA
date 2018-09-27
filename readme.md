# Background

In the past, when a computer runs out memory, it enables virtual memory to extend disk to memory. This bring a good number of disk-bound requests for accessing the disk, while the performance of the computer is degrade by accessing the disk. Fortunately, these requests cannot affect another computer connected via network or by a similar way. In virtualization platform, however, these requests may degrade another computer or virtual machine (VM) in this platform because each VM shared the disk on physical machine (i.e., disk contention).

My advicer and I propose an dynamic memory management for Xen virtualization platforms, called critical amount guaranteed memory allocation (CAGMA)[1], to expand or shrink the allocated memory of each virtual machine (VM) dynamically with a guaranteed amount of available memory. 

This project consists of VM build tools, which can quickly create a VM image and install OS into that, an implementation of CAGMA and a set of self-made benchmark tools for assessing the capabilities of CAGMA.

# CAGMA

Under CAGMA, a critical memory amount (CMA) is calculated for each VM periodically and at the time a swapping event is occurred or virtual memory is enabled. The allocated memory of each VM is then adjusted according to its critical memory amount so that the number of I/O requests generated for virtual memory could be reduced greatly and the performance degradation could be prevented. The allocation periodically calculates CMA for each VM, determines which VM needs more memory and doesn't need more with CMA and finally adjust their memory amount according to CMA. Except for periodical memory adjustment, we also provide an emergency allocation: when there is performance degradation due to lack of memory before the period, the system directly adjusts the memory and calculates the CMA.

For implementation for CAGMA, there are 3 following targets:
* an implementation for calculating CMA.
* a monitor in host computer to periodically calculate CMA for each VM and adjust memory amount according to CMA  
* an emergency allocation: when there is performance degradation due to lack of memory before the period, the system need to adjust directly memory and calculate CMA


We use a set of self-made daemon, xenstore daemon, kswap daemon and balloon driver to complete these targets and store it into <strong> package </strong> dir.

## 


# The benchmark tools


This benchmark


This benchmark tools is designed to assess capability of CAGMA based on a testbed. The testbed was built on a server with two dual-core processors (i.e., two Intel Core 2 Duo proces- sors ). The frequency of each processor is 2333MHz, and each has 2MB cache. The memory size is 4096MB and the OS is CentOS 7 64-bit (kernel version is Linux 4.2.2). Our proposed CAGMA has been implemented in Xen 4.2.2. The guest OS of each VM is CentOS 6 (kernel version is also Linux 4.2.2) and number of VM is 3.

To better observe, we set the <srong> residence time and I/O activities per second (IOPS) </strong> as performance metrics. The residence time and IOPS are represented to the average time all task reside in the system and the number of I/O requests been done per second. The residence time is growing as the IOPS is being increased or more I/O requests have not been done.

In this benchmark tools, we inoke multiple processes with signal handling to generate two types of workload to test: memory-bound worklord and disk-bound workload. Each process periodically write the collected metrics into the <strong> buffer in 
/proc dir </strong> and the event-driven driver of the buffer collects these data from that. Additionally, two of the VMs are tested under memory-bound workload and the other is tested under disk-bound workload.

The implementation for this part store in <strong> benchmark </strong> dir.

# The todo list
1. 


# References
1. Jun Wu and Shou-Liang Sun. "A Dynamic Memory Allocation Approach for Virtualization Platforms." Big Data Security on Cloud (BigDataSecurity), IEEE International Conference on High Performance and Smart Computing (HPSC), and IEEE International Conference on Intelligent Data and Security (IDS), 2017 IEEE 3rd International Conference on. IEEE, 2017.
