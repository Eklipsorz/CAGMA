#include "Spec.h"

entry *head=NULL;
entry *tail=NULL;
entry *Datahead=NULL;

int nr_item=0;
int version=0;
int zero_case=0;
int nonzero_case=0; 
int critimemU=0;
int isExist(double MemU)
{

	entry *temp=head;
	for(;temp!=NULL;temp=temp->next)
		if(temp->MemU==MemU)
			return 1;
	return 0;
}


void listEntry()
{
	entry *temp=head;
	for(;temp!=NULL;temp=temp->next)
		printf("rtime:%f \n",temp->rtime);
}

int check_MultiData(double MemU,double majpgf)
{
	entry *temp=head;
	for(;temp!=NULL;temp=temp->next)
	{
		if(temp->MemU==MemU)
		{
			temp->nr_majpgf+=majpgf;
			temp->Num++;
			return 1;
		}
	}	
	return 0;

}

void Queu2Aray(double *a,double *b)
{

	if(head==NULL)
		return;
	int i;
	entry *temp=head;
	for(i=0;temp!=NULL;temp=temp->next,i++)
	{
		a[i]=temp->MemU;
		b[i]=(temp->nr_majpgf/temp->Num);	
	}
}

int num_Data_For_majPGf()
{
	entry *temp=head;
	for(;;temp=temp->next)
	{
		if(temp==NULL)	
			break;
		else if(temp->nr_majpgf>0)
			nonzero_case++;
		else
			zero_case++;
	}

		
	return ((nonzero_case>=zero_case)?nonzero_case:0);

}

void addEntry(double rtime)
{

	
	entry *temp=NULL,*moving=NULL;
	temp=(entry *)calloc(1,sizeof(entry));
	temp->next=NULL;
	temp->rtime=rtime;
	nr_item++; /* increase number of the data when it run "add" */
	
	if(head==NULL)
		head=temp;
	else
	{
		moving=head;
		
		for(;moving->next!=NULL;moving=moving->next);

		moving->next=temp;
	}
	
	return;	
}	
//
int getEntry(entry *result)
{
	if(head!=NULL){
		entry *firstEntry=head;
		result->nr_majpgf=head->nr_majpgf;
		result->MemU=head->MemU;
		head=head->next;
		free(firstEntry);
		return 0;		
	}
	else
		return -1;
}


void sendData()
{

	char output[1000],memU[5],majpgf[10];	
	int len,configfd;	
	entry *moving=head;	
	double meanPgf;
	printf("nonzero %d zero %d\n",nonzero_case,zero_case);
	for(;moving!=NULL;moving=moving->next)
	{
		meanPgf=(moving->nr_majpgf)/moving->Num;
		sprintf(memU,"%.0f",moving->MemU*100);
		sprintf(majpgf,"%.0f",meanPgf);
	
		len=strlen(memU)+strlen(majpgf)+3;
		if(moving==head)
			sprintf(output,"%s:%s %d",memU,majpgf,len);
		else
			sprintf(output,"%s %s:%s %d",output,memU,majpgf,len);
				
	}

	
	configfd = open("/proc/buffer", O_RDWR);	
	write(configfd, output, strlen(output) + 1);
	close(configfd);
}

void init_pool(pool *pol)
{
	int i;
	pooldata *poldata=pol->data;
	
	for(i=0;i<101;i++)
	{
		poldata[i].nr_majpgf=0;
		poldata[i].nr_memU=0;
	}	
	

}

int find_critimemU_from_pool(pool *pol)
{
	int i;
	pooldata *poldata=pol->data;	
	
	for(i=0;i<101;i++)
	{
		if(poldata[i].nr_majpgf>0)
			return i;
	}
	
	return -1;
}

int check_numof_nonzero_and_zero(pool *pol,int memU)
{
	int i,nonzero=0,zero=0;
	pooldata *poldata=pol->data;	
	
	for(i=0;i<101;i++)
	{
		
		if(poldata[i].nr_majpgf>0)
			nonzero++;
		else
			zero++;
		
		printf("%f\n",poldata[i].nr_majpgf);
	}
	printf("inner %d %d\n",nonzero,zero);	
	return ((nonzero>zero)?1:0);
}

void add_item_into_pool(double pgf,int MemU,pool*pol)
{
	pooldata *poldata=pol->data;	
	
	poldata[MemU].nr_majpgf+=pgf;
	poldata[MemU].nr_memU++;
		
}
