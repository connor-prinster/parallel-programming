#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* qsort */
#include <vector>

#define MCW MPI_COMM_WORLD

using namespace std;

int main(int argc, char **argv) {
	srand(time(NULL)); // init the time

	int rank, size;
	int* list = NULL;
	int randomLength, perProc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	printf("hello\n");

	MPI_Finalize();

	return 0;
}
