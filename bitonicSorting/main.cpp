#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* qsort */
#include <string>
#include <bitset>
#include <iostream>
#include <sstream>

#define MCW MPI_COMM_WORLD

using namespace std;

void printArr(int arr[], int randLength);
string decToBin(int dec);

int main(int argc, char **argv) {
	srand(time(NULL)); // init the time

	int rank, size;

	// the list of random numbers
	int* list = NULL;
	string* binList = NULL;
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
		int fakeList[powSize] = {4, 3, 0, 7, 1, 6, 2, 5};
		list = (int*) malloc(len * sizeof(int));
		binList = (string*) malloc(len * sizeof(string));
		for(int i = 0; i < len; i++) {
			list[i] = fakeList[i];
		}
	}
	MPI_Barrier(MCW);

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

string decToBin(int dec) {
	return std::bitset<4>(dec).to_string();
}
