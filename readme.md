# Background

In the past, when a computer runs out memory, it enables virtual memory to extend disk to memory. This bring a good number of disk-bound requests for accessing the disk, while the performance of the computer is degrade by accessing the disk. Fortunately, these requests cannot affect another computer connected via network or by a similar way. In virtualization platform, however, these requests may degrade another computer or virtual machine(VM) in this platform because each VM shared the disk on physical machine (i.e., disk contention)

# CAGMA


# The workload generator



# The Prograss of this project: 
1. Build AutoKickStart to automatically download clean kernel 		....... 01/30/2017 finished
   and update the kernel with modified install.cfg 			
2. Build MemEventTrigger to monitor 'memory/target' and 'mem-		....... 02/28/2017 finished
   ory/warning' in Xenstore						

3. Modify Kernel 4.2.2 for checking AVM and UMA for each VM 		....... 03/10/2017 finished
   periodically and adjusting memory of each VM according to 
   that :						
	a. CMA Watcher
	b. Memory Adjuster		
 
3. Build a Workload Generator for verifying cabibility of CAGMA:	....... 03/20/2017 finished
	a. Data Collector
	b. Task Generator

4. Optimize Source Code within this project :
	a. Optimize AutoKickStart sub-project 				....... 08/11/2017 finished
	b. Optimize modified kernel 4.2.2
	c. Optimize Workload Generator
