#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* qsort */

#define MCW MPI_COMM_WORLD

using namespace std;

void printArr(int arr[], int randLength);

int main(int argc, char **argv) {
	srand(time(NULL)); // init the time

	int rank, size;
	int* list = NULL;
	// what power of two to go to
	// what the actual size is
	int powOfTwo = 0;
	int powSize = 1;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	while(powSize<size){
        	powSize<<=1;
        	powOfTwo++;
    	}

	printf("powsize: %d -> powoftwo: %d\n", powSize, powOfTwo);

	if(rank == 0) {
		int len = 32; //2 ^ (rand() % 8 + 4)
		list = (int*) malloc(len * sizeof(int));
		for(int i = 0; i < len; i++) {
			list[i] = rand() % 100 + 1;
		}
//		printArr(list, len);
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
