For implementation for CAGMA, there are 3 following targets:
* an implementation for calculating CMA.
* a monitor in host computer to periodically calculate CMA for each VM and adjust memory amount according to CMA  
* an emergency allocation: when there is performance degradation due to lack of memory before the period, the system need to adjust directly memory and calculate CMA


The implementation of first target is stored into <strong> hypervisor/hypervisor_memAlloc/ </strong>, the senond is stored into <strong> hypervisor/hypervisor_memAlloc/ </strong> and <strong> package/domainU/domainU_supCenter/supCenter.c </strong>
(p.s. CMA calculation needs additional information for each VM but there is no way to directly obtain. Therefore, I design a daemon in each VM and the daemon send the necessary information periodically via xenstore daemon) and last one is stored into <strong> domainU/domainU_kernel/kernel/mm/vmscan.c , domainU_kernel/kernel/drivers/xen/cagma-mem-requester.c domainU_kernel/kernel/drivers/xen/balloon.c </strong> 
