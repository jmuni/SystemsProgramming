#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"
#define RAM 4096

memEntry *current;
memEntry *target;


static char memory[RAM];

struct memEntry *head=(void*)memory;
//moved from mymalloc.h

void * myMalloc(size_t size, const char *file, int line)
{
	if(head->totalSpace == 0)	//CW: if the head has no values associated with it, this code will give it the proper values to continue
	{
	//	printf("Making First Metadata\n");
		head->totalSpace = RAM-(sizeof(memEntry));
		head->isFree = 1;
		head->next = NULL;
	}
	void* address = NULL;
	current = head;
	while(current != NULL)	//loops until the end of the list
	{
		//printf("Making Allocation\n");
	//	printf("Total Space: %i\n", current->totalSpace);
	//	printf("Size: %i\n", (int)size); //casted size int
	//	printf("Is Free: %i\n", current->isFree);
		if(size == 0) //CW: No mallocing 0 bytes, okay?
		{
			printf("Cannot malloc data of size 0, please try again\n");
			return NULL;
		}
		if((current->totalSpace < size || current->isFree == 0) && current->next != NULL)//CW: if the block is too small, or allocated, OR if its the end of the list
		{
			current = current->next;
			//printf("Allocated or too small, moving to next\n");
		}
		else if(current->totalSpace >= size && current-> isFree == 1)//CW: if the block is the right size and is free
		{	//be ready to return the adress in the array
			if((current->totalSpace - size) > sizeof(memEntry))	//CW: if there is enough space for another metadata to be created with a size of at least 1
			{
				//printf("Making new Chunk\n");
				memEntry *newEntry = (void*)((void*)current+size+sizeof(memEntry));
//				printf("current total: %i \n size: %i \n MemEntrySize: %i \n", current->totalSpace, size, sizeof(memEntry)); 
//compiler format error, some units are unsigned longs and stuff
//trying to split it up see whats going wrong			CW: This was just debug code to see if everything was working
				//printf("Space available: %i\n", current->totalSpace);
				//printf("Size requested: %i\n", (int)size);
//size is of type "size_t" this printf is giving errors. Changed to %zu
				//printf("Metadata size: %i\n", (int)sizeof(memEntry));
//this type is in long unsigned int
			
				
				newEntry->totalSpace = current->totalSpace - size - sizeof(memEntry);
				newEntry->isFree = 1;
				newEntry->next = current->next;
				current->totalSpace = size;
				current-> isFree = 0;
			//	printf("Chunk Allocated, new chunk created at tail with size: %i\n", newEntry->totalSpace);
				current->next = newEntry;
				address = (void*)(++current);
				return address;
				/**make new metaData with size of (totalSpace - size - sizeof(memEntry)) 
				that starts at current + size + sizeof(metadata)*/  
			}
			else
			{
			//	printf("Chunk allocated, no more space in tail for additional chunks\n");
				current->isFree = 0;
				address = (void*)(++current);
				return address;
			}
			
		}
		else
		{
			printf("No space for allocation at line %d in %s, please free up some memory\n", __LINE__, __FILE__);
			return NULL;
		}
	}
	return NULL;
//getting control reachs end of non-void function error
}
void myFree(void* ptr, const char *file, int line)
{
	int isPointer = 0;
	target = ptr;//CW: the address we're looking to free
	--target; //CW: it was getting lost, this seemed to fix it
	current = head;
	if(target != NULL)
	{
		while(current->next != NULL)
		{
			//printf("Checking for pointer\n");
			if(target == current)
			{
				//printf("pointer found\n");
				isPointer = 1;
				break;
			}
			current = current->next;
		}
		if(current->next == NULL && target == current)//if the entry is the last in the linked list (the size requested accompanied the remaining space in memory and could not fit a tail
		{
			isPointer = 1;
		}
		if(isPointer == 0)
		{
			printf("Pointer freed on line %d in file %s not found in memory\n", __LINE__, __FILE__);
			return;
		}
		if(target-> isFree == 0)
		{
			target->isFree = 1;
		//	printf("Chunk Freed\n");
			current = head;
			while(current->next!= NULL) //CW: searches all the chunks to find one who shares an isFree value of 1 with the next chunk it is pointing to, then simply expands its allowed size and points itself to its next chunk's next. By only searching forwards, the head chunk will never be lost

			{
 				if(current->isFree == 1 && current->next->isFree == 1)
				{
			//		printf("Combining consecutive free chunks\n");
					current->totalSpace += (current->next->totalSpace + sizeof(memEntry));
					current-> next = current->next->next;
			//		printf("New Chunk Size: %i\n", current->totalSpace);
				}
				if(current->next == NULL)
				{
					break;
				}
				current = current->next;

			}

		}
		else
		{
			printf("Already Freed, please try again\n");
		}
		
		if(head-> isFree == 1 && head->next != NULL && head->next->isFree == 1)
		{	
		//	printf("Combining consecutive free chunks\n");
			head->totalSpace += (head->next->totalSpace + sizeof(memEntry));
			head-> next = head->next->next;
		//	printf("New Chunk Size: %i\n", head->totalSpace);

		}

	}
	else
	{
		printf("Not a valid pointer, please try agian\n");
	}
}
/**			CW:	just notes I wrote for myself about how to cannibalize the next chunk in a sequence
 * cannibalizing
 * if(this.isFree = 1 && this.next.isFree = 1)
 * {
 * 	this.size += (this.next.size + sizeof(memEntry);
 * 	this.next = next.next;
 * }
 * */


/**int main()
{
	int *P = malloc(40*sizeof(char));
	*P = 'g';
	free(P);
	*P = malloc(40*sizeof(char));
	free(P);
	int *e = malloc(40*sizeof(char));
	
	*e = 'g';
	free(P);
	int *f = malloc(80*sizeof(char));
	int *G = malloc(15*sizeof(int));
	*G = 50;
	int *T = malloc(500*sizeof(int));
	free(G);
	free(f);

	return 0;
}*/

//main is not used in this list, can't have 2 mains from memgrind.c and mymalloc.c


//CW: look it was just for testing, okay???
/*	
	int *P = malloc(40*sizeof(char));
	*P = 'g';
	free(P);
	*P = malloc(40*sizeof(char));
	free(P);
	int *e = malloc(40*sizeof(char));
	
	*e = 'g';
	free(P);
	int *f = malloc(80*sizeof(char));
	int *G = malloc(15*sizeof(int));
	*G = 50;
	int *T = malloc(500*sizeof(int));
	free(G);
	free(f);
*/	


