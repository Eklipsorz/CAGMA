
#define _GNU_SOURCE

#include "./Spec.h"
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
//#define sectorSize 4194304
#define sectorSize 2097152
//#define sectorSize 3145728
#define maxTime 2048000
//#define w_len 2097152
//#define r_len 2097152
//#define execIter 600 //origin 41250
//#define execIter 1000
//#define execIter 12
#define execIter 3
// 3 = 0.3
// 12 = 0.9
//350 = 900
//150 = 450
static struct timeval runth_s,runth_e;


static void timer_handler()
{
	int resultfd;
	double rtime;
	char output[30];
	
	resultfd = open("/proc/buffer", O_RDWR);    

	gettimeofday(&runth_e,NULL);
	Timediff(&runth_s,&runth_e,2);
	rtime=(double)runth_e.tv_sec+(double)runth_e.tv_usec/1000000;
	
	if (rtime > 720)
		return;
	
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


int main()
{
	int i,fd;
	struct itimerval timer;
	double rtime = rtime;
	char *buff_read, *buff_write;
	struct sigaction sa;
	//open family 
	//read: read data from file
	// 		int  read(  int  handle,  void  *buffer,  int  nbyte );
	//write: write data into file
	// 		int  write(  int  handle,  void  *buffer,  int  nbyte  );
	//fopen family
	//fgets: read data from file
	//fprintf: write data into file

	buff_read = (char *) mmap(NULL, sectorSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	buff_write = (char *) mmap(NULL, sectorSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	for(i=0;i<sectorSize;i++)	
		buff_write[i] = '1' + i % 26;

	if( (fd = open("../../myrandom",O_RDWR | O_DIRECT | O_SYNC | O_RSYNC)) == -1 )
	{
		//O_DIRECT
		printf("The file isn't exist\n");
		return 0;
	}


	/* It can sequentially read from disk and write data into disk
	 * the execution time:
	 */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &timer_handler;
	sigaction(SIGALRM,&sa,NULL);
	
	timer.it_value.tv_sec = 3;
	timer.it_value.tv_usec = 0;

	timer.it_interval.tv_sec = 3;
	timer.it_interval.tv_usec = 0;
	
	setitimer(ITIMER_REAL,&timer,NULL);
	while(1)
	{	

		gettimeofday(&runth_s,NULL);

		#ifdef SEQACCESS	
		seqAccess(fd,buff_read,buff_write);
		#else
			#ifdef RANDACCESS
				randAccess(fd,buff_read,buff_write);
			#endif
		#endif
	

	}	
	
	close(fd);	
	
	return 0;
}
