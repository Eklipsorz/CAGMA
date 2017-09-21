
#ifndef SPEC
#define SPEC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <math.h>
#include <time.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>
 
// choose whether seqAccess or randAccess?
#define SEQACCESS  
//#define RANDACCESS
//
#define sizeOfDomi domisize() 
#define MAXRCVLEN 512
#define PORTNUM 8111
//#define taskNum 100000 // unit: N tasks/sec
#define memUsage 102400 // the memory usage for each thread 
//#define iterInTask 96408 // the execution iteration for each thread
#define testUtil 0.5
#define iterInTask 85733 // the execution iteration for each thread
// 135733 = 1/3
//#define iterInTask 105733 // the execution iteration for each thread
//552582 = 1s
//138092 = 0.25s
//42000 =0.25s
#define period 1000000*0.25
//3750
#define exptime 10
#define restTime 5000000 // 5 * 1000000
#define size_dbase 102400  // the size of the pool 
//#define PeriodInMemUsg 1
//#define memoryStock 25000




typedef struct QueueEntry
{	
	double rtime;
	double MemU;
	double nr_majpgf;
	int Num;
	struct QueueEntry *next;

} entry;

typedef struct Queue
{
	int version;
	int test;
	struct QueueEnter *head;
	//void (*add)();	
}queue;

typedef struct Data
{	
	double nr_majpgf;
	double nr_memU;
} pooldata;

typedef struct recData{
	double memU;
	double nr_majpgf;
}recdata;

typedef struct sharedPool{
	pooldata data[101];
}pool;

typedef struct sharedMem{
	char data[memUsage*1024];
}shedmem;




extern int nr_item;
extern int version;
extern int critimemU;
extern int zero_case;
extern int nonzero_case;
extern entry *head,*tail;

extern void Timediff(struct timeval *src,struct timeval *des,int type);
extern void Timeadd(struct timeval *src,struct timeval *des);
extern void genRand(double *,int,double,double);
extern void Queu2Aray(double *a,double *b);
extern int num_Data_For_majPGf();
extern void listEntry();
extern void addEntry();
extern int getEntry();
extern void add_item_into_pool(double pgf,int MemU,pool*pol);
extern void init_pool(pool *pol);
extern int find_critimemU_from_pool(pool *pol);
extern int check_numof_nonzero_and_zero(pool *pol,int memU);
#endif
