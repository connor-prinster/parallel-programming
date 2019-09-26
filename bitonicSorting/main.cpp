#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* qsort */

#define MCW MPI_COMM_WORLD

using namespace std;

void printArr(int arr[], int randLength);
void compAndSwap(int a[], int i, int j, int direction);
void bitonicMerge(int a[], int low, int cnt, int dir);
void bitonicSort(int a[], int low, int cnt, int dir); 
void swap(int a[], int i, int j);
int* genRandList(len);

int main(int argc, char **argv) {
	srand(time(NULL)); // init the time

	int rank, size;

	// the list of random numbers
	int* list = NULL;
	// what power of two to go to
	int powOfTwo = 0;
	// the number of processors
	int powSize = 1;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	while(powSize<size){
       	powSize<<=1;
       	powOfTwo++;
    }

	// generate the random list
	if(rank == 0) {
		int len = powSize << 1;
		list = (int*) malloc(len * sizeof(int));
		for(int i = 0; i < len; i++) {
			list[i] = rand() % 100 + 1;
		}
		printArr(list, len);
	}

	MPI_Finalize();

	return 0;
}

void printArr(int* arr, int randLength) {
	if(arr != NULL) {
		for(int i = 0; i < randLength; i++) {
			printf("%d ", arr[i]);
		}
		printf("\n\n\n");
	}
}

// FROM GEEKSFORGEEKS
void compAndSwap(int a[], int i, int j, int direction) { 
    if (direction==(a[i]>a[j])) 
        swap(a, i, j); 
} 
  
// FROM GEEKSFORGEEKS
void bitonicMerge(int a[], int low, int cnt, int dir) { 
    if (cnt>1) { 
        int k = cnt/2; 
        for (int i=low; i<low+k; i++) 
            compAndSwap(a, i, i+k, dir); 
        bitonicMerge(a, low, k, dir); 
        bitonicMerge(a, low+k, k, dir); 
    } 
} 

void swap(int a[], int i, int j) {
	int k = a[i];
	a[i] = a[j];
	a[j] = k;
}

// FROM GEEKSFORGEEKS
void bitonicSort(int a[],int low, int cnt, int dir) { 
    if (cnt>1) 
    { 
        int k = cnt/2; 
  
        // sort in ascending order since dir here is 1 
        bitonicSort(a, low, k, 1); 
  
        // sort in descending order since dir here is 0 
        bitonicSort(a, low+k, k, 0); 
  
        // Will merge wole sequence in ascending order 
        // since dir=1. 
        bitonicMerge(a,low, cnt, dir); 
    } 
} 