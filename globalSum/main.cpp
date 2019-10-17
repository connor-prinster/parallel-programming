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
void masterSlave(int rank);
void ringAroundTheNeighbor(int rank);
void halfRing(int rank);
void calculateEnd(auto start, string message);

int main(int argc, char** argv) {
	srand(time(NULL));
	auto start = std::chrono::system_clock::now();

	int choice;
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	masterSlave(rank);
	ringAroundTheNeighbor(rank);
	halfRing(rank);

	MPI_Finalize();
	return 0;
}

int generateRandom() {
	return (rand() % 100 + 1);
}

void masterSlave(int rank) {
	auto start = std::chrono::system_clock::now();

	if(rank == MASTER) {
		calculateEnd(start, "Master/Slave");
	}
}

void ringAroundTheNeighbor(int rank) {
	auto start = std::chrono::system_clock::now();

	if(rank == MASTER) {
		calculateEnd(start, "Running the Ring");
	}
}

void halfRing(int rank) {
	auto start = std::chrono::system_clock::now();

	if(rank == MASTER) {
		calculateEnd(start, "Half-ring");
	}
}

void calculateEnd(auto start, string message) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << message << ": " << elapsed_seconds.count() << "s\n\n";
}

