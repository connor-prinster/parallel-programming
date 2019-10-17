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

void masterSlave(int rank, int size);
void ringAroundTheNeighbor(int rank, int size);
void halfRing(int rank, int size);
void calculateEnd(auto start, string message);
bool master(int rank);
void finishVal(int total);

int main(int argc, char** argv) {
	auto start = std::chrono::system_clock::now();

	int choice;
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	masterSlave(rank, size);
	ringAroundTheNeighbor(rank, size);
	halfRing(rank, size);

	MPI_Finalize();
	return 0;
}

bool master(int rank) {
	return (rank == MASTER);
}

void masterSlave(int rank, int size) {
	auto start = std::chrono::system_clock::now();

	if(!master(rank)) { // send all processes to 0
		MPI_Send(&rank, 1, MPI_INT, 0, 0, MCW);
	}

	if(master(rank)) {
		int total = 0;
		for(int i = 1; i < size; i++) {
			int recv = 0;
			MPI_Recv(&recv, 1, MPI_INT, i, 0, MCW, MPI_STATUS_IGNORE);
			total += recv;
		}

		finishVal(total);
		calculateEnd(start, "Master/Slave");
	}
}

void ringAroundTheNeighbor(int rank, int size) {
	auto start = std::chrono::system_clock::now();

	if(master(rank)) {
		MPI_Send(&rank, 1, MPI_INT, 1, 0, MCW);
	}

	int total = 0;
	MPI_Recv(&total, 1, MPI_INT, (rank - 1) % size, 0, MCW, MPI_STATUS_IGNORE);
	total += rank;
	MPI_Send(&total, 1, MPI_INT, (rank + 1) % size, 0, MCW);

	if(master(rank)) {
		MPI_Recv(&total, 1, MPI_INT, 7, 0, MCW, MPI_STATUS_IGNORE);
		finishVal(total);
		calculateEnd(start, "Running the Ring");
	}
}

void halfRing(int rank, int size) {
	auto start = std::chrono::system_clock::now();
	int total = 0;
	int half = size / 2;

	// below examples are for n=8 processors
	if(master(rank)) { // 0 goes to 1 & 4
		MPI_Send(&rank, 1, MPI_INT, 1, 0, MCW); // 1
		MPI_Send(&rank, 1, MPI_INT, half, 0, MCW);	// 4

		MPI_Recv(&rank, 1, MPI_INT, half - 1, 0, MCW, MPI_STATUS_IGNORE); // get from 3
		total += rank;
		MPI_Recv(&rank, 1, MPI_INT, size - 1, 0, MCW, MPI_STATUS_IGNORE); // get from 7
		total += rank;

		finishVal(total);
		calculateEnd(start, "Half Ring");
	} 
	else if (rank < half) { // 1, 2, 3
		int recv = rank - 1;
		int dest = (rank + 1) % half;

		MPI_Recv(&total, 1, MPI_INT, recv, 0, MCW, MPI_STATUS_IGNORE);
		total += rank;
		MPI_Send(&total, 1, MPI_INT, dest, 0, MCW);
	} 
	else if( rank >= half) { // 4, 5, 6, 7
		int recv = rank - 1;
		int dest = half + ((rank + 1) % half);

		if(rank == half) {
			MPI_Recv(&total, 1, MPI_INT, 0, 0, MCW, MPI_STATUS_IGNORE); // from 0
		} else {
			MPI_Recv(&total, 1, MPI_INT, recv, 0, MCW, MPI_STATUS_IGNORE); // anything else
		}

		total += rank;

		if(rank == (size - 1)) {
			MPI_Send(&total, 1, MPI_INT, 0, 0, MCW);
		} else {
			MPI_Send(&total, 1, MPI_INT, dest, 0, MCW);
		}

	}
}

void calculateEnd(auto start, string message) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << message << ": " << elapsed_seconds.count() << "s\n\n";
}

void finishVal(int total) {
	cout << "finish val: " << total << endl;
}

