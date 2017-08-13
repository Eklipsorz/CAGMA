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
	char memUinStr[2][10]={{"56"},{"58"}};
	char pgfinStr[2][10]={{"5"},{"10"}};
	char output[100]="hi_i'm_steck",int2str[10];
	int i=0,len,id,target,outlen,sumlen=0;	
	
	configfd = open("/proc/buffer", O_RDWR);	
	
	for(;i<2;i++)
	{
		len=strlen(memUinStr[i])+strlen(pgfinStr[i])+3;    
		if(i==0)
			sprintf(output,"%s:%sx%d",memUinStr[i],pgfinStr[i],len);
		else
			sprintf(output,"%sx%s:%sx%d",output,memUinStr[i],pgfinStr[i],len);


	}
		
	write(configfd, output, strlen(output) + 1);
	close(configfd);
	return 0;
}
