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

	int dest = 0;
	int val = list[rank];
	int recv = 0;
	mask = 1;
	int phase = 0;
	while(phase < powOfTwo) {
//		printf("phase: %d\n\n\n", phase);
		mask = 1;
		mask <<= phase;
//		printf("rank: %d -> phase: %d -> mask: %d\n\n\n", rank, phase, mask);
		while(mask > 0) {
			dest = rank ^ mask;
			printf("rank: %d -> dest: %d\n", rank, dest);
			MPI_Send(&val, 1, MPI_INT, dest, 0, MCW);
			MPI_Recv(&recv, 1, MPI_INT, dest, 0, MCW, MPI_STATUS_IGNORE);
			int cending = !((rank & (mask << 1)) == 0);
//			printf("rank: %d -> recived: %d -> have: %d\n", rank, recv, val);
			val = ascDesc(cending, rank, mask, val, recv);
//			printf("rank: %d -> kept: %d\n", rank, val);
			mask >>= 1;
		}
		MPI_Barrier(MCW);
		phase++;
//		printf("\n\n");
		MPI_Barrier(MCW);
	}
	MPI_Barrier(MCW);
	printf("\nrank: %d -> val: %d\n", rank, val);

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
		val = ((rank & dmask) == 0) ? small : big;
	}
	else { // descending
		val = ((rank & dmask) == 0) ? big : small;

	}
//	printf("rank %d -> returning %d\n", rank, val);
	return val;
}

string decToBin(int dec) {
	return std::bitset<4>(dec).to_string();
}
