#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

#define MCW MPI_COMM_WORLD

using namespace std;

int main(int argc, char **argv) {
	srand(time(NULL));

	int rank, size;
	int data;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	// random numbers for time and process chosen
	int currTime = rand() % 10 + 1;
	int randProc = rand() % size;

	// will decrement when received, so this must stop if
	//the timer is received at time 1
	while( currTime > 0 ) {
		sleep(1);
		if( rank == randProc ) {
			// just in case randomizing randProc sets itself to itself
			while(rank == randProc) {
				randProc = rand() % size;
			}
			printf("rank: %d -> passing to proc: %d -> with timer %d\n", rank, randProc, currTime );
			// send the random time to the randomly chosen process
			MPI_Send( &currTime, 1, MPI_INT, randProc, 0, MCW );
		} else {
			MPI_Recv( &currTime, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE );
			printf("proc %d -> received bomb\n",
				rank, currTime);
			currTime--;
			printf("time decremented to %d\n", currTime);
			if(currTime == 0) {
				printf("\n\nTHE BIGGEST LOSER IS %d\n\n", rank);
				MPI_Abort(MCW, 0);
			}
			randProc = rand() % size;
			while(rank == randProc) {
				randProc = rand() % size;
			}
			printf("rank: %d -> passing to proc: %d -> with timer %d\n", rank, randProc, currTime );
			MPI_Send( &currTime, 1, MPI_INT, randProc, 0, MCW );
		}
	}

	MPI_Finalize();

	return 0;
}
