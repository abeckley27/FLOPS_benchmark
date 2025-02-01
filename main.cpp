#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <cstdint>
#include <omp.h>

// multithreaded lower triangular matrix multiplication - not being used yet
void matmul(const float* B, const float* C, float* A, size_t N) {
	int num_threads = 1;
	#pragma omp parallel
	{
		num_threads = omp_get_num_threads();
		#pragma omp for schedule(static, 2)
		for (size_t i = 0; i < N; i++) {
			size_t ii = i * (i+1) / 2;
			for (size_t j = 0; j <= i; j++) {
				double sum = 0.0;
				for (size_t k = j; k <= i; k++) {
					size_t kk = k * (k+1) / 2;
					sum += B[ii + k] * C[kk + j];
				}
				A[ii + j] = sum;
			}
		}
	}
}

// Initializes and multiplies N x N matrices. Returns the amount of time taken.
inline double run(int64_t N, int num_trials) {
	int i, j, k, trial;

	// Initialize Matrices
	float** A, ** B, ** C;
	A = new float* [N];
	B = new float* [N];
	C = new float* [N];

	for (i = 0; i < N; i++) {
		A[i] = new float[N];
		B[i] = new float[N];
		C[i] = new float[N];
	}

	trial = 0;
	double output = 0.0;

	while (trial < num_trials) {

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
		output += (t1 - t0);
		trial++;
	}

	for (i = 0; i < N; i++) {
		delete[] A[i];
		delete[] B[i];
		delete[] C[i];
	}

	return output;
}

int main(int argc, char* argv[]) {

	int64_t N = 1000;
	int trials_per_size = 10;
	int step = 200;

	if (argc > 1) {	
		N = std::stoi(std::string(argv[1]));
		if ( argc > 2 ) {
			step = std::stoi( std::string(argv[2]) );
		}
	}
	

	for (int64_t i = step; i < N; i += step) {
		double dt = run(i, trials_per_size);
		int64_t flops = (2 * i - 1) * i * i * trials_per_size;
		std::cout << "Array Size: " << i << std::endl;
		std::cout << "Time: " << dt << std::endl;
		std::cout << flops / (1000000000 * dt) << " Gflops\n";
		std::cout << "----------\n";
	}


/*
	Code for writing a matrix to a file

	std::ofstream f;
	f.open("Matrix_output.txt");
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			f << C[i][j] << ' ';
		}
		f << std::endl;
	}
	f.close();
*/


	return 0;
}