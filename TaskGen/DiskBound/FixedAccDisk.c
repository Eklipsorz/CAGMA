/* 
 * This program can simulate the execution of a disk-bound task. Under the simulation, 
 * Each task writes its the residence time into /proc/buffer for each 3 seconds. 
 * When /proc/buffer is written, the guest OS indirectly writes the file storing data 
 * of each disk-bound task. 
 */


#define _GNU_SOURCE

#include "FixedAccDisk.h"

/* set the size of accessed section */
#define sectorSize 2097152

/* set the number of iterations for each task */
#define execIter 3

int fd;
static struct timeval runth_s,runth_e;

/* import Timediff() and Timeadd() from other file */
extern void Timediff(struct timeval *src,struct timeval *des,int type);
extern void Timeadd(struct timeval *src,struct timeval *des);

/* set a callback function of signal sighup */
/* when a signal is sent to here, the task is allowed to exit the loop */
void sighup()
{
	close(fd);
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


void randAccess(int fd,char *readtemp, char *writemp)
{
	//
	int j, rc, SectNo = SectNo;
	/* It can randomly read from disk and write data into disk
	 * the execution time:
	 */
	srand(time(NULL));
	for(j = 0; j < execIter; j++)
	{
		
		SectNo = rand()%512;
		
		if(j%2)
		{
			if ( (rc = read(fd,readtemp,sectorSize) == -1 ))
				printf("read failed\n");
		}
		else
		{
			if ( (rc = write(fd,(void*)writemp,sectorSize) == -1 ))
				printf("write failed\n");
		}

	}

}

//void seqAccess(FILE *fp,char *readtemp, char *writemp)
void seqAccess(int fd,char *readtemp, char *writemp)
{
	int j,rc;	
	/* It can sequentially read from disk and write data into disk
	 * the execution time:
	 */
	for(j = 0; j < execIter; j++)
	{
		lseek(fd,j*sectorSize,SEEK_SET);
		if(j%2)
		{
			if ( (rc = read(fd,readtemp,sectorSize) == -1 ))
				printf("read failed\n");
		}
		else
		{
			if ( (rc = write(fd,(void*)writemp,sectorSize) == -1 ))
				printf("write failed\n");
		}

	}

}

/* The main function of this program */
int main()
{
	int i;
	struct itimerval timer;
	double rtime = rtime;
	char *buff_read, *buff_write;
	struct sigaction sa;


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
	
	//open family 
	//read: read data from file
	// 		int  read(  int  handle,  void  *buffer,  int  nbyte );
	//write: write data into file
	// 		int  write(  int  handle,  void  *buffer,  int  nbyte  );
	//fopen family
	//fgets: read data from file
	//fprintf: write data into file
	
	/* initialize some buffers for writing and reading disk */
	buff_read = (char *) mmap(NULL, sectorSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	buff_write = (char *) mmap(NULL, sectorSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	
	/* initialize the buffer buff_write */
	for(i=0;i<sectorSize;i++)	
		buff_write[i] = '1' + i % 26;

	/* require for accessing the disk */
	if( (fd = open("../../myrandom",O_RDWR | O_DIRECT | O_SYNC | O_RSYNC)) == -1 )
	{
		//O_DIRECT
		printf("The file isn't exist\n");
		return 0;
	}
	
	/* set a infinite loop to prevent decrease of the number of tasks at same time */
	/* if the boolean ready_to_exit is 0, the task jump out from this loop */
	while(!ready_to_exit)
	{	
		/* record the beginning time of each task */
		gettimeofday(&runth_s,NULL);

		/* check whether SEQACCESS is enabled */
		/* if so, then it sequentially accesses the disk */
		/* if not, then it randomly accesses the disk */
		#ifdef SEQACCESS	
		seqAccess(fd,buff_read,buff_write);
		#else
			#ifdef RANDACCESS
				randAccess(fd,buff_read,buff_write);
			#endif
		#endif

	}	
	
	
	return 0;
}
