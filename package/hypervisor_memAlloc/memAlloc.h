#ifndef __memAlloc_H_
#define __memAlloc_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <libxl.h>
#include <xenstore.h>
#include <xenstat.h>

#define minmsglevel XTL_PROGRESS
#define NRsample 5			/* collect the last NRsample amounts (AVM - CMA) */
#define threshold_relea 10
#define Mem_minimum 262144		/* Minimum memory amount */
#define Mem_maximum 1536000		/* Maxmium memory amount */
#define Alloc_rate 1.0			/* Avoid thrashing via allocating more memory to vm */
#define MAX(a,b) (((a)>(b))?(a):(b))

/* 
 * define a structure as the template for each vm. In other word, 
 * each instance from this template is represented each existing vm
 */  
struct __entry__{

	int id;					/* domain id or vm id 					 */	
	int64_t CMA;				/* critical memory amount 				 */
	int64_t AVM; 				/* unused memory amount 				 */
	int64_t ALM;				/* allocated memory amount 				 */
	int64_t amount;				/* allocatable memory amount or releasable memory amount */
	int64_t sample[NRsample];		/* last NRsample amounts				 */	
	char path[50];				/* the path of each vm in xenstore 			 */
	int releaCount;				/* sequental times for releasing memory			 */
	struct __entry__ *next, *prev;		/* can link next node or previous node 			 */
};

/* redefine */
typedef struct __entry__ entry;

extern entry *Alloc;
extern entry *Relea;
/* import two variables into this header from memAlloc */
extern xenstat_node *cur_node;
extern xenstat_handle *xhandle;

#endif
