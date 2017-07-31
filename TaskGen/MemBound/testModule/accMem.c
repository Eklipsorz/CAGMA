#include "Spec.h"
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#define __USE_GNU 
#include <sys/resource.h>

#define PAGE_SIZE 4096

static struct timeval runth_s,runth_e;
static double meanValue = 80;
static double stdDiff = 10;

void sighup()
{
	int configfd;
	double rtime;
	char output[30];
	
	configfd = open("/proc/buffer", O_RDWR);	
	
	if(configfd < 0) {
		perror("open");
		return;
	}
	
	gettimeofday(&runth_e,NULL);
	Timediff(&runth_s,&runth_e,2);
	rtime=(double)runth_e.tv_sec+(double)runth_e.tv_usec/1000000;
	
	if (rtime > 720)
		return;
	
	sprintf(output,"%.2lf\n",rtime);	
	write(configfd, output, strlen(output) + 1);
	close(configfd);
	

}

int main()
{
	
		
	int iter,mUsage,temp = temp;
	double randTemp;
	char *heapMem=NULL;
	
	genRand(&randTemp,1,meanValue,stdDiff);
	mUsage = (int)(round(randTemp*1024));
	printf("%d %d\n",mUsage,iterInTask);	
	gettimeofday(&runth_s,NULL);
	heapMem=(char*)malloc(mUsage*1024*sizeof(char));	

	for(iter=0;iter<mUsage/4;iter++)
		heapMem[iter*4096]=10;

	for(iter=0;iter<iterInTask;iter++)
	{
		if (iter%2==0)		
			heapMem[(iter%(mUsage/4))*4096]=9;	
		else
			temp=heapMem[(iter%(mUsage/4))*4096];
	
		gettimeofday(&runth_e,NULL);
		Timediff(&runth_s,&runth_e,2);
	}
	
	free(heapMem);

	
	return EXIT_SUCCESS;

}
