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
const int WIDTH = 1024;
const int HEIGHT = 1024;
const int AREA = HEIGHT * WIDTH;
const string FILENAME = "gol";

void calculateEnd(auto start, int size);
bool isMaster(int rank);
int returnStatus(int neighborCount, int currentStatus);
int* readFromFile();
void printToFile(int* arr, int count);
int startingIndexAdjustment(int rank, int size, int perProc);
int generateNumRowsToSend(int rank, int size, int perProc);
void printArr(int* arr, int size);
int* analyzeNeighbors(int* arr, int numRows, int arrSize);
void calculateDayTime(auto start, int count);

int main(int argc, char** argv) {
	srand(time(NULL));
	auto start = std::chrono::system_clock::now();
	auto prevTime = start;

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	int* dataArr;
	int perProc = HEIGHT / size;
	int sendSize = generateNumRowsToSend(rank, size, perProc) * WIDTH;
	int* recvArr = (int*) malloc(sendSize * sizeof(int));

	if(isMaster(rank)) { // only fill the array if you need to
		dataArr = (int*) malloc(AREA * sizeof(int));
		dataArr = readFromFile();
		printf("\n------------- Day 0 -------------\n");
		printToFile(dataArr, 0);
		printf("\n\n");
	}

	for(int iters = 0; iters < ITERATIONS; iters++) {
		auto tempStart = std::chrono::system_clock::now();
		if(isMaster(rank)) { // send to the other ranks 1 -> size - 1
			// allocate memory for itself
			for(int i = 0; i < sendSize; i++) {
				recvArr[i] = dataArr[i];
			}

			for(int i = 1; i < size; i++) {
				int tempRecv = startingIndexAdjustment(i, size, perProc);
				int tempSendSize = generateNumRowsToSend(i, size, perProc) * WIDTH;
				MPI_Send(&dataArr[tempRecv], tempSendSize, MPI_INT, i, 0, MCW);
			}

			// --- do work for itself (rank 0) --- //
			int numRows = generateNumRowsToSend(rank, size, perProc);
			analyzeNeighbors(recvArr, numRows, sendSize);
			for(int i = 0; i < numRows - 1; i++) {
				for(int j = 0; j < WIDTH; j++) {
					dataArr[(i * WIDTH) + j] = recvArr[(i * WIDTH) + j];
				}
			}

			for(int i = 1; i < size; i++) {
				int recvSize = perProc * WIDTH;
				int* tempRecvArr = (int*) malloc(recvSize * sizeof(int));
				MPI_Recv(tempRecvArr, recvSize, MPI_INT, i, 0, MCW, MPI_STATUS_IGNORE);
				for (int m = 1; m <= perProc; m++) {
					for(int n = 0; n < WIDTH; n++) {
						dataArr[i * ((m * WIDTH) + n)] = tempRecvArr[(m * WIDTH) + n];
					}
				}
				delete tempRecvArr;
			}
		}
		else {
			MPI_Recv(recvArr, sendSize, MPI_INT, MASTER, 0, MCW, MPI_STATUS_IGNORE);
			int numRows = generateNumRowsToSend(rank, size, perProc);
			analyzeNeighbors(recvArr, numRows, sendSize);

			// send back a "pruned" array
			int returnRows = (rank == (size - 1) ? numRows - 1 : numRows - 2);
			int returnSize = returnRows * WIDTH;
			int* returnArray = (int*) malloc(returnSize * sizeof(int));
			int ctr = 0;
			for(int i = 1; i <= returnRows; i++) {
				for(int j = 0; j < WIDTH; j++) {
					returnArray[ctr] = recvArr[(i * WIDTH) + j];
					ctr++;
				}
			}

			MPI_Send(returnArray, returnSize, MPI_INT, 0, 0, MCW);
		}
		if(isMaster(rank)) {
			printf("------------- Day %d -------------\n", iters + 1);
			printToFile(dataArr, iters + 1);
			calculateDayTime(tempStart, iters + 1);
			printf("\n\n");
		}
	}

	MPI_Barrier(MCW);
	if(isMaster(rank)) {
		calculateEnd(start, size);
	}

	MPI_Finalize();
	return 0;
}

bool isMaster(int rank) {
	return (rank == MASTER);
}

void calculateEnd(auto start, int size) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	
	std::cout << "======= FINAL REPORT =======" << std::endl;
	std::cout << "Average Runtime: " << (elapsed_seconds.count() / size) << "s" << std::endl;
	std::cout << "Total Runtime: " << elapsed_seconds.count() << "s" << std::endl << std::endl << std::endl;
}

void calculateDayTime(auto start, int count) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "Day Length: " << elapsed_seconds.count() << "s\n";
}
 
int returnStatus(int neighborCount, int currentStatus) { // died of loneliness or overcrowding
	// regardless of if the cell is currently alive or 
	// dead, this will end up as a 0 due to loneliness
	// or overpopulation respectively
	if(neighborCount < 2 || neighborCount > 3) {
		return 0;
	}
	else if (neighborCount == 3) { // cell is now a baby!
		return 1;
	}
	else {
		return currentStatus;
	}

}

int* readFromFile() {
	int* arr = (int*)malloc(AREA * sizeof(int));
	
	for(int i = 0; i < AREA; i++) {
		int tmp = rand() % 100;
		arr[i] = ( (tmp % 5 == 0) ? 1 : 0);
	}

	return arr;
}

void printToFile(int* arr, int count) {
	string newFile = FILENAME + to_string(count) + ".ppm";
	ofstream of(newFile);

	if(of.is_open()) {
		of << "P3\n" << WIDTH << " " << HEIGHT << " 255\n";
		for(int i = 0; i < AREA; i++) {
			string val = "0 0 0";
			if(arr[i] == 0) {
				val = "255 255 255";
			}
			of << val << " ";
			if(i != 0 && (i + 1) % HEIGHT == 0) {
				of << "\n";
			}
		}
	}
	of.close();
	printf("State saved to filename: gol%d.ppm\n", count, count);
}

int startingIndexAdjustment(int rank, int size, int perProc) {
	if(isMaster(rank)) {
		return 0;
	}
	else {
		// returning the following minus the WIDTH
		// allows for the starting value to be one
		// row less than it normally would be
		return ( (rank * perProc * WIDTH) - WIDTH );
	}
}

int generateNumRowsToSend(int rank, int size, int perProc) {
	return (isMaster(rank) || rank == (size - 1)) ? (perProc + 1) : (perProc + 2);
}

int* analyzeNeighbors(int* arr, int numRows, int arrSize) {
	int rowSize = arrSize / numRows;
	int* returnArray = (int*) malloc(arrSize * sizeof(int));

	for(int i = 1; i < numRows - 1; i++) {
		for(int j = 1; j < rowSize - 1; j++) {
			int neighbors = 0;

			for (int m = -1; m <= 1; m++) {
				for (int n = -1; n <= 1; n++) {
					neighbors += arr[(i * rowSize + m) + (j + n)];
				} 
			}

			neighbors -= arr[(i * rowSize) + j];

			returnArray[(i * rowSize) + j] = returnStatus(neighbors, arr[(i * rowSize) + j]);
		}
	}

	for(int i = 0; i < numRows; i++) {
		for(int j = 0; j < rowSize; j++) {
			arr[(i * rowSize) + j] = returnArray[(i * rowSize) + j];
		}
	}

	delete returnArray;
}

void printArr(int* arr, int size) {
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < WIDTH; j++) {
			printf("%d ", arr[(i * WIDTH) + j]);
		}
		printf("\n");
	}
	printf("\n\n\n");
}