#include <iostream>
#include <chrono>
#include <ctime>
#include <time.h>
#include <mpi.h>

#define MCW MPI_COMM_WORLD

using namespace std;
const int MASTER = 0;
const int WIDTH = 1024;
const int HEIGHT = 1024;

void calculateEnd(auto start);
bool isMaster(int rank);
int returnStatus(int neighborCount);
bool fellOffWorld(int x, int y);
int returnRand();

int main(int argc, char** argv) {
	srand(time(NULL));
	auto start = std::chrono::system_clock::now();

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

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
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "Runtime: " << elapsed_seconds.count() << "s\n\n";
}

/*
	returns 0 if person needs to die
	returns 1 if the neighbor will just survive
	returns 2 if the neighbor is supposed to make baby
*/
int returnStatus(int neighborCount) { // died of loneliness or overcrowding
	if(neighborCount < 2 || neighborCount > 3) {
		return 0;
	}
	else if (neighborCount == 2) { // will survive, but there are no children
		return 1;
	}
	else (neighborCount == 3) { // will survive AND make a baby
		return 2;
	}
}

bool fellOffWorld(int x, int y) {
	return (x > WIDTH || x < 0 || y > HEIGHT || y < 0)
}

int returnRand() {
	return (rand() % (WIDTH - 1));
}

