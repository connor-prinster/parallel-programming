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

int main(int argc, char **argv) {
	srand(time(NULL)); // init the time

	int rank, size;

	int list[] = NULL;
	// what power of two to go to
	int powOfTwo = 0;
	// the number of processors
	int powSize = 1;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	while(powSize<size){
       	powSize <<= 1;
       	powOfTwo++;
    }

	// generate the random list
	if(rank == 0) {
		int len = powSize << 1;
		list = {2, 4, 6, 8, 7, 5, 3, 1};
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