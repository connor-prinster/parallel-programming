#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* qsort */
#include <string>

#define MCW MPI_COMM_WORLD

using namespace std;

void printArr(int arr[], int randLength);
int* decimalBinary(int dec);

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
       	powSize <<= 1;
       	powOfTwo++;
    }

	// generate the random list
	if(rank == 0) {
		int len = powSize << 1;
		int fakeList[powSize] = {4, 3, 0, 7, 1, 6, 2, 5};
		list = (int*) malloc(len * sizeof(int));
		for(int i = 0; i < len; i++) {
			list[i] = fakeList[i];
			decimalBinary(list[i]);
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

int* decimalBinary(int dec) {
	int binaryNum[4];

	int i = 0;
	while (n > 0) { 
        binaryNum[i] = n % 2; 
		printf(binaryNum[i])
        n = n / 2; 
        i++; 
    } 
	printf("\n");

	return binaryNum;
}