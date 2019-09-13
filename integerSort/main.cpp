#include <iostream>
#include <mpi.h>
#include <stdio.h>

#define MCW MPI_COMM_WORLD

using namespace std;

int main(int argc, char **argv) {
	int rank, size;
	int data;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);


	MPI_Finalize();

	return 0;
}
