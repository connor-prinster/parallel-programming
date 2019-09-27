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

int ascDesc(int cending, int rank, int dmask, int a, int b);
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
	int* list = (int*) malloc(powSize * sizeof(int));

	int mask = size;
	int dmask = mask;

	if(rank == 0) {
		int fakeList[powSize] = {4, 3, 0, 7, 1, 6, 2, 5};
//		int fakeList[powSize] = {2, 4, 6, 8, 7, 5, 3, 1};
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
	dmask = 1;
	int amask = 2;
	while(dmask < mask) {
		int dest = (rank ^ dmask);
		MPI_Send(&val, 1, MPI_INT, dest, 0, MCW);
		MPI_Recv(&recv, 1, MPI_INT, dest, 0, MCW, MPI_STATUS_IGNORE);

		//cending would be 1 if (rank & amask == 0)
		int cending = !((rank & amask) == 0);
//		printf("rank %d -> amask %d -> cending ->%d\n", rank, amask, cending);
		val = ascDesc(cending, rank, dmask, val, recv);
//		printf("rank: %d -> keptVal: %d\n", rank, val);
		dmask <<= 1;
		amask <<= 1;
	}
//	printf("rank: %d -> val: %d\n\n", rank, val);

//	recv = 0;
//	dmask = mask >> 1;
//	int dest = (rank ^ dmask);
//	while(dmask > 0) {
//		MPI_Send(&val, 1, MPI_INT, dest, 0, MCW);
//		MPI_Recv(&recv, 1, MPI_INT, dest, 0, MCW, MPI_STATUS_IGNORE);
//		val = ascDesc(0, rank, dmask, val, recv);
//		dmask >>= 1;
//	}

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

	int val = 0;
	int big, small;
	if (a > b) {
		big = a;
		small = b;
	}
	else {
		big = b;
		small = a;
	}
	//printf("rank: %d -> dmask: %d -> cending -> %d -> big: %d -> small %d\n", rank, dmask, cending,  big, small);
	if (cending == 0) { // ascending
		if ((rank & dmask) == 0) {
		//	return small;
			val = small;
		}
		else {
//			return big;
			val = big;
		}
	}
	else { // descending
		if ((rank & dmask) == 0) {
//			return big;
			val = big;
		}
		else {
//			return small;
			val = small;
		}
	}
	printf("rank %d -> returning %d\n", rank, val);
	return val;
}

string decToBin(int dec) {
	return std::bitset<4>(dec).to_string();
}
