#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


void begin(char *input,int len)
{
	
	int configfd;
	
	configfd = open("/proc/enabler", O_RDWR);	
	
	if(configfd < 0) {
		perror("open");
		return;
	}

	write(configfd,input,len);
	close(configfd);

}

int main(int argc,char *argv[])
{
	char *fileNum = NULL;
	int len = 0;

	if (argc < 2)
	{
		printf("u cannot specify fileNum\n");
		return 0;
	}
	
	len = strlen(argv[1]) + 1;
	fileNum = (char *) calloc(sizeof(char),len);	
	strcpy(fileNum,argv[1]);
	
	begin(fileNum,len);

	return 0;
}
