#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <cstdint>
#include <omp.h>



// Write a matrix to a file
void print_matrix(float** A, size_t N, std::string filename = "matrix_output.txt") {
	std::ofstream f;
	f.open(filename);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			f << A[i][j] << ' ';
		}
		f << std::endl;
	}
	f.close();
}

// multithreaded lower triangular matrix multiplication
inline void matmul(float** A, float** B, float** C, size_t N) {
	//int num_threads = 1;
	#pragma omp parallel
	{
		//num_threads = omp_get_num_threads();
		#pragma omp for schedule(dynamic, 1)
		for (size_t i = 0; i < N; i++) {
			for (size_t j = 0; j < N; j++) {
				C[i][j] = 0.0;
				for (size_t k = 0; k < N; k++) {
					C[i][j] += (A[i][k] * B[k][j]);
				}
			}
		}
	}

}

// version for double precision arithmetic
void matmul(double** A, double** B, double** C, size_t N) {
	//int num_threads = 1;
	#pragma omp parallel
	{
		//num_threads = omp_get_num_threads();
		#pragma omp for schedule(dynamic, 1)
		for (size_t i = 0; i < N; i++) {
			for (size_t j = 0; j < N; j++) {
				C[i][j] = 0.0;
				for (size_t k = 0; k < N; k++) {
					C[i][j] += (A[i][k] * B[k][j]);
				}
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
				A[i][j] = static_cast<float>(std::rand()) / RAND_MAX;
				B[i][j] = static_cast<float>(std::rand()) / RAND_MAX;
				C[i][j] = 0;
			}
		}

		double t0 = omp_get_wtime();
		matmul(A, B, C, N);
		double t1 = omp_get_wtime();
		output += (t1 - t0);
		trial++;
	}

	//print_matrix(A, N, "A.txt");
	//print_matrix(B, N, "B.txt");
	//print_matrix(C, N, "C.txt");

	for (i = 0; i < N; i++) {
		delete[] A[i];
		delete[] B[i];
		delete[] C[i];
	}

	return output;
}

int main(int argc, char* argv[]) {

	int64_t upper_limit = 1000;
	int trials_per_size = 4;
	int step = 200;

	if (argc > 1) {	
		upper_limit = std::stoi(std::string(argv[1]));
	}
	if ( argc > 2 ) {
		step = std::stoi( std::string(argv[2]) );
	}	
	if ( argc > 3 ) {
		trials_per_size = std::stoi( std::string(argv[3]) );
	}
	
	std::cout << "Running " << trials_per_size << " multiplications at each size \n";

	for (int64_t i = step; i <= upper_limit; i += step) {
		double dt = run(i, trials_per_size);
		int64_t flops = (2 * i - 1) * i * i * trials_per_size;
		std::cout << "Array Size: " << i << " x " << i << std::endl;
		std::cout << "Time: " << dt << std::endl;
		std::cout << flops / (1000000000 * dt) << " Gflops\n";
		std::cout << "----------\n";
	}


	return 0;
}