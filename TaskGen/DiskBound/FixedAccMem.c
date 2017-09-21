/* 
 * This program can simulate the execution of a memory-bound task. Under the simulation, 
 * Each task writes its the residence time into /proc/buffer for each 3 seconds. 
 * When /proc/buffer is written, the guest OS indirectly writes the file storing data 
 * of each memory-bound task. 
 */

#include "FixedAccMem.h"

#define PAGE_SIZE 4096


char *heapMem = NULL;

static struct timeval runth_s,runth_e;

/* import Timediff() and Timeadd() from other file */
extern void Timediff(struct timeval *src,struct timeval *des,int type);
extern void Timeadd(struct timeval *src,struct timeval *des);

/* set a callback function of signal sighup */
/* when a signal is sent to here, the task is allowed to exit the loop */
void sighup()
{
	free(heapMem);
	exit(0);
}

/* set a callback function of a timer */ 
/* 
 * When this function is called, it calculates its residence time and 
 * writes the residence time into /proc/buffer
 */
static void timer_handler()
{
	int resultfd;
	double rtime;
	char output[30];
	/* require for accessing /proc/buffer */
	resultfd = open("/proc/buffer", O_RDWR);    
	
	if(resultfd < 0) {
		perror("open");	
		return;
	}
	

	/* record the end time of each task */
	gettimeofday(&runth_e,NULL);
	
	/* calculate the residence time of each memory-bound task */
	/* 
	 * The residence time is the difference between the beginning time 
	 * and the end time 
	 */
	Timediff(&runth_s,&runth_e,2);
	rtime=(double)runth_e.tv_sec+(double)runth_e.tv_usec/1000000;
	

	/* write the residence time into /proc/buffer */
	sprintf(output,"%d",(int)(rtime*1000));
	write(resultfd, output, strlen(output) + 1);
	close(resultfd);

}

/* The main function of this program */
int main(int argc, char *argv[])
{
	
	int iter,mUsage,temp = temp;
	struct itimerval timer;
	struct sigaction sa;
	double rtime = rtime;
	char input = 'a';


	/* set a callback function of a timer */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &timer_handler;
	sigaction(SIGALRM,&sa,NULL);
	
	/* This is the period between now and the first timer interrupt. If zero, the alarm is disabled. */
	timer.it_value.tv_sec = 3;
	timer.it_value.tv_usec = 0;

	/* This is the period between successive timer interrupts. If zero, the alarm will only be sent once. */
	timer.it_interval.tv_sec = 3;
	timer.it_interval.tv_usec = 0;
	
	/* set a timer to periodically collect the data */
	setitimer(ITIMER_REAL,&timer,NULL);
	
	/* set a callback function to the function sighup */
	signal(SIGHUP,sighup);	

	/* set the memory usage for each memor-bound task */
	mUsage = 49152; 	/* This value is in kb unit */	
	
	/* The memory usage is fixed to mUsage via pre-allocating memory */
	heapMem = (char*)malloc(mUsage*1024*sizeof(char));	

	/* Initialize the pre-allocated memory */
	for (iter = 0; iter < mUsage/4;iter++)
		heapMem[iter*4096] = 10;

	/* set a infinite loop to prevent decrease of the number of tasks at same time */
	/* if the boolean ready_to_exit is 0, the task jump out from this loop */
	while(1)
	{
		/* record the beginning time of each task */	
		gettimeofday(&runth_s,NULL);
		for(iter = 0;iter < iterInTask; iter++)
		{
			int pos;
		
			/* random access */
			input = input + (rand() % 26);		
			pos = (rand() % (mUsage/4));		
			
			if (iter%2 == 0)		
				heapMem[pos*4096] = input;	
			else
				temp = heapMem[pos*4096];
			temp = temp + ( rand() % 26 );
	
			/* update the beginning time of each task */
			gettimeofday(&runth_e,NULL);
			
		}
		
		Timediff(&runth_s,&runth_e,2);
	
	}
	
	return EXIT_SUCCESS;

}
