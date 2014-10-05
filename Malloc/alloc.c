/*Author:George Karaszi
 * Date:September 26 2014
 * 
 * About: A recreation of the popular malloc, realloc, calloc and free 
 * 	functions based on the useage of sbrk and heap management.
 * 
 * Functions:
 * nu_malloc(..):Starts off making sure that there is significant memory 
 * 	allocated to the heap. Once allocation has be performed by sbrk it 
 * 	proceeds to create a new structure on the given address provided by sbrk().
 * 
 * nu_calloc(..):Calls nu_malloc then proceeds to zero out the memory location 
 * 	of the given data by its given size requested by the user.
 * 
 * nu_realloc(..):Takes the original structure and expands it based on the user 
 * 	input. If the current structure can not hold the new data, a new structure 
 * 	is created on the heap then data from the old is copied into the new only 
 * 	and a new pointer address is assigned to the new location. 
 * 	Old location is then freed.
 * 
 * nu_free(..):Free the structure that the user wants free. This also performs
 * 	heap management by connecting free blocks that are next to each other.
 * 
 */


#include <unistd.h>
#include <stdio.h>
#include "malloc_handler.h"
#include "malloc_handler.c"


#define MALLOCSIZE 2048


static malloc_head *_malloc_head; //Root to our heap list

//------------------------------------------------------------------------------
//Allocate memory based on its requested size

void* malloc(size_t size)
{
	malloc_head *temp     = NULL; //Handles inserting into the list
	malloc_head *mListend = NULL; //Handles the end of the list for linking temp
	
	if(_malloc_head == NULL)
	{
		_malloc_head = NewMallocLocation(sbrk(MALLOCSIZE), MALLOCSIZE);
	}
	
	temp = GetFreeSpaceBySize(_malloc_head, size);

	//Not enough space left of the heap to allocate to, request more
	if(temp == NULL)
	{
		temp = NewMallocLocation(sbrk(size+MHSIZE), size+MHSIZE);
		mListend = GetEndOfList(_malloc_head);
		mListend->next = temp; //Link the structures together
		
	}

	HeapInsert(temp, size);
	
	//ReportProcess(_malloc_head);
	
	return &temp->pdata;
}

//------------------------------------------------------------------------------
//Allocate and fill the data block with the given size and count

void* calloc(size_t count, size_t size)
{
	void *ptr = malloc(size*count);
	
	if(ptr != NULL)
		memset(ptr, 0, size*count);
	
	return ptr;
}

//------------------------------------------------------------------------------
//Reallocate new memory size to the given pointer

void* realloc(void* ptr, size_t size)
{
	malloc_head *datatmp = NULL;
	malloc_head *newSize = NULL;
	void *data_tmp       = NULL;
	
	if(ptr != NULL)
	{
		//Find the structure that the data is located in
		datatmp = GetMatchingDataPtr(_malloc_head, ptr);
		if(datatmp != NULL)
		{
			
			if(datatmp->size < size) //Expand the size of the data structure
			{
				//Find new memory to house the expanded block
				newSize = GetFreeSpaceBySize(_malloc_head, size);
				if(newSize != NULL)           //Check for enough free space
				{
					HeapInsert(newSize, size); //Insert section into heap
				}
				else                         //Alloc more free space if needed
				{
					data_tmp = malloc(size);
					newSize = GetMatchingDataPtr(_malloc_head, data_tmp);
				}
				
				CopyHeapData(datatmp, newSize);    //Copy data from old to new
				free(&datatmp->pdata);          //Free old data
			}
			else if(datatmp->size > size)//Resize structure if request is smaller
			{
				HeapInsert(datatmp, size);
				newSize = datatmp;
			}
			else                     //Do nothing if request is the same size
			{
				newSize = datatmp;
			}
		}
	}
	//ReportProcess(_malloc_head);
	
	return &newSize->pdata;
}

//------------------------------------------------------------------------------
//Free sources that have been allocated

void free(void* ptr)
{
	if(ptr != NULL)
	{
		CleanAllocatedSource(GetMatchingDataPtr(_malloc_head, ptr));
		CleanAllocatedList(_malloc_head);
	}
	
	//ReportProcess(_malloc_head);
}

//------------------------------------------------------------------------------
//Report memory information

void MemoryReport()
{
	ReportProcess(_malloc_head);
}
