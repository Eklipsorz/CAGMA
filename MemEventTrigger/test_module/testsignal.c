#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>


void sighup();
void sendpid()
{ 
	int procfs;
	char msg[20];
 	
	while((procfs = open("/proc/pidbuffer", O_RDWR))<0){
		printf("/proc/pidbuffer isn't exist\n");    
       		sleep(1);
   	} 
     	sprintf(msg,"%d",getpid());    
   	write(procfs, msg, strlen(msg) + 1);
   	close(procfs);
   
}

int main(){

	signal(SIGHUP,sighup);
	sendpid();
	
	while(1)
 	{
		printf("PID=%d\n", getpid());
   		sleep(1);
	}

}

void sighup(){ 
	signal(SIGHUP,sighup); 
	printf("Signal SIGHUP received!\n");

}






