#include <iostream>
#include <chrono>
#include <ctime>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <mpi.h>

#define MCW MPI_COMM_WORLD

using namespace std;
const int ITERATIONS = 9;
const int MASTER = 0;
const int WIDTH = 32;
const int HEIGHT = 32;
const int AREA = HEIGHT * WIDTH;
const string FILENAME = "procedural";

void calculateEnd(auto start);
bool isMaster(int rank);

int main(int argc, char** argv) {
	srand(time(NULL));
	auto start = std::chrono::system_clock::now();
	auto prevTime = start;

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	MPI_Barrier(MCW);
	if(isMaster(rank)) {
		calculateEnd(start);
	}

	MPI_Finalize();
	return 0;
}

bool isMaster(int rank) {
	return (rank == MASTER);
}

void calculateEnd(auto start) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "Runtime: " << elapsed_seconds.count() << "s" << std::endl << std::endl << std::endl;
}