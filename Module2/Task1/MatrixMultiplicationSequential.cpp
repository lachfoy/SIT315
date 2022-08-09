#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <chrono>
#include <cstdlib>

// A, B and C are all N*N
// C = A * B
// initialize A and B with random values

#define MATRIX_SIZE 512
#define NUM_TESTS 10

// the matrix is a pointer to a pointer...
// where the first pointer is an array of pointers, which point to another array
// therefore building a 2d array
int** createEmptyMatrix()
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
int** createRandomMatrix()
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
    srand((unsigned)time(nullptr)); // seed the random number generator

    // create the matrices
    int **a = createRandomMatrix();
    int **b = createRandomMatrix();
    int **c = createEmptyMatrix();

    // get current time
    auto start = std::chrono::high_resolution_clock::now();

    // matrix multiplication
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
                std::string numAsStr = std::to_string(c[i][j]);
                if (j == (MATRIX_SIZE - 1)) // if the last item of a row
                {
                    numAsStr += "\n";
                }
                else
                {
                    numAsStr += ",";
                }

                output << numAsStr;
            }
        }

        output.close();
    }

    std::cout << "Matrix multiplication of random matrix size " << MATRIX_SIZE << "x" << MATRIX_SIZE << "\n"
        << "Time taken: " << duration.count() << " microseconds\n";

    return 0;
}

// g++ MatrixMultiplicationSequential.cpp -o MatrixMultiplicationSequential && ./MatrixMultiplicationSequential