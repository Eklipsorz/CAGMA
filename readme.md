# Background
Virtualization technology is the foundation of cloud computing because of these advantages. Recently, it is increasingly being used in applica- tion in cloud computing. For example, Google Compute Engine. Despite the fact that virtualization technique seems to be pretty practical, it may has a performance degradation problem resulted from insufficient memory between VMs. In virtualization platform, a VM enables virtual memory technology to generate a lot of disk-bound tasks to access as a physical machine does when it occurs insufficient memory, while these disk-bound tasks degrade the performance of the VM. However, if there some other VMs or a single VM accessing disk, a VM enabling virtual memory inference another VM because of shared disk.

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
