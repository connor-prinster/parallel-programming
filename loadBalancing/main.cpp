#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <fstream>
#include <random>
#include <sstream> 
#include <mpi.h>

#define MCW MPI_COMM_WORLD

using namespace std;

bool isMaster(int rank);
void printArr(int* arr, int numRows);
void calculateEnd(auto start);

int main(int argc, char** argv) {
	srand(time(NULL));
	auto start = std::chrono::system_clock::now();
	auto prevTime = start;

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);
	
	MPI_Barrier(MCW);
	if (isMaster(rank)) {
		calculateEnd(start);
	}

	MPI_Finalize();
	return 0;
}

bool isMaster(int rank) {
	return (rank == MASTER);
}

void printArr(int* arr, int numRows) {
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < WIDTH; j++) {
			printf("%d ", arr[(i * WIDTH) + j]);
		}
		printf("\n");
	}
	printf("\n\n\n");
}

void calculateEnd(auto start) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "Runtime: " << elapsed_seconds.count() << "s" << std::endl << std::endl << std::endl;
}
