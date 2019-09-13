#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* qsort */
#include <vector>

#define MCW MPI_COMM_WORLD

using namespace std;

int compare (const void * a, const void * b);
void printArr(int arr[], int randLength);

int main(int argc, char **argv) {
	srand(time(NULL)); // init the time

	int rank, size, tag;
	int* list = NULL;
	vector<int> finished;
	int randomLength, perProc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	if(rank == 0) {
		/* this is the number of random numbers from 1 - 100 multiplied by
		the number of processes NOT the master so the numbers can be split
		evenly between all of the slave processors */
		randomLength = (rand() % 100 + 1) * (size - 1);
		perProc = randomLength / size;
		printf("random length: %d\n\n", randomLength, perProc);
		list = (int*) malloc(randomLength * sizeof(int));
		for(int i = 0; i < randomLength; i++) {
			list[i] = rand() % 500 + 1;
		}
		printf("UNSORTED ARRAY\n");
		printArr(list, randomLength);
	}

	// send out "perProc" and "randomLength" to every process
	MPI_Barrier(MCW);
	MPI_Bcast(&perProc, 1, MPI_INT, 0, MCW);
	MPI_Bcast(&randomLength, 1, MPI_INT, 0, MCW);
	MPI_Barrier(MCW);

	// get the list, scatter it, sort it, bring it back into a single array
	int *subList = (int*) malloc(perProc * sizeof(int));
	MPI_Scatter(list, perProc, MPI_INT, subList, perProc, MPI_INT, 0, MCW);
	qsort(subList, perProc, sizeof(int), compare);
	int* gatherList = NULL;
	if (rank == 0) {
		gatherList = (int*)malloc(randomLength * sizeof(int));
	}
	MPI_Gather(subList, perProc, MPI_INT, gatherList, perProc, MPI_INT, 0, MCW);

	// if the process is master, sort the final array and print it
	if (rank == 0) {
		qsort(gatherList, randomLength, sizeof(int), compare);
		printf("SORTED ARRAY\n");
		printArr(gatherList, randomLength);
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

// to be used in the qsort
int compare (const void * a, const void * b) {
	const int* x = (int*)a;
	const int* y = (int*)b;
	if (*x > * y) {
		return 1;
	} else if (*x < *y) {
		return -1;
	}
	return 0;
}
