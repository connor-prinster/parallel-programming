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

const int MASTER = 0;
const int TASKS_PER_PROC = 4;
const int THRESHOLD = 16;

const int NO_DATA = -1;
const int WHITE = 99; // still work left
const int BLACK = 98; // no work left

int totalSize = 0;
int maxWork = 0;

void doWork(int taskSize);
bool isFull(int* arr);
bool isMaster(int rank);
void printQueue(int* arr, int size, int rank);
void calculateEnd(auto start);


int main(int argc, char** argv) {
	srand(time(NULL));
	auto start = std::chrono::system_clock::now();

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);
	int* tasks;
	int* queue;
	int totalJobs;

	int maxWork = rand() % 2048 + 1024;

	if(isMaster(rank)) {
		totalJobs = maxWork * size + 1;
		tasks = (int*)malloc(totalJobs * sizeof(int)); // make sure that you have enough work to at least exhaust every process
		for(int i = 0; i < totalJobs; i++) {
			tasks[i] = rand() % 1024 + 1;
		}
	}
	else {
		queue = (int*)malloc(THRESHOLD * sizeof(int));
		for(int i = 0; i < THRESHOLD; i++) {
			queue[i] = NO_DATA;
		}
	}
	
	MPI_Barrier(MCW);
	if (isMaster(rank)) {
		calculateEnd(start);
	}

	MPI_Finalize();
	return 0;
}

int randomProcess() {
	int maxProc = totalSize - 1;
	int randomProc = rand() % maxProc + 1;
	return randomProc;
}

void doWork(int taskSize) {
	int work = 0;
	int iterations = int(pow(double(taskSize), 2.0));
	for(int i = 0; i < iterations; i++) {
		work++;
	}
}

bool isFull(int* arr) {
	return (arr[THRESHOLD - 1] == NO_DATA);
}

bool isMaster(int rank) {
	return (rank == MASTER);
}

void printQueue(int* arr, int size, int rank) {
	cout << "Rank " << rank << " -> ";
	for(int i = 0; i < size; i++) {
		cout << arr[i] << " ";
	}
	cout << endl;
}

void calculateEnd(auto start) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "Runtime: " << elapsed_seconds.count() << "s" << std::endl << std::endl << std::endl;
}
