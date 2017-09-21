#include "FixedAccMem.h"

/* This program helps user modify the suffex of a file storing each data from
 * each memory-bound task. You first run this program with a parameter which 
 * can be the suffex of the file 
 */

/* This function modify the suffex via writing /proc/fsuffexSetter */
void SuffexSetter(char *input,int len)
{
	
	int configfd;

	/* require for accessing /proc/fsuffexSetter */
	configfd = open("/proc/fsuffexSetter", O_RDWR);	
	
	if(configfd < 0) {
		perror("open");
		return;
	}

	/* write new suffex into /proc/fsuffexSetter */
	write(configfd,input,len);
	close(configfd);

}

int main(int argc,char *argv[])
{
	char *fileNum = NULL;
	int len = 0;


	/* check whether the number of the inputted arguments is 2 */
	if (argc < 2)
	{
		printf("u cannot specify fileNum\n");
		return 0;
	}

	/* allocate memory to a string */
	/* then the Guest OS can give the argument into the string */
	len = strlen(argv[1]) + 1;
	fileNum = (char *) calloc(sizeof(char),len);	
	strcpy(fileNum,argv[1]);

	/* modify the suffex of the file storing each data from memory-bound task */
	SuffexSetter(fileNum,len);

	return 0;
}
