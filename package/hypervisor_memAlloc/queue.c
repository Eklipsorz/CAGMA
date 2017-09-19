#include "memAlloc.h"

entry *Alloc = NULL;
entry *Relea = NULL;
// variable type: 
// 0 = Alloc
// 1 = Relea

/* 
 * Find the entry from the queue with entry id
 * note that entry *from is the queue you wants to find.
 */
entry * FindEntry(int id,entry *from)
{
	entry *temp = NULL;
	
	for (temp = from->next ; temp != from; temp = temp->next)
		if (temp->id == id)
			return temp;
	
	return NULL;	
}

/* Remove the selected entry in the queue */
void RemoveEntry(entry *e)
{
	entry *temp = e;
	(e->prev)->next = e->next;
	(e->next)->prev = e->prev;
	
	free(temp);
}

/* Update the info of the selected entry */
void UpdateEntry(entry *e,int64_t ALM,int64_t AVM,int64_t CMA)
{
	e->ALM = ALM;
	e->AVM = AVM;
	e->CMA = CMA;
}

/*
 * add the proper queue according to whether the available memory of each vm, 
 * which is waiting to be queued, meets its critical memory amount.
 */
void addEntry(int id,char *path,int64_t ALM,int64_t AVM,int64_t CMA)
{	
	entry *temp = NULL, *head = NULL,*found = NULL;

	/* 
 	 * determine which queue is proper for this vm according to (CMA > AVM) 
 	 * if CMA > AVM, then the variable head is Alloc
 	 * if CMA < AVM, then the variable head is Relea
 	 */	
	head = (AVM < (int64_t)((float)(Alloc_rate*CMA))) ? Alloc : Relea;

	/* Update old info of the existing entry or add new entry into proper queue */	
	if ((found = FindEntry(id,head)))	/* Update old info of the existing entry */
		UpdateEntry(found,ALM,AVM,CMA);	
	else					/* add new entry into proper queue */
	{
	
		/* 
 		 * if a VM has been put into another queue previously, then the info of this 
 		 * vm in the queue is removed for updating the status of each vm
 		 * 
 		 * For example, suppose that the info of a vm has been put into Alloc. However,
 		 * the VM is classified into the queue Relea now. For updating the status, the
 		 * old info in the queue Alloc need to be removed and the new info is put into 
 		 * the queue Relea.
 		 */
		if (head == Alloc)				
		{
			/* check whether the old info is in the queue Relea */
			if ((found = FindEntry(id,Relea)))
				RemoveEntry(found);			
		}
		else if (head == Relea)
		{
			/* check whether the old info is in the queue Alloc */
			if ((found = FindEntry(id,Alloc)))
				RemoveEntry(found);			
		}

		/* build a new entry */
		temp = (entry *)calloc(1,sizeof(entry));	
		temp->id = id;
		temp->ALM = ALM;
		temp->AVM = AVM;
		temp->CMA = CMA;
		temp->releaCount = 0;
		temp->next = temp->prev = temp;
		sprintf(temp->path,"%s",path);	

		/* the new entry is put into the queue */
		(head->prev)->next = temp;
		temp->prev = head->prev;
		head->prev = temp;
		temp->next = head;
	}
}	

/* list all entries from the two queues (Alloc and Relea) respectively. */
void listEntry()
{	
	entry *temp =NULL;
	
	/* list all entries from the queue Alloc */	
	printf("Alloc Set:\n");
	for (temp = Alloc->next ; temp != Alloc; temp = temp->next)
		printf("%d: ALM: %"PRId64" AVM: %"PRId64" CMA: %"PRId64"\n",\
					temp->id,temp->ALM,temp->AVM,temp->CMA);
	/* list all entries from the queue Relea */
	printf("Relea Set:\n");
	for (temp = Relea->next ; temp != Relea; temp = temp->next)
		printf("%d: ALM: %"PRId64" AVM: %"PRId64" CMA: %"PRId64"\n",\
					temp->id,temp->ALM,temp->AVM,temp->CMA);
					
}

/* update memory/CMA with newCMA */
void updateCMA(char *path,int64_t newCMA)
{
	struct xs_handle *xs;
	xs_transaction_t trans;
	char CMApath[50],CMA[10];

	/* obtain the path of memory/CMA */
	sprintf(CMApath,"%s/CMA",path);	
	sprintf(CMA,"%"PRId64,newCMA);	


	/* update memory/CMA with newCMA via the interface xenstore */
	xs = xs_daemon_open();
	
	trans = xs_transaction_start(xs);
	
	xs_write(xs,trans,CMApath,CMA,strlen(CMA));
	xs_transaction_end(xs,trans,false);	

	xs_daemon_close(xs);


}


