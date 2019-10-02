#include <iostream>
#include <stdio.h>
#include <chrono>
#include <ctime>
#include <string>
#include <complex>
#include <fstream>

#define MCW MPI_COMM_WORLD

using namespace std;

struct Color
{
	int red;
	int green;
	int blue;
};

int width = 512;
int height = 512;

Color generateValue(complex<double> complex);

int main(int argc, char** argv) {
	auto start = std::chrono::system_clock::now();

	if (argc < 4) {
		printf("\nNot enough arguments. Requires 3 doubles as arguments\n");
		return -1;
	}
	double r0 = atof(argv[1]);
	double i0 = atof(argv[2]);
	double r1 = atof(argv[3]);
	double i1 = i0 + (r1 - r0);

	// check to see if you can even open the ppm file
	ofstream mandel("brot.ppm");
	if (!mandel.is_open()) {
		printf("you failed, we'll get 'em next time\n\n");
		return -1;
	}

	// fill in the file
	mandel << "P3\n" << width << " " << height << " 255\n"; // this particular line is for the header
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			std::complex<double> complex(
				r0 + (j * (r1 - r0) / height),
				i0 + (i * (i1 - i0) / width)
			);
			Color val = generateValue(complex);
			mandel << val.red << " " << val.blue << " " << val.green << "\n";
		}
		mandel << "\n";
	}
	mandel.close();

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

	return 0;
}

Color generateValue(complex<double> complex) {

	std::complex<double> point((float)complex.real() / width - 1.5, (float)complex.imag() / height - 0.5);
	std::complex<double> curr(0, 0);

	int iter = 0; //mandelbrot require that a set remain bounded (by 2 here) after any number of iterations
	const int MAX_ITERS = 30;
	while (abs(curr) < 2 && iter < MAX_ITERS) {
		curr = curr * curr + point;
		iter++;
	}

	Color color;

	int r = (255 * iter) / 12;
	int g = (255 * iter) / 21;
	int b = (255 * iter) / 12;
	if (iter < 10) {
		color.blue = 0;
		color.green = 0;
		color.red = 0;
	} 
	else {
		color.blue = r;
		color.green = g;
		color.red = b;
	} 

	return color;
}