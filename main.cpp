#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <cstdint>
#include <omp.h>



int main(int argc, char* argv[]) {

	int64_t N = 1000;
	if (argc > 1) N = std::stoi(std::string(argv[1]));
	int i, j, k;

	// Initialize Matrices
	float **A, **B, **C;
	A = new float* [N];
	B = new float* [N];
	C = new float* [N];

	for (i = 0; i < N; i++) {
		A[i] = new float[N];
		B[i] = new float[N];
		C[i] = new float[N];
	}

	// Fill with random numbers
	std::srand(static_cast<unsigned int>(omp_get_wtime()));
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			A[i][j] = static_cast<float>(std::rand());
			B[i][j] = static_cast<float>(std::rand());
			C[i][j] = 0;
		}
	}

	double t0 = omp_get_wtime();

	//Multiply A * B
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			for (k = 0; k < N; k++) {
				C[i][j] += (A[i][k] * B[k][j]);
			}
		}
	}

	double t1 = omp_get_wtime();

	int64_t flops = N * N * N;
	std::cout << "Time: " << (t1 - t0) << std::endl;
	std::cout << flops / (1000000000 * (t1 - t0)) << "Gflops\n";


	std::ofstream f;
	f.open("Matrix_output.txt");
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			f << C[i][j] << ' ';
		}
		f << std::endl;
	}
	f.close();






	return 0;
}