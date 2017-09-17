
#ifdef CONFIG_CAGMA_MEMORY_REQUESTER

extern bool is_less_than_maxALM;
extern bool can_provide_mem;
extern bool enable_to_run_memAlloc;
extern long long int Mmax;
extern long long int CMA;

void cagma_memory_requester_worker_gen(void);

#endif
