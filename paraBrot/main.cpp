#include <iostream>
#include <stdio.h>
#include <chrono>
#include <ctime>
#include <string>
#include <complex>
#include <fstream>
#include <stddef.h>
#include <mpi.h>

#define MCW MPI_COMM_WORLD

using namespace std;

const int WIDTH = 512;
const int HEIGHT = 512;
const int AREA = WIDTH * HEIGHT;
const int MASTER = 0;

int generateValue(complex<double> complex);
void printToFile(int rank, int* arr, int size);

int main(int argc, char** argv) {
	// start on all the processes
	auto beg = std::chrono::system_clock::now();

	// set up the MPI stuff
	int rank, numProc;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &numProc);

	// if there aren't enough arguments, tell everyone they're wrong
	if(rank == MASTER && argc < 4) {
		printf("\nNot enough arguments. Requires 3 doubles as arguments\n");
		return -1;
	}

	if(rank == MASTER) {
		ofstream ofs;
		ofs.open("brot.ppm", std::ofstream::out | std::ofstream::trunc);
		ofs.close();
		printf("FILE WIPED\n\n");
	}


	MPI_Barrier(MCW);

	// get the arguments
	double data[4];
	double r0 = atof(argv[1]);
	double i0 = atof(argv[2]);
	double r1 = atof(argv[3]);
	double i1 = i0 + (r1 - r0);
	data[0] = r0;
	data[1] = i0;
	data[2] = r1;
	data[3] = i1;

	// initialize interval data
	int interval = 512/numProc;
	int start = interval * rank;
	int end = start + interval;

	//int store[interval][WIDTH];
	int store[interval * WIDTH];
	int fakeEye = start;
	int ctr = 0;

	for(int i = 0; i < interval; i++) {
		for(int j = 0; j < WIDTH; j++) {
			std::complex<double> complex (
				r0 + (j * (r1 - r0) / HEIGHT),
				i0 + (fakeEye * (i1 - i0) / WIDTH)
			);
			int val = generateValue(complex);
			store[ctr] = val;
			ctr++;
		}
		fakeEye++;
	}

	int dest = rank;
	if(rank == 0) {
		printToFile(rank, store, interval * WIDTH);
//		MPI_Send(store, (interval * WIDTH), MPI_INT, 1, 0, MCW);
		MPI_Send(&dest, 1, MPI_INT, 1, 0, MCW);
	} else {
//		MPI_Recv(store, (interval * WIDTH), MPI_INT, (dest - 1), 0, MCW, MPI_STATUS_IGNORE);
		MPI_Recv(&dest, 1, MPI_INT, (dest - 1), 0, MCW, MPI_STATUS_IGNORE);
		printToFile(rank, store, interval * WIDTH);
		if(rank != (numProc - 1)) { // not the last process
			MPI_Send(&dest, 1, MPI_INT, (rank + 1), 0, MCW);
		}
	}


	// end the timing
	if(rank == (numProc - 1)) {
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - beg;
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
	}

	MPI_Finalize();
	return 0;
}

int generateValue(complex<double> complex) {

	std::complex<double> point((float)complex.real() / WIDTH - 1.5, (float)complex.imag() / HEIGHT - 0.5);
	std::complex<double> curr(0, 0);

	int iter = 0; //mandelbrot require that a set remain bounded (by 2 here) after any number of iterations
	const int MAX_ITERS = 30;
	while (abs(curr) < 2 && iter < MAX_ITERS) {
		curr = curr * curr + point;
		iter++;
	}

	return (iter < 10) ? 0 : 255;
}

void printToFile(int rank, int* arr, int size) {
	// check to see if you can even open the ppm file
	fstream mandel("brot.ppm", fstream::app);
	if (!mandel.is_open()) {
		printf("you failed, we'll open 'em next time\n\n");
	}
	if(rank == MASTER) {
		mandel << "P3\n" << WIDTH << " " << HEIGHT << " 255\n"; // this particular line is for the header
	}

	cout << "Rank " << rank << " Printing to file" << endl;

	for(int i = 0; i < size; i++) {
		mandel << arr[i] << " " << 0 << " " << 0 << " ";;
	}

	mandel << "\n\n";

	cout << "Rank " << rank <<  " Done Printing" << endl;

	mandel.close();
}

