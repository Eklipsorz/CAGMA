#include <stdio.h>
#include <stdlib.h>

void funct()
{
	printf("hi\n");

}

int main()
{
	int i;

	for(i=0;i<10;i++)
	{
		printf("%d\n",i);
		funct();
	
	}

	return 0;
}
