#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#define PAGE_SIZE 4096

int main()
{
	int configfd;
	char * address=NULL;
	char output[100],input1[10]="20",input2[10]="200";
	int i=0,len,id,target,outlen,sumlen=0;	
	
	configfd = open("/proc/buffer", O_RDWR);	
	
	sprintf(output,"%s %s",input1,input2);
	printf("%s\n",output);

		
	write(configfd, output, strlen(output) + 1);
	close(configfd);
	return 0;
}
