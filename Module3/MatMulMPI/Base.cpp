#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <cstdlib>

// A, B and C are all N*N
// C = A * B
// initialize A and B with random values

// the matrix is a pointer to a pointer...
// where the first pointer is an array of pointers, which point to another array
// therefore building a 2d array
int** CreateEmptyMatrix(const int matrixSize)
{
    int** matrix = new int*[matrixSize];
    for (int row = 0; row < matrixSize; row++) { // fill down the rows first
        matrix[row] = new int[matrixSize];
        for (int col = 0; col < matrixSize; col++) { // then fill the cols
            matrix[row][col] = 0;
        }
    }

    return matrix; 
}

// create an matrix in the same way but fill in a random value
int** CreateRandomMatrix(const int matrixSize)
{
    int** matrix = new int*[matrixSize];
    for (int row = 0; row < matrixSize; row++) { // fill down the rows first
        matrix[row] = new int[matrixSize];
        for (int col = 0; col < matrixSize; col++) { // then fill the cols
            matrix[row][col] = rand() % 100 + 1; // 1 to 100
        }
    }

    return matrix; 
}

void OutputToTxt(int** matrix, const int matrixSize)
{
    // generate output.txt in a .csv style format
    std::ofstream output;
    output.open("output.txt");
    if (output.is_open()) {
        for (int i = 0; i < matrixSize; i++) {
            for (int j = 0; j < matrixSize; j++) {
                std::string numAsStr = std::to_string(matrix[i][j]);
                if (j == (matrixSize - 1)) { // if the last item of a row
                    numAsStr += "\n";
                } else {
                    numAsStr += ",";
                }
                output << numAsStr;
            }
        }
        output.close();
    }  
}

int main()
{
    srand((unsigned)time(0)); // seed the random number generator

    const int matrixSize = 8;

    // create the matrices
    int **a = CreateRandomMatrix(matrixSize);
    int **b = CreateRandomMatrix(matrixSize);
    int **c = CreateEmptyMatrix(matrixSize);

    // matrix multiplication
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            for (int k = 0; k < matrixSize; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    OutputToTxt(c, matrixSize);

    return 0;
}

// g++ Base.cpp -o test && ./test