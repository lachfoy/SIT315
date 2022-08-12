#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <chrono>
#include <cstdlib>
#include <pthread.h>

// A, B and C are all N*N
// C = A * B
// initialize A and B with random values

#define MATRIX_SIZE 512 // MUST BE A MULTIPLE OF 2
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
            matrix[row][col] = rand() % 10 + 1; // 1 to 100
        }
    }

    return matrix; 
}

struct MultiplyTask
{
    int startRow, endRow, startCol, endCol;
    int **a;
    int **b;
    int **c;
};

void* multiplyMatrix(void* args)
{
    MultiplyTask *multiplyTask = (MultiplyTask*)args;

    for (int i = multiplyTask->startRow; i < multiplyTask->endRow; i++)
    {
        for (int j = multiplyTask->startCol; j < multiplyTask->endCol; j++)
        {
            for (int k = 0; k < MATRIX_SIZE / 2; k++)
            {
                multiplyTask->c[i][j] += multiplyTask->a[i][k] * multiplyTask->b[k][j];
            }
        }
    }

    return nullptr;
}

int main()
{
    srand((unsigned)time(0)); // seed the random number generator

    // create the matrices
    int **a = createRandomMatrix();
    int **b = createRandomMatrix();
    int **c = createEmptyMatrix();

    // get current time
    auto start = std::chrono::high_resolution_clock::now();

    // matrix multiplication
    MultiplyTask *tasks[4];

    // allocate memory for each task
    for (int i = 0; i < 4; i++)
    {
        tasks[i] = (MultiplyTask*)malloc(sizeof(MultiplyTask));
        tasks[i]->a = a;
        tasks[i]->b = b;
        tasks[i]->c = c;
    }

    // top left quadrant
    tasks[0]->startRow = 0;
    tasks[0]->endRow = MATRIX_SIZE / 2;
    tasks[0]->startCol = 0;
    tasks[0]->endCol = MATRIX_SIZE / 2;

    // top right quadrant
    tasks[1]->startRow = 0;
    tasks[1]->endRow = MATRIX_SIZE / 2;
    tasks[1]->startCol = MATRIX_SIZE / 2;
    tasks[1]->endCol = MATRIX_SIZE;

    // bottom left quadrant
    tasks[2]->startRow = MATRIX_SIZE / 2;
    tasks[2]->endRow = MATRIX_SIZE;
    tasks[2]->startCol = 0;
    tasks[2]->endCol = MATRIX_SIZE / 2;

    // bottom right quadrant
    tasks[3]->startRow = MATRIX_SIZE / 2;
    tasks[3]->endRow = MATRIX_SIZE;
    tasks[3]->startCol = MATRIX_SIZE / 2;
    tasks[3]->endCol = MATRIX_SIZE;

    // create a thread for each quadrant
    pthread_t threads[4];
    for (int i = 0; i < 4; i++)
    {
        pthread_create(&threads[i], NULL, multiplyMatrix, (void*)tasks[i]);
    }

    // join the threads when they are done with their work
    for (int i = 0; i < 4; i++)
    {
        pthread_join(threads[i], NULL);
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

// g++ MatrixMultiplicationParallel.cpp -o MatrixMultiplicationParallel -lpthread && ./MatrixMultiplicationParallel