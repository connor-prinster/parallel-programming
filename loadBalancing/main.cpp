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
#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

#define MCW MPI_COMM_WORLD

using namespace std;

const int MASTER = 0;
const int TASKS_PER_PROC = 4;
const int THRESHOLD = 16;
const int MAX_WORK = 200;
const int MIN_WORK = 100;

const int TESTPROC = 7;

const int NO_DATA = -1;
const int DONE = 1;
const int NOT_DONE = 2;
const int DATA_SENDS = 3;
const int LIFE_SENDS = 4;
const int POISON = 9;
const int NOT_POISON = 8;
const int WORK_TO_SEND = 3;

int globalRank = 0;
int totalSize = 0;
int maxWork = 0;
int workDone = 0;
int procStatus = NOT_DONE;

class Queue {
	int *arr;		// array to store queue elements
	int capacity;	// maximum capacity of the queue
	int front;		// front points to front element in the queue (if any)
	int rear;		// rear points to last element in the queue
	int count;		// current size of the queue

public:
	Queue();
	Queue(int size = THRESHOLD); 	// constructor
	~Queue();   				// destructor

	void dequeue();
	void enqueue(int x);
	int peek();
	int size();
	bool isEmpty();
	bool isFull();
};

// Constructor to initialize queue
Queue::Queue(int size) {
	arr = new int[size];
	capacity = size;
	front = 0;
	rear = -1;
	count = 0;
}

Queue::Queue() { }

// Destructor to free memory allocated to the queue
Queue::~Queue() {
	delete arr;
}

// Utility function to remove front element from the queue
void Queue::dequeue()
{
	// check for queue underflow
	if (isEmpty()) {
		exit(EXIT_FAILURE);
	}

	front = (front + 1) % capacity;
	count--;
}

// Utility function to add an item to the queue
void Queue::enqueue(int item)
{
	// check for queue overflow
	if (isFull()) {
		exit(EXIT_FAILURE);
	}

	rear = (rear + 1) % capacity;
	arr[rear] = item;
	count++;
}

// Utility function to return front element in the queue
int Queue::peek() {
	if (isEmpty()) {
		exit(EXIT_FAILURE);
	}
	return arr[front];
}

// Utility function to return the size of the queue
int Queue::size() {
	return count;
}

// Utility function to check if the queue is empty or not
bool Queue::isEmpty() {
	return (size() == 0);
}

// Utility function to check if the queue is full or not
bool Queue::isFull() {
	return (size() == capacity);
}

void sendToNextProcess(int* data);
int ringFunction(int workDone);
void doWork(int taskSize);
void specialPill();
bool isMaster(int rank);
void calculateEnd(auto start);

int main(int argc, char** argv) {
	auto start = std::chrono::system_clock::now();
	srand(time(NULL));

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	totalSize = size;
	int lastRank = size - 1;
	globalRank = rank;

	int* tasks;
	int totalJobs;
	int flag = 0;
	int* recvData;

	maxWork = (rand() + rank) % MAX_WORK + MIN_WORK;
	maxWork *= 2; // make sure that there is an even number of jobs to be done by each rank
	Queue queueObj (maxWork);

	if(isMaster(rank)) {
		printf("====== STARTING DATA ======\n");
		totalJobs = 0;
		for(int i = 1; i < size; i++) {
			int tempLargest = 0;
			MPI_Recv(&tempLargest, 1, MPI_INT, i, 0, MCW, MPI_STATUS_IGNORE);
			printf("Rank %d SHOULD run %d tasks\n", i, tempLargest);
			totalJobs += tempLargest;
		}
		printf("jobs to run %d\n", totalJobs);
		tasks = (int*)malloc(totalJobs * sizeof(int)); // make sure that you have enough work to at least exhaust every process
		for(int i = 0; i < totalJobs; i++) {
			tasks[i] = rand() % 1024 + 1;
		}
		printf("===========================\n");
	}
	else {
		MPI_Send(&maxWork, 1, MPI_INT, MASTER, 0, MCW);
	}
	// === === //

	MPI_Barrier(MCW);

	if(isMaster(rank)) {
		while(workDone < totalJobs) {
			int randomProc = rand() % lastRank + 1;
			MPI_Send(&tasks[workDone], WORK_TO_SEND, MPI_INT, randomProc, DATA_SENDS, MCW);
			workDone += WORK_TO_SEND;
		}
		ringFunction(0);
		while(procStatus != POISON) {
			MPI_Status status;
			int jobsCompleted;
			MPI_Recv(&jobsCompleted, 1, MPI_INT, 7, MPI_ANY_TAG, MCW, &status);
			int recvTag = status.MPI_TAG;
			int buffer = 2 * (size + 1);
			// cout << jobsCompleted << "/" << totalJobs << endl;
			if( (jobsCompleted + buffer) >= totalJobs) {
				// printf("I'm DONE");
				specialPill();
				procStatus = POISON;

				cout << "===========================" << endl;
				cout << "Tasks completed!" << endl;
				cout << "===========================" << endl;

				calculateEnd(start);
				MPI_Finalize();
				return 0;
			}
			else {
				ringFunction(0);
			}
		}
	}
	else {
		bool alreadyPrinted = false;
		int tag = NOT_POISON;
		recvData = (int*) malloc(WORK_TO_SEND * sizeof(int));
		while((workDone + WORK_TO_SEND) < maxWork && tag != POISON) {
			flag = 0;
			MPI_Status status;
			while (!flag) {
				MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, &flag, MPI_STATUS_IGNORE);
			}
			MPI_Recv(recvData, WORK_TO_SEND, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, &status);
			int recvTag = status.MPI_TAG;
			int recvSource = status.MPI_SOURCE;

			if(recvTag == DATA_SENDS) {
				for(int i = 0; i < WORK_TO_SEND; i++) {
					queueObj.enqueue(recvData[i]);
				}

				for(int i = 0; i < WORK_TO_SEND; i++) {
					int nextVal = queueObj.peek();
					queueObj.dequeue();
					doWork(nextVal);
				}

				workDone += WORK_TO_SEND;	
			}
			else {
				int singleData = recvData[0];
				if(singleData == POISON) {
					tag = POISON;
				} 
				else {
					int result = singleData + workDone;
					ringFunction(result);
				}
			}
		}
		while(tag != POISON) {
			if(!alreadyPrinted) {
				alreadyPrinted = !alreadyPrinted;
				cout << globalRank << " is done with " << workDone << " tasks" << endl;
			}
			flag = 0;
			MPI_Status status;
			while (!flag) {
				MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, &flag, MPI_STATUS_IGNORE);
			}
			int recvTag = status.MPI_TAG;
			int recvSource = status.MPI_SOURCE;
			if(recvTag != LIFE_SENDS) {
				MPI_Recv(recvData, WORK_TO_SEND, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, &status);
				sendToNextProcess(recvData);
			}
			else {
				int singleData = recvData[0];
				int newWork = workDone + singleData;
				if(singleData == POISON) {
					tag = POISON;
				}
				else {
					ringFunction(newWork);
				}
			}
			
		}
	}
	cout << rank << " is completely dead" << endl;

	// MPI_Barrier(MCW);
	if (isMaster(rank)) {
		calculateEnd(start);
	}

	MPI_Finalize();
	return 0;
}

void sendToNextProcess(int* data) {
	int newProc = (globalRank + 1) % totalSize;
	while(newProc == 0) {
		newProc++;
	}
	MPI_Send(data, WORK_TO_SEND, MPI_INT, newProc, DATA_SENDS, MCW);
}

int ringFunction(int workDone) {
	int newProc = (globalRank + 1) % totalSize;
	// cout <<"ring function: " << globalRank << " to " << newProc << endl;
	// MPI_Send(&workDone, 1, MPI_INT, newProc, LIFE_SENDS, MCW);
	MPI_Request req;
	MPI_Isend(&workDone, 1, MPI_INT, newProc, LIFE_SENDS, MCW, &req);
	
	return newProc;
}

void doWork(int taskSize) {
	int work = 0;
	int iterations = int(pow(double(taskSize), 2.0));
	for(int i = 0; i < iterations; i++) {
		work++;
	}
}

void specialPill() {
	for(int i = 1; i < totalSize; i++) {
		MPI_Send(&POISON, 1, MPI_INT, i, LIFE_SENDS, MCW);
	}
}

bool isMaster(int rank) {
	return (rank == MASTER);
}

void calculateEnd(auto start) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "Runtime: " << elapsed_seconds.count() << "s" << std::endl << std::endl << std::endl;
}
