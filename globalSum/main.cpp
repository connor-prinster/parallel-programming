#include <iostream>
#include <stdio.h>
#include <chrono>
#include <ctime>
#include <string>
#include <complex>
#include <fstream>
#include <mpi.h>

#define MCW MPI_COMM_WORLD

using namespace std;
const int MASTER = 0;

int generateRandom();
void ringAroundTheNeighbor(int rank, int heldVal);
void trustYourLeader(int rank, int heldVal);

int main(int argc, char** argv) {
	srand(time(NULL));
	auto start = std::chrono::system_clock::now();

	int choice;
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	if(argc > 1 && atoi(argv[1]) < 2) {
		choice = atoi(argv[1]);
	} else {
		if(rank == MASTER) {
			printf("Invalid or no input. Choosing 0 as default\n\n\n");
		}
		choice = 0;
	}

	int heldVal = generateRandom();

	trustYourLeader(rank, heldVal);
	ringAroundTheNeighbor(rank, heldVal);

	if(rank == MASTER) {
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
	}

	MPI_Finalize();
	return 0;
}

int generateRandom() {
	return (rand() % 100 + 1);
}

void ringAroundTheNeighbor(int rank, int heldVal) {

}

void trustYourLeader(int rank, int heldVal) {

}

