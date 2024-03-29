#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <chrono>
#include <cstdlib>
#include <omp.h>

// A, B and C are all N*N
// C = A * B
// initialize A and B with random values

#define MATRIX_SIZE 512
#define NUM_TESTS 10

// the matrix is a pointer to a pointer...
// where the first pointer is an array of pointers, which point to another array
// therefore building a 2d array
int** CreateEmptyMatrix()
{
    int** matrix = new int*[MATRIX_SIZE];
    for (int row = 0; row < MATRIX_SIZE; row++) // fill down the rows first
    {
        matrix[row] = new int[MATRIX_SIZE];
        for (int col = 0; col < MATRIX_SIZE; col++) // then fill the cols
        {
            matrix[row][col] = 0;
        }
    }

    return matrix; 
}

// create an matrix in the same way but fill in a random value
int** CreateRandomMatrix()
{
    int** matrix = new int*[MATRIX_SIZE];
    for (int row = 0; row < MATRIX_SIZE; row++) // fill down the rows first
    {
        matrix[row] = new int[MATRIX_SIZE];
        for (int col = 0; col < MATRIX_SIZE; col++) // then fill the cols
        {
            matrix[row][col] = rand() % 100 + 1; // 1 to 100
        }
    }

    return matrix; 
}


int main()
{
    omp_set_num_threads(4);

    srand((unsigned)time(0)); // seed the random number generator

    // create the matrices
    int **a = CreateRandomMatrix();
    int **b = CreateRandomMatrix();
    int **c = CreateEmptyMatrix();

    // get current time
    auto start = std::chrono::high_resolution_clock::now();

    // matrix multiplication
    #pragma omp parallel for collapse(3)
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            for (int j = 0; j < MATRIX_SIZE; j++)
            {
                for (int k = 0; k < MATRIX_SIZE; k++)
                {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }

    // get current time
    auto stop = std::chrono::high_resolution_clock::now();

    // calculate duration
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    // generate output.txt in a .csv style format
    std::ofstream output;
    output.open("output.txt");
    if (output.is_open())
    {
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            for (int j = 0; j < MATRIX_SIZE; j++)
            {
                std::string num_as_str = std::to_string(c[i][j]);
                if (j == (MATRIX_SIZE - 1)) // if the last item of a row
                {
                    num_as_str += "\n";
                }
                else
                {
                    num_as_str += ",";
                }

                output << num_as_str;
            }
        }

        output.close();
    }

    std::cout << "Matrix multiplication of random matrix size " << MATRIX_SIZE << "x" << MATRIX_SIZE << "\n"
        << "Time taken: " << duration.count() << " microseconds\n";

    return 0;
}

// g++ matmul_omp.cpp -o matmul_omp -fopenmp && ./matmul_omp