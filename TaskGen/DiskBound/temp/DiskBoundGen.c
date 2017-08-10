
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>

#define sectorSize 2097152
#define taskNum 10
#define target 100
#define w_len 512
#define r_len 512
#define execIter 5
#define dalayed_target 150
void do_as_children();
static struct timeval runth_s,runth_e;
extern void Timediff(struct timeval *src,struct timeval *des,int type);
void sig_fork(int signu)
{
	switch(fork())
	{
		case -1:
			printf("child Generation Failed\n");
			exit(0);
		break;
		case 0:
			do_as_children();
		break;
	}

}
void sig_alarm(int a)
{

	exit(0);

}

void sighup()
{
	int configfd,timeTemp;
	double rtime;
	char output[30];
	time_t rawtime;
	struct tm * timeinfo;
	
	configfd = open("/proc/buffer", O_RDWR);	
	
	if(configfd < 0) {
		perror("open");
		return;
	}
	
	gettimeofday(&runth_e,NULL);
	Timediff(&runth_s,&runth_e,2);
	rtime = (double)runth_e.tv_sec+(double)runth_e.tv_usec/1000000;
	
	

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	
	timeTemp = timeinfo->tm_hour*3600+timeinfo->tm_min*60+timeinfo->tm_sec;
	sprintf(output,"%d %.4lf\n",timeTemp,rtime);	
	printf("%d\t%f\n",timeinfo->tm_hour*3600+timeinfo->tm_min*60+timeinfo->tm_sec,rtime);
	//
	write(configfd, output, strlen(output) + 1);
	close(configfd);
	
	if (rtime > 720)
		return;
}

void do_as_children()
{
	int j,rc,fd;	
	char *buff_read, *buff_write;
	
//	signal(SIGALRM,sig_alarm);
//	alarm(dalayed_target);
	
	
	signal(SIGHUP,sighup);

	buff_read = (char *) mmap(NULL, sectorSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	buff_write = (char *) mmap(NULL, sectorSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	
	for(j = 0; j < sectorSize; j++)	
		buff_write[j] = '1' + j % 26;

//access:
	if( (fd = open("../../myrandom",O_RDWR | O_DIRECT | O_DSYNC)) == -1 )
	{
		//O_DIRECT
		printf("The file isn't exist\n");
		return;
	}

	gettimeofday(&runth_s,NULL);
	for(j = 0; j < execIter; j++)
	{
		lseek(fd,j*sectorSize,SEEK_SET);
		if(j%2)
		{
			if ( (rc = read(fd,buff_read,sectorSize) == -1 ))
				printf("read failed\n");
		}
		else
		{
			if ( (rc = write(fd,(void*)buff_write,sectorSize) == -1 ))
				printf("write failed\n");
		}
	}

	close(fd);
}
int main()
{
	int i;
	long time;
	struct timeval begin,end;
	struct sigaction sigchld_action = {
		.sa_handler =  sig_fork,
		.sa_flags = SA_NOCLDWAIT
	};

	sigaction(SIGCHLD, &sigchld_action, NULL);

	
	for (i = 0; i < taskNum; i++)
	{
		switch(fork())
		{
			case -1:
				printf("child Generation Failed\n");
				return 0;
			break;
			case 0:
				do_as_children();
				exit(0);
			break;

		}
	}
	time = 0;
	gettimeofday(&begin,NULL);

	while(time < target)
	{
		gettimeofday(&end,NULL);
		Timediff(&begin,&end,2);
		time = end.tv_sec;
	}
	


	return 0; // End for parent


	
	

}
