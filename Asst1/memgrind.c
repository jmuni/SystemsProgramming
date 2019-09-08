#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "mymalloc.h"
#define MAX 4096

void workloadA() {

	char* testA[MAX];
	
	int i;
	for(i = 0; i < 150; i++) {
		testA[i] = (char*)malloc(1); //malloc 1 byte
//		printf("Malloc %p \n", &testA[i]);		

		free(testA[i]); //free immediately
//		printf("Free %p \n", &testA[i]); 
	}
}

void workloadB() {

	char* testB[MAX];

	int i;
	int pos = 0;
	for(i = 0; i < 150; i++) { //malloc 150 bytes first
		testB[pos] = (char*)malloc(1);
		pos += (1+sizeof(memEntry)); 
//		printf("Malloc %p \n", &testB[i]);		
	}
	pos = 0;
	for(i = 0; i < 150; i++) {
		//pos-= (1+sizeof(memEntry));
		free(testB[pos]); //free the malloc'd 150 bytes
		pos += (1+sizeof(memEntry));
//		printf("Free %p \n", &testB[i]); 
	}

}


void workloadC() { //figuring out this one.


//random choose between malloc 1 byte or free 1 byte until 50 mallocs done
//once 50 malloc's have been done, finish the rest of the free operations
//dont free if there are no pointers to free (we cant have numMalloc < numFree

	int numMalloc = 0;
	int numFree = 0;
	int r;
	int pos = 0;
 
	char* testC[MAX];

	while(numFree < 50) {
		r = (rand() % 2); //r will be either 0 or 1
		if(r == 0 && numMalloc < 50) { //if r is 0, malloc
			testC[pos] = malloc(1);
			numMalloc++;
			pos+= (1+sizeof(memEntry));
		} else { //if r is 1, free
			if(numMalloc > numFree)
			{	
				pos -= (1+sizeof(memEntry));
				free(testC[pos]);
				numFree++;
			}
		}
	}

}	



void workloadD() {
 
// random choose between malloc random 1-64 bytes, or free a pointer
//once 50 malloc's have been done, finish rest of the free operations
//again, dont free if no pointers to free
 	int numMalloc = 0;
	int numFree = 0;
	int r;
	int q;
	int pos = 0;
	int sizes[50];
	int getSize = 0;
//looking up how to make a random number within a range
 
	char* testD[MAX];

	while(numFree < 50) {
		r = (rand() % 2); //r will be either 0 or 1
		if(r == 0 && numMalloc <50 && pos < MAX-96) { //if r is 0, and there are less than 50 mallocs (Edit, also does not allow for any allocation that could go over the alloted memory)
			q = (rand()%64) + 1;		
			testD[pos] = malloc(q);
			pos += (q+sizeof(memEntry));
			sizes[getSize] = q+sizeof(memEntry);
			getSize++;
			numMalloc++;
			//printf("Number of Mallocs: %i\n", numMalloc);
			//printf("Random Size Allocated: %i\n", q);
		} else { //if r is 1, or there are 50 mallocs
			if(numMalloc>numFree) //make sure there more mallocs than frees
			{
				getSize--;
				pos -= sizes[getSize];
				free(testD[pos]);
				numFree++;
			}
		}
	}

}	


void workloadE() {
int *A = malloc(200*sizeof(char));
*A = 34;
int *B = malloc(200*sizeof(char));
*B = 56;
int *C = malloc(200*sizeof(char));
*C = 678;
int *D = malloc(200*sizeof(char));
*D = 34;
int *E = malloc(200*sizeof(char));
*E = 3555;
int *F = malloc(200*sizeof(char));
*F = 40;
int *G = malloc(200*sizeof(char));
*G = 6456;
free(C);
free(D);
free(E);
int *H = malloc(500*sizeof(char));
*H = 3456;
int *I = malloc(2052*sizeof(char));
*I = 4563;
free(F);
int *J = malloc(250*sizeof(char));
if(J != NULL) //segfaults without this if the size requested is too large, assigning value to nothing.
{
*J = 345;
}
free(A);
free(B);
free(G);
free(H);
free(I);
free(J);

//workload of our choosing
//my idea: malloc, then reverse freeing

}



void workloadF() { //allocate 128 blocks of 32 bytes of memory, free them, then allocate 64 blocks of 64 bytes of memory, to ensure that the chunk combining is properly implimented

int i = 0;
int pos = 0;
char* testF[MAX];

while(i<128)
{
	testF[pos] = malloc(4*sizeof(int));
	pos += 32;
	i++;
}
pos = 0;
i = 0;
while(i<128)
{
	free(testF[pos]);
	pos+=32;
	i++;
}
//printf("\n\n\n\n\n\n\n END OF 32 BYTE FREES\n");

i = 0;
pos = 0;
while(i<64)
{
	testF[pos] = malloc(12*sizeof(int));
	pos +=64;
	i++;
}
pos = 0;
i = 0;
while(i<64)
{
	free(testF[pos]);
	pos+=64;
	i++;
}
//printf("\n\n\n\n\n\n\n END OF 64 BYTE ALLOCATION\n");

pos = 0;
i = 0;
testF[pos] = malloc(4096-sizeof(memEntry));
free(testF[pos]);
}



