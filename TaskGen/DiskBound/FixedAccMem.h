
#ifndef SPEC
#define SPEC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <mqueue.h>

#include <signal.h>
 
// choose whether seqAccess or randAccess?
#define SEQACCESS  
//#define RANDACCESS
//
#define iterInTask 85733 // the execution iteration for each thread


extern void Timediff(struct timeval *src,struct timeval *des,int type);
extern void Timeadd(struct timeval *src,struct timeval *des);
#endif
