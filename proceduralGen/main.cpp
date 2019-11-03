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
const int WIDTH = 32;
const int HEIGHT = 32;
const int AREA = HEIGHT * WIDTH;
const string FILENAME = "procedural";

void calculateEnd(auto start, int size);
bool isMaster(int rank);
void printToFile(int* arr, int numRows);
void generateWorld();

int main(int argc, char** argv) {
	srand(time(NULL));
	auto start = std::chrono::system_clock::now();
	auto prevTime = start;

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

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

void calculateEnd(auto start) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "Runtime: " << elapsed_seconds.count() << "s" << std::endl << std::endl << std::endl;
}

string returnString(int input) {
    string null = "-255 -255 -255";
    string stone = "128 128 128";
    string dirt = "102 51 0";
    string sky = "0 204 204";
    string cloud = "255 255 255";
    string water = "0 0 204";
    string sand = "255 255 204";
    string grass = "0 204 102";
    string ice = "204 255 255";
    string lava = "255 153 50";
    string cave = "32 32 32";
    string wood = "153 153 0";

    if(input == 0) {
        return stone;
    }
    else if(input == 1) {
        return dirt;
    }
    else if(input == 2) {
        return sky;
    }
    else if(input == 3) {
        return cloud;
    }
    else if(input == 4) {
        return water;
    }
    else if(input == 5) {
        return sand;
    }
    else if(input == 6) {
        return grass;
    }
    else if(input == 7) {
        return ice;
    }
    else if(input == 8) {
        return lava;
    }
    else if(input == 9) {
        return cave;
    }
    else if(input == 10) {
        return wood;
    }
    else {
        return null;
    }
}

void printToFile(int* arr, int numRows) {
    string stone = "128 128 128";
    string stone = "102 51 0";
    string sky = "0 204 204";
    string cloud = "255 255 255";
    string water = "0 0 204";
    string sand = "255 255 204";
    string grass = "0 204 102";
    string ice = "204 255 255";
    string lava = "255 153 50";
    string cave = "32 32 32";
    string wood = "153 153 0";
}