int main() {
	struct timeval start, end; //timeval structs
	double startTime, endTime, totalTime, averageTime;
	int i;
// 	srand(time(NULL));
	gettimeofday(&start, 0);
	startTime = (double)start.tv_sec + (double)start.tv_usec/1000000; //startTime in seconds + microseconds
	for(i = 0; i < 100; i++) {
		workloadA();
	}
	gettimeofday(&end, 0);

	endTime = (double)end.tv_sec + (double)end.tv_usec/1000000;  //endTime in seconds + microseconds
	totalTime = endTime - startTime;
	averageTime = totalTime / 100;

	printf("Workload A -----------------------------------\n");
	printf("Total Time 100 iterations: %lf seconds OR %lf microseconds\n", totalTime, totalTime * 1000000);
	printf("Average Time: %lf seconds OR %lf microseconds \n", averageTime, averageTime * 1000000);
//////////////////
	










	gettimeofday(&start, 0);
	startTime = (double)start.tv_sec + (double)start.tv_usec/1000000; //startTime in seconds + microseconds
	for(i = 0; i < 100; i++) {
		workloadB();
	}
	gettimeofday(&end, 0);

	endTime = (double)end.tv_sec + (double)end.tv_usec/1000000; //endTime in seconds + microseconds
	totalTime = endTime - startTime;
	averageTime = totalTime / 100;

	printf("Workload B -----------------------------------\n");
	printf("Total Time 100 iterations: %lf seconds OR %lf microseconds\n", totalTime, totalTime * 1000000);
	printf("Average Time: %lf seconds OR %lf microseconds \n", averageTime, averageTime * 1000000);
	
	












	gettimeofday(&start, 0);
	startTime = (double)start.tv_sec + (double)start.tv_usec/1000000; //startTime in seconds + microseconds
	for(i = 0; i < 100; i++) {
		workloadC();
	}
	gettimeofday(&end, 0);

	endTime = (double)end.tv_sec + (double)end.tv_usec/1000000; //endTime in seconds + microseconds
	totalTime = endTime - startTime;
	averageTime = totalTime / 100;

	printf("Workload C -----------------------------------\n");
	printf("Total Time 100 iterations: %lf seconds OR %lf microseconds\n", totalTime, totalTime * 1000000);
	printf("Average Time: %lf seconds OR %lf microseconds \n", averageTime, averageTime * 1000000);












	gettimeofday(&start, 0);
	startTime = (double)start.tv_sec + (double)start.tv_usec/1000000; //startTime in seconds + microseconds
	for(i = 0; i < 100; i++) {
		workloadD();
	}
	gettimeofday(&end, 0);

	endTime = (double)end.tv_sec + (double)end.tv_usec/1000000; //endTime in seconds + microseconds
	totalTime = endTime - startTime;
	averageTime = totalTime / 100;

	printf("Workload D -----------------------------------\n");
	printf("Total Time 100 iterations: %lf seconds OR %lf microseconds\n", totalTime, totalTime * 1000000);
	printf("Average Time: %lf seconds OR %lf microseconds \n", averageTime, averageTime * 1000000);

	
	






	gettimeofday(&start, 0);
	startTime = (double)start.tv_sec + (double)start.tv_usec/1000000; //startTime in seconds + microseconds
	for(i = 0; i < 100; i++) {
		workloadE();
	}
	gettimeofday(&end, 0);

	endTime = (double)end.tv_sec + (double)end.tv_usec/1000000;  //endTime in seconds + microseconds
	totalTime = endTime - startTime;
	averageTime = totalTime / 100;

	printf("Workload E -----------------------------------\n");
	printf("Total Time 100 iterations: %lf seconds OR %lf microseconds\n", totalTime, totalTime * 1000000);
	printf("Average Time: %lf seconds OR %lf microseconds \n", averageTime, averageTime * 1000000);









	gettimeofday(&start, 0);
	startTime = (double)start.tv_sec + (double)start.tv_usec/1000000; //startTime in seconds + microseconds
	for(i = 0; i < 100; i++) {
		workloadF();
	}
	gettimeofday(&end, 0);

	endTime = (double)end.tv_sec + (double)end.tv_usec/1000000; //endTime in seconds + microseconds
	totalTime = endTime - startTime;
	averageTime = totalTime / 100;

	printf("Workload F -----------------------------------\n");
	printf("Total Time 100 iterations: %lf seconds OR %lf microseconds\n", totalTime, totalTime * 1000000);
	printf("Average Time: %lf seconds OR %lf microseconds \n", averageTime, averageTime * 1000000);

	return 0;
}