/* allocate more memory to each vm, which meets the condition (CMA > AVM) */
void allocate(void)
{

	xs_transaction_t trans;
	struct xs_handle *xs;
	entry *temp = NULL;
	int64_t AllfreeMem,amount,Target;
	char ALMpath[50],ALM[50];
	xs = xs_daemon_open();
	
	/* travel the list to allocate memory respectively */
	for (temp = Alloc->next ; temp != Alloc; temp = temp->next)
	{
		/* obtain the value of the physical memory */ 
		cur_node = xenstat_get_node(xhandle, XENSTAT_ALL);
		AllfreeMem = (int64_t) (xenstat_node_free_mem(cur_node)/1024);
		
		if (AllfreeMem <= 0)	/* if no any physical memory, the hypervisor does not allocate */
			return;
		else			/* if there are sufficient memory, the hypervisor allocate memory */
		{
			
			/* 
 			 * calculate new allocated memory amount according difference between
 			 * CMA and AVM. However, if the new amount is bigger than maximum amount,
 			 * the new amount is set to maximum amount.
 			 */
			amount = (int64_t)(Alloc_rate*(float)temp->CMA) - temp->AVM;
			Target = temp->ALM + amount;	
			
			if (Target > AllfreeMem)
				Target = AllfreeMem;
			else if (Target > Mem_maximum)
				Target = Mem_maximum;
			/* obtain the path of memory/target in xenstore */	
			sprintf(ALMpath,"%s/target",temp->path);
			sprintf(ALM,"%"PRId64,Target);	
			/* display the expected the status of the allocation */	
			printf("	allocate: %s %"PRId64" , TAM: %"PRId64" \n",temp->path,Target,AllfreeMem);		
			/* adjust allocated memory amount to the new amount */
			trans = xs_transaction_start(xs);
			xs_write(xs,trans,ALMpath,ALM,strlen(ALM));
			xs_transaction_end(xs,trans,false);	
			
		}
	}	

	xs_daemon_close(xs);		
}

/* try to release more memory if a VM is sequentially released and the times meets threshold_relea */	
int64_t Tryto_release_more(entry *e)
{
	int64_t temp,mean = 0;
	int i;
	
//	printf("entering tryto mode\n");	
	for (i = 0 ; i < NRsample ; i++)
	{
		mean = mean + e->sample[i];
		//printf("sample: %"PRId64 " \n",e->sample[i]);
	}
	mean = mean / NRsample;	

	//printf("%"PRId64 " \n",mean);
	if (e->AVM == 0)
		return 0;
	else if (mean > e->AVM)
	{
	//	printf("case 1\n");
		temp = e->AVM ;
		e->CMA = 0;
		updateCMA(e->path,0);	
	}
	else if ( mean > (e->AVM - e->CMA))
	{
	//	printf("case 2\n");
		temp = mean;
		e->CMA = e->AVM - mean;
		updateCMA(e->path,e->CMA);
	}
	else
	{
	//	printf("case 3\n");
		temp = (e->AVM - (int64_t)(Alloc_rate*(float)e->CMA));
	}
	//printf("CMA %"PRId64 " amount %"PRId64 " AVM %"PRId64 " %mean %"PRId64 "\n",e->CMA,temp,e->AVM,mean);
	
	return temp;
}

/* release the memory of the selected vm according to the condition (CMA < AVM) */
void release(void)
{
	
	xs_transaction_t trans;
	int64_t Target,amount;
	struct xs_handle *xs;
	char ALMpath[50],ALM[10];
	entry *temp;
	
	xs = xs_daemon_open();
	
	for (temp = Relea->next ; temp != Relea; temp = temp->next)
	{
	
		temp->releaCount = temp->releaCount + 1;
		
		if (temp->releaCount >= threshold_relea)
			amount = Tryto_release_more(temp);
		else
			amount = temp->AVM - (int64_t)( Alloc_rate*(float)temp->CMA );
		/*printf("before: %"PRId64" after: %"PRId64" AVM%"PRId64" amount:%"PRId64" \n",temp->CMA,\
						(int64_t)(Alloc_rate*(float)temp->CMA), temp->AVM, amount);
		*/
	
		Target = temp->ALM - amount;
		if (Target < Mem_minimum)
			Target = Mem_minimum;
			
				
		sprintf(ALMpath,"%s/target",temp->path);
		sprintf(ALM,"%"PRId64,Target);		
	//	printf("	release: %s %"PRId64" \n",temp->path,Target);		
		//printf("%s %s %"PRId64 " count %d\n",ALMpath,ALM,Target,temp->releaCount);
		//printf("ans: %lld %"PRId64"\n",(long long int)Target,Target);
	
		trans = xs_transaction_start(xs);
		xs_write(xs,trans,ALMpath,ALM,strlen(ALM));
		xs_transaction_end(xs,trans,false);	
		temp->sample[temp->releaCount % NRsample] = temp->AVM - amount;
		//printf("sample AVM: %"PRId64 " CMA: %"PRId64 " \n",temp->AVM,temp->CMA);
	}	
	xs_daemon_close(xs);		
	
}
