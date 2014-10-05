

#ifndef __MALLOCHANDLER__
#define __MALLOCHANDLER__

#define TRUE  1
#define FALSE 0


typedef struct _malloc_head_t
{
	int free;
	int size;
	struct _malloc_head_t *next;
	void *pdata;
	
}malloc_head;

#define MHSIZE sizeof(malloc_head)

//New allocations
malloc_head *NewMallocLocation(void* address, int size);
void HeapInsert(malloc_head *location, int requestSize);

//Modifications
void CopyHeapData(malloc_head *Src, malloc_head *Dst);

//Information
malloc_head *GetMatchingDataPtr(malloc_head *mList, void *pdata);
malloc_head *GetFreeSpaceBySize(malloc_head *mlist, int size);
malloc_head *GetEndOfList(malloc_head *mlist);
void ReportProcess(malloc_head *mList);

//Clean up
void CleanAllocatedSource(malloc_head *target);
void CleanAllocatedList(malloc_head *mList);



#endif