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

// === Generate Maps === //
int* genHeightMap();
int* genDirtMap();
int* genCaveMap();
int* genCaveMap();
int* genOreMap();
int* genLiquidMap();
int* genGrassMap();
// === Compare Maps === //
void initMap();
void compareHeightMap(int* base, int* next);
void compareDirtMap(int* base, int* next);
void compareCaveMap(int* base, int* next);
void compareOreMap(int* base, int* next);
void compareLiquidMap(int* base, int* next);
void compareGrassMap(int* base, int* next);
// === Map Utilities === //
int returnOreType();
void cleanCaveRes(int* base);
// === General Utilities === //
int* steps(int choice);
bool isMaster(int rank);
void printArr(int* arr, int numRows);
void printArr(float* arr, int numRows);
string returnString(int input);
int generateNumRowsToSend(int rank, int size, int perProc);
int startingIndexAdjustment(int rank, int size, int perProc);
void comparisons(int choice, int* base, int* next);
void printToFile(int* arr, string prefix);
void calculateEnd(auto start);

//==== CONSTANTS ====//
const int STEPS = 7;
const int MASTER = 0;
const int WIDTH = 256;
const int HEIGHT = 256;
const int AREA = HEIGHT * WIDTH;

const int SKYLIMIT = HEIGHT / 4;
const int HEIGHT_LIMIT = SKYLIMIT - (HEIGHT / 16);
const int LAVA_LIMIT = HEIGHT - SKYLIMIT;
const int WATER_LIMIT = SKYLIMIT + (SKYLIMIT / 2);

const int STONE = 0;
const int DIRT = 1;
const int SKY = 2;
const int CLOUD = 3;
const int WATER = 4;
const int SAND = 5;
const int GRASS = 6;
const int ICE = 7;
const int LAVA = 8;
const int CAVE = 9;
const int WOOD = 10;
const int LIGHT_CAVE = 11;

const int COAL = 99;
const int IRON = 98;
const int GOLD = 97;
const int REDSTONE = 96;
const int DIAMOND = 95;
const int EMERALD = 94;

// === PERLIN TUPLE === //
class PerlinTuple {
public:
	int octave;
	int max;
	PerlinTuple(int pOctave, int pMax) {
		octave = pOctave;
		max = pMax;
	}
};

// === PERLIN TUPLE CONSTS === //
const PerlinTuple HEIGHT_MAP_TUPLE = PerlinTuple(7, 2);
const PerlinTuple DIRT_MAP_TUPLE = PerlinTuple(5, 3);
const PerlinTuple CAVE_MAP_TUPLE = PerlinTuple(3, 3);
const PerlinTuple ORE_MAP_TUPLE = PerlinTuple(4, 3);
const PerlinTuple LIQUID_MAP_TUPLE = PerlinTuple(4, 2);

//==== PERLIN NOISE ====//
class PerlinNoise {
public:
	int octive;
	float* noiseSeed = nullptr;
	float* perlinNoise = nullptr;
	int* returnNoise = nullptr;
	int modSize;
	PerlinNoise(int pOctive, int passSize) {
		octive = pOctive;
		modSize = passSize;
		noiseSeed = (float*)malloc(AREA * sizeof(float));
		for (int i = 0; i < AREA; i++) {
			noiseSeed[i] = (float)rand() / (float)RAND_MAX;
		}
		perlinNoise = (float*)malloc(AREA * sizeof(float));
		returnNoise = (int*)malloc(AREA * sizeof(int));
		PerlinNoise2D(WIDTH, HEIGHT, noiseSeed, octive, fScalingBias, perlinNoise);
		adjustFloatToInt(WIDTH, HEIGHT, perlinNoise, returnNoise);
	}
	~PerlinNoise() {
		delete noiseSeed;
		delete perlinNoise;
	};
private:
	float fScalingBias = 2.0f;
	void PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput) {
		for (int x = 0; x < nWidth; x++)
			for (int y = 0; y < nHeight; y++)
			{
				float fNoise = 0.0f;
				float fScaleAcc = 0.0f;
				float fScale = 1.0f;

				for (int o = 0; o < nOctaves; o++)
				{
					int nPitch = nWidth >> o;
					int nSampleX1 = (x / nPitch) * nPitch;
					int nSampleY1 = (y / nPitch) * nPitch;

					int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
					int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

					float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
					float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

					float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
					float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

					fScaleAcc += fScale;
					fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
					fScale = fScale / fBias;
				}
				float out = (fNoise / fScaleAcc);
				fOutput[y * nWidth + x] = out;
			}
	}

	void adjustFloatToInt(int nWidth, int nHeight, float* arr, int* intArr) {
		for (int x = 0; x < nWidth; x++) {
			for (int y = 0; y < nHeight; y++) {
				short bg_col, fg_col;
				wchar_t sym;
				int pixel_bw = (int)(arr[y * nWidth + x] * 12.0f);
				intArr[y * nWidth + x] = (pixel_bw % modSize);
			}
		}
	}
};

bool isMaster(int rank) {
	return (rank == MASTER);
}

void initMap(int* dataArr) {
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			dataArr[(i * WIDTH) + j] = STONE;
			if (i < SKYLIMIT) {
				dataArr[(i * WIDTH) + j] = SKY;
			}
		}
	}
}

int* steps(int choice) {
	switch (choice) {
	case 1:
		return genHeightMap();
		break;
	case 2:
		return genDirtMap();
		break;
	case 3:
		return genCaveMap();
		break;
	case 4:
		return genOreMap();
		break;
	case 5:
		return genLiquidMap();
	case 6:
		return genGrassMap();
	default:
		return NULL;
	}
}

void comparisons(int choice, int* base, int* next) {
	switch (choice) {
		case 1:
			compareHeightMap(base, next);
			break;
		case 2:
			compareDirtMap(base, next);
			break;
		case 3:
			compareCaveMap(base, next);
			break;
		case 4:
			compareOreMap(base, next);
			break;
		case 5:
			compareLiquidMap(base, next);
			break;
		case 6:
			compareGrassMap(base, next);
			break;
		default:
			break;
	}
}

void compareHeightMap(int* base, int* next) {
	int tolerance = HEIGHT_MAP_TUPLE.max - 1;

	for (int i = SKYLIMIT; i >= (SKYLIMIT - HEIGHT_LIMIT); i--) {
		for (int j = 0; j < WIDTH; j++) {
			int idx = i * WIDTH + j;
			int nextVal = next[idx];
			int valUnder = base[idx + WIDTH];
			if (valUnder == STONE && nextVal == tolerance) {
				base[idx] = STONE;
			}
		}
	}
	printToFile(base, "1_heightMap");
}

void compareDirtMap(int* base, int* next) {

	int dirt = DIRT_MAP_TUPLE.max - 1;
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			int idx = i * WIDTH + j;
			if (base[idx] == STONE && next[idx] == dirt) {
				base[idx] = DIRT;
			}
		}
	}

	printToFile(base, "2_dirtMap");
}

void compareCaveMap(int* base, int* next) {
	for (int i = 1; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH - 1; j++) {
			int idx = i * HEIGHT + j;
			if ((base[idx] == DIRT || base[idx] == STONE) && next[idx] == 0) {
				base[idx] = CAVE;
			}
		}
	}
	
	cleanCaveRes(base);
	printToFile(base, "3_caveMap");
}

int returnOreType() {
	int ore = rand() % 100 + 1;
	if(ore < 90) {
		return COAL;
	}
	else if(ore < 97) {
		return IRON;
	}
	else {
		return DIAMOND;
	}
}

void compareOreMap(int* base, int* next) {
	for (int i = 1; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH - 1; j++) {
			int idx = i * HEIGHT + j;
			bool ore = (next[idx] % 2 == 0);
			if (next[idx] % 3 == 0 && rand() % 7 == 0 && base[idx] == STONE) {
				base[idx] = returnOreType();
			}
		}
	}

	printToFile(base, "4_oreMap");
}

void compareLiquidMap(int* base, int* next) {
	for(int i = 1; i < WATER_LIMIT; i++) {
		for(int j = 1; j < WIDTH; j++) {
			int idx = i * HEIGHT + j;
			int above = idx - WIDTH;
			int below = idx + WIDTH;
			bool water = (rand() % 10) == 1;
			if((base[idx] == STONE || base[idx] == DIRT) && (base[above] == SKY || base[above] == CAVE) && next[idx] == 1 && water) {
				base[idx] = WATER;
				base[below] = SAND;
				int oneOver = idx + 1;
				int twoOver = oneOver + 1;
				if(idx % WIDTH != 0 && base[oneOver] != SKY) {
					base[oneOver] = WATER;
					base[oneOver + WIDTH] = SAND;
					if(oneOver % WIDTH != 0 && base[oneOver] != SKY) {
						base[twoOver] = WATER;
						base[twoOver + WIDTH] = SAND; 
					}
				}
			}

		}
	}

	for(int i = LAVA_LIMIT; i < HEIGHT - 1; i++) {
		for(int j = 1; j < WIDTH - 1; j++) {
			int idx = i * HEIGHT + j;
			int lava = (rand() % 100) == 0;
			if(lava && (base[idx] == STONE || base[idx] == DIRT)) {
				base[idx] = LAVA;
				int oneOver = idx + 1;
				int twoOver = oneOver + 1;
				int threeOver = twoOver + 1;
				if(idx % WIDTH != 0 && base[oneOver] != CAVE) {
					base[oneOver] = LAVA;
					if(oneOver % WIDTH != 0 && base[oneOver] != CAVE) {
						base[twoOver] = LAVA;
						if(twoOver % WIDTH != 0 && base[twoOver] != CAVE) {
							base[threeOver] = LAVA;
						}
					}
				}
			}
		}
	}

	printToFile(base, "5_liquidMap");
}

void cleanCaveRes(int* base) {
	for(int i = 1; i < HEIGHT; i++) {
		for(int j = 0; j < WIDTH; j++) {
			int idx = i * WIDTH + j;
			int above = idx - WIDTH;
			if(base[above] == SKY && base[idx] == CAVE) {
				base[idx] = SKY;
			}
		}
	}
}

void compareGrassMap(int* base, int* next) {
	for (int i = SKYLIMIT; i >= 0; i--) {
		for (int j = 0; j < WIDTH; j++) {
			int idx = i * WIDTH + j;
			// if (base[idx] == STONE && base[idx - WIDTH] == SKY && next[idx] > 0) {
			if (base[idx] == STONE && base[idx - WIDTH] == SKY) {
				base[idx] = GRASS;
				base[idx + WIDTH] = DIRT;
				base[idx + WIDTH + WIDTH] = DIRT;
			}
		}
	}
	printToFile(base, "6_grassMap");
}

int* genCaveMap() {
	PerlinNoise pn(CAVE_MAP_TUPLE.octave, CAVE_MAP_TUPLE.max);
	return pn.returnNoise;
}

int* genDirtMap() {
	PerlinNoise pn(DIRT_MAP_TUPLE.octave, DIRT_MAP_TUPLE.max);
	return pn.returnNoise;
}

int* genHeightMap() {
	PerlinNoise pn(HEIGHT_MAP_TUPLE.octave, HEIGHT_MAP_TUPLE.max);
	return pn.returnNoise;
}

int* genOreMap() {
	PerlinNoise pn(ORE_MAP_TUPLE.octave, ORE_MAP_TUPLE.max);
	return pn.returnNoise;
}

int* genLiquidMap() {
	PerlinNoise pn(LIQUID_MAP_TUPLE.octave, LIQUID_MAP_TUPLE.max);
	return pn.returnNoise;
}

int* genGrassMap() {
	PerlinNoise pn(3, 2);
	return pn.returnNoise;
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
	string cave = "10 10 10";
	string wood = "153 153 0";
	string coal = "48 48 88";
	string iron = "255 204 153";
	string gold = "245 245 90";
	string redstone = "255 0 0";
	string diamond = "0 153 153";
	string emerald = "155 203 155";


	if (input == STONE) {
		return stone;
	}
	else if (input == DIRT) {
		return dirt;
	}
	else if (input == SKY) {
		return sky;
	}
	else if (input == CLOUD) {
		return cloud;
	}
	else if (input == WATER) {
		return water;
	}
	else if (input == SAND) {
		return sand;
	}
	else if (input == GRASS) {
		return grass;
	}
	else if (input == ICE) {
		return ice;
	}
	else if (input == LAVA) {
		return lava;
	}
	else if (input == CAVE) {
		return cave;
	}
	else if (input == WOOD) {
		return wood;
	}
	else if(input == COAL) {
		return coal;
	} 
	else if(input == IRON) {
		return iron;
	}
	else if(input == GOLD) {
		return gold;
	}
	else if(input == REDSTONE) {
		return redstone;
	}
	else if(input == DIAMOND) {
		return diamond;
	}
	else if(input == EMERALD) {
		return emerald;
	}
	else {
		return null;
	}
}

int generateNumRowsToSend(int rank, int size, int perProc) {
	return (isMaster(rank) || rank == (size - 1)) ? (perProc + 1) : (perProc + 2);
}

int startingIndexAdjustment(int rank, int size, int perProc) {
	if (isMaster(rank)) {
		return 0;
	}
	else {
		// returning the following minus the WIDTH
		// allows for the starting value to be one
		// row less than it normally would be
		return ((rank * perProc * WIDTH) - WIDTH);
	}
}

void printToFile(int* arr, string prefix) {
	string newFile = prefix + ".ppm";
	ofstream of(newFile);

	if (of.is_open()) {
		of << "P3\n" << WIDTH << " " << HEIGHT << " 255\n";
		for (int i = 0; i < AREA; i++) {
			string val = returnString(arr[i]);
			of << val << " ";
			if (i != 0 && (i + 1) % HEIGHT == 0) {
				of << "\n";
			}
		}
	}
	of.close();
	cout << "Printed to " << newFile << endl;
}

void printArr(int* arr, int numRows) {
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < WIDTH; j++) {
			printf("%d ", arr[(i * WIDTH) + j]);
		}
		printf("\n");
	}
	printf("\n\n\n");
}

void printArr(float* arr, int numRows) {
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < WIDTH; j++) {
			printf("%d ", int(arr[(i * WIDTH) + j]));
		}
		printf("\n");
	}
	printf("\n\n\n");
}

void calculateEnd(auto start) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "Runtime: " << elapsed_seconds.count() << "s" << std::endl << std::endl << std::endl;
}

int main(int argc, char** argv) {
	srand(time(NULL));
	auto start = std::chrono::system_clock::now();
	auto prevTime = start;

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);
	int* dataArr;
	int* recvArr;

	int numRows = HEIGHT / size;
	int perProc = AREA / size;

	if (isMaster(rank)) {
		dataArr = (int*)malloc(AREA * sizeof(int));
		recvArr = (int*)malloc(AREA * sizeof(int));
		int nextStep;
		initMap(dataArr);
		printToFile(dataArr, "0_init");
		for (int i = 1; i < STEPS; i++) {
			int sendProc = i % size;
			if (sendProc == 0) { // don't send to yourself
				sendProc++;
			}
			nextStep = i;
			MPI_Send(&nextStep, 1, MPI_INT, sendProc, 0, MCW);
		}
		for (int i = 1; i < STEPS; i++) {
			int recvProc = i % size;
			if (recvProc == 0) { // don't receive from yourself
				recvProc++;
			}
			MPI_Recv(recvArr, AREA, MPI_INT, recvProc, 0, MCW, MPI_STATUS_IGNORE);
			comparisons(i, dataArr, recvArr);
		}
	}
	else if (rank < STEPS) {
		int step;
		MPI_Recv(&step, 1, MPI_INT, 0, 0, MCW, MPI_STATUS_IGNORE);
		MPI_Send(steps(step), AREA, MPI_INT, 0, 0, MCW);
	}


	MPI_Barrier(MCW);
	if (isMaster(rank)) {
		calculateEnd(start);
	}

	MPI_Finalize();
	return 0;
}
