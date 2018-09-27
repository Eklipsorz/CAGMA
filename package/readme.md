For implementation for CAGMA, there are 3 following targets:
* an implementation for calculating CMA.
* a monitor in host computer to periodically calculate CMA for each VM and adjust memory amount according to CMA  
* an emergency allocation: when there is performance degradation due to lack of memory before the period, the system need to adjust directly memory and calculate CMA
