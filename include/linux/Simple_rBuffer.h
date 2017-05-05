#ifndef _LINUX_SIMPLE_RING_BUFFER_H
#define _LINUX_SIMPLE_RING_BUFFER_H

#define rBuffer_Size 100
struct __Simple_rBuffer_Entry__
{	
	long long int AVM;
	long long int CMA;
};

typedef struct __Simple_rBuffer_Entry__ Simple_rBuffer_Entry;
#endif
