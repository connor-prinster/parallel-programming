#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <bitset>
#include <iostream>
#include <sstream>

#define MCW MPI_COMM_WORLD

using namespace std;

// reminder that 0 = Ascending
// reminder that 1 = Decending

void printArr(int arr[], int randLength);

int main(int argc, char **argv) {
	srand(time(NULL)); // init the time

	int rank, size;

	// what power of two to go to
	int powOfTwo = 0;
	// the number of processors
	int powSize = 1;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	while(powSize<size) {
	       	powSize <<= 1;
       		powOfTwo++;
    	}
	// the list of random numbers
	int* list = (int*) malloc(powSize * sizeof(int));

	int mask = size;

	// generate the random list
	if(rank == 0) {
		int fakeList[powSize] = {2, 4, 6, 8, 7, 5, 3, 1};
		list = (int*) malloc(powSize * sizeof(int));
		for(int i = 0; i < powSize; i++) {
			list[i] = fakeList[i];
		}
		printArr(list, powSize);
	}

	MPI_Bcast(list, powSize, MPI_INT, 0, MCW);
	MPI_Barrier(MCW);

	int val = list[rank];
	int recv = 0;
	mask >>= 1;
	int dest = (rank ^ mask);
//	cout << "rank: " << rank << " -> dest: " << dest << " -> val: " << val << endl;

	MPI_Send(&val, 1, MPI_INT, dest, 0, MCW);
	MPI_Recv(&recv, 1, MPI_INT, dest, 0, MCW, MPI_STATUS_IGNORE);
	printf("rank: %d -> val: %d -> recv: %d\n\n", rank, val, recv);

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

//ascending = 0, decending = 1;
int ascDesc(int cending, int rank, int dmask, int a, int b) {
	int big, small;
	if (a > b) {
		big = a;
		small = b;
	}
	else {
		big = b;
		small = a;
	}

	if (cending == 0) { // ascending
		if (rank & dmask == 0) {
			// return smaller
			return small;
		}
		else {
			// return bigger
			return big;
		}
	}
	else { // descending
		if (rank & dmask == 0) {
			// return bigger
			return big;
		} 
		else {
			// return smaller
			return small;
		}
	}
}

string decToBin(int dec) {
	return std::bitset<4>(dec).to_string();
}
