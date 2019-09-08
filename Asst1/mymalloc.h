#ifndef MYMALLOC_H_
#define MYMALLOC_H_

#include <stdio.h>
#include <stdlib.h> 
#include <stddef.h>
//#define RAM 4096
#define  malloc(x) myMalloc(x, __FILE__, __LINE__)
#define free(x) myFree(x, __FILE__,__LINE__ )

//static char memory[RAM];
//also moved to mymalloc.c

typedef struct memEntry
{
	int totalSpace;
	int isFree;
	struct memEntry *next;
} memEntry;

//struct memEntry *head=(void*)memory;
//moved this like to mymalloc.c memgrind is getting errors cuz this gets called twice

void *myMalloc(size_t size, const char *file, int line);
void myFree(void* ptr, const char *file, int line);

#endif

