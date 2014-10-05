/*Author:George Karaszi
 * Date:September 26 2014
 * 
 * About:A recreation of the popular malloc, realloc, calloc and free 
 * 	functions based on the usage of sbrk and heap management.
 * 
 * Functions:
 * 
 * NewMallocLocation(..):Creates a new structure that acts as the allocated data's HEADER. 
 * 	This structure is created on a specific address.
 * 
 * HeapInsert(..):HeapInsert is sort for inserting new structures of allocated 
 * 	data in the root of MALLOC_HEAD structure. This function assumes that the 
 * 	target has enough room for all information it requires. It will also chunk 
 * 	the heap structure if it finds that the allocated memory will allow for 
 * 	further free locations based on the structure size requirements. If there 
 * 	is not enough space for another free chunk, it will give the user extra 
 * 	space as a result.
 * 
 * GetFreeSpaceBySize(..): Locates blocks of memory that have enough free 
 * 	space to satisfy the users demand.
 * 
 * GetMatchingDataPtr(..): Matches the data ptr address to that which is stored 
 * 	in the MALLOC_HEAD structure. Thus allowing to pinpoint which structure is 
 * 	being targeted.
 * 
 * CopyHeapData(..): Simply copies one structures pdata field to another.
 * 
 * CleanAllocatedSource(..):Labels the block as free and puts the pdata to NULL. 
 * 	We do not have to worry about the actual data, as that will be overwritten 
 * 	when ever a malloc is called again and overrides that information.
 * 
 * CleanAllocatedList(..): Passes through the link list connecting free 
 * 	data blocks that are next to each other, in order to maintain good address 
 * 	and allocation flow.
 * 
 * ReportProcess(..): Reporting tools to show whats going on with this heap 
 * 	process.
*/
#include <stdio.h>
#include "malloc_handler.h"


//-----------------------------------------------------------------------------
//Start a new malloc structure with its size and state and address location

malloc_head *NewMallocLocation(void* address, int size)
{
	malloc_head *newmalloc = (malloc_head *)address;

	newmalloc->free = TRUE;
	newmalloc->size = size - MHSIZE;
	newmalloc->next = NULL;
	newmalloc->pdata = NULL;

	return newmalloc;
}

//-----------------------------------------------------------------------------
//Insert new structure into chunk of memory

void HeapInsert(malloc_head *target, int requestSize)
{
	//Address to the new chunk of memory if we need to split the heap more
	void *chunkAddress  = (unsigned long)target + requestSize + MHSIZE;

	//Left over bytes from when we split the heap more into chunks based on 
	//user size request. 
	int bytesLeftOver   = target->size - requestSize;

	//Temp variable used to handle *next* pointer's in memory
	malloc_head *temp = NULL;


	target->free = FALSE;   //target is no longer free

	if(target->next != NULL)
	{
		//If there is enough bytes left to split the heap more, do so
		if(bytesLeftOver > MHSIZE)
		{
			//Create a new structure in the left bytes that are not being used
			temp = NewMallocLocation(chunkAddress, bytesLeftOver);
			temp->next = target->next; //Maintain next pointer addresses
			target->next = temp;		

			//Change the data size of the target since we split the heap more
			target->size = requestSize;
		}
	}
	else
	{
		if(bytesLeftOver > MHSIZE)
		{
			target->next = NewMallocLocation(chunkAddress, bytesLeftOver);

			//Change the data size of the target since we split the heap more
			target->size = requestSize;
		}
	}

}

//-----------------------------------------------------------------------------
//Get the next structure in the list that has enough space for incoming data

malloc_head *GetFreeSpaceBySize(malloc_head *mlist, int size)
{
	
	do
	{
		if(mlist->size >= size && mlist->free == TRUE)
		{
			return mlist;
		}

	}while((mlist = mlist->next) != NULL);

	return NULL;
}

//------------------------------------------------------------------------------
//Returns the end of the Link List structure

malloc_head *GetEndOfList(malloc_head *mlist)
{
	while(mlist->next != NULL)
	{
		mlist = mlist->next;
	}
		
	return mlist;
}

//-----------------------------------------------------------------------------
//Find a matching address in the malloc head structure

malloc_head *GetMatchingDataPtr(malloc_head *mList, void *pdata)
{
	do{
		if(&mList->pdata == pdata)
			return mList;
	}while((mList = mList->next) != NULL);
	
	return mList;
}

//------------------------------------------------------------------------------
//Copy one data set to another

void CopyHeapData(malloc_head *Src, malloc_head *Dst)
{
	memcpy(&Dst->pdata, &Src->pdata, Src->size);
}

//------------------------------------------------------------------------------
//Change the lable that tells if the block is free

void CleanAllocatedSource(malloc_head *target)
{
	target->free = TRUE;
	target->pdata = NULL;
}

//------------------------------------------------------------------------------
//Some times the heap list is going to have multiple free spots next to each other.
//We need to merg them each time an object is freed. Since this is called each 
//time an object is freed, we only need to traverse the list one time.

void CleanAllocatedList(malloc_head *mList)
{
	malloc_head *tmp = NULL;
	
	do{
		
		if(mList->free == TRUE)
		{
			tmp = mList->next;
			if(tmp != NULL && tmp->free == TRUE)
			{
				//Combine the sizes plus its structure to gain the entire block size.
				mList->size += tmp->size + MHSIZE;
				//Point to the next location since tmp is not longer a block itself.
				mList->next = tmp->next;
			}
		}
	}while((mList = mList->next) != NULL);
}


//------------------------------------------------------------------------------
//Reports the free and occupied space from the list

void ReportProcess(malloc_head *mList)
{
	int totalfree             = 0;
	int totalspace_free       = 0;
	int totaloccupied         = 0;
	int totalspace_occupied   = 0;
	
	do
	{
		if(mList->free == TRUE)
		{
			++totalfree;
			totalspace_free += mList->size;
		}
		else
		{
			++totaloccupied;
			totalspace_occupied += mList->size;
		}
		
		printf("Address:%X\tFree Status:%s\tSize:%d\n",
		mList, (mList->free == TRUE ? "TRUE" : "FALSE"), mList->size);
		
	}while((mList = mList->next) != NULL);
	
	printf("\nTotal free blocks:%d\t   Total Free Space:%d\n", 
		totalfree, totalspace_free);
	printf("Total Occupied blocks:\%d\tTotal Occupied space:%d\n\n", 
		totaloccupied, totalspace_occupied);
		
		
}
