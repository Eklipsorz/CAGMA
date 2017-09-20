#include "FixedAccMem.h"

#define PAGE_SIZE 4096

static struct timeval runth_s,runth_e;


extern void Timediff(struct timeval *src,struct timeval *des,int type);
extern void Timeadd(struct timeval *src,struct timeval *des);

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
 
	sprintf(output,"%d",(int)(rtime*1000));    
	write(configfd, output, strlen(output) + 1);
	close(configfd);
}

static void timer_handler()
{
	int resultfd;
	double rtime;
	char output[30];
	
	resultfd = open("/proc/buffer", O_RDWR);    
	
	if(resultfd < 0) {
		perror("open");	
		return;
	}
	


	gettimeofday(&runth_e,NULL);
	Timediff(&runth_s,&runth_e,2);
	rtime=(double)runth_e.tv_sec+(double)runth_e.tv_usec/1000000;
	
	if (rtime > 720)
		return;
	sprintf(output,"%d",(int)(rtime*1000));
	write(resultfd, output, strlen(output) + 1);
	close(resultfd);

}

int main(int argc, char *argv[])
{
	
	int iter,mUsage,temp = temp;
	struct itimerval timer;
	struct sigaction sa;
	double rtime = rtime;
	char *heapMem = NULL;
	char input = 'a';

	mUsage = 49152;	
	
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &timer_handler;
	sigaction(SIGALRM,&sa,NULL);
	
	timer.it_value.tv_sec = 3;
	timer.it_value.tv_usec = 0;

	timer.it_interval.tv_sec = 3;
	timer.it_interval.tv_usec = 0;
//	signal(SIGHUP,sighup);	
	setitimer(ITIMER_REAL,&timer,NULL);
	
	heapMem = (char*)malloc(mUsage*1024*sizeof(char));	

	for (iter = 0; iter < mUsage/4;iter++)
		heapMem[iter*4096] = 10;
	
	while(1)
	{
		
		gettimeofday(&runth_s,NULL);
		for(iter = 0;iter < iterInTask; iter++)
		{
			int pos;
			
			input = input + (rand() % 26);		
			pos = (rand() % (mUsage/4));		
			
			if (iter%2 == 0)		
				heapMem[pos*4096] = input;	
			else
				temp = heapMem[pos*4096];
			temp = temp + ( rand() % 26 );
			
			gettimeofday(&runth_e,NULL);
			
		}
		
		Timediff(&runth_s,&runth_e,2);
	
	}
	
	free(heapMem);
		
	return EXIT_SUCCESS;

}
