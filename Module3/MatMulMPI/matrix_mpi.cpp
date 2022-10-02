#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <cstdlib>

#include <mpi.h> // "/usr/lib/mpich/include"

// A, B and C are all N * N
// C = A * B
// initialize A and B with random values

void FillZeroMatrix(int* matrix, const int matrixSize)
{
    for (int i = 0; i < matrixSize * matrixSize; i++) {
        matrix[i] = 0;
    }
}

void FillRandomMatrix(int* matrix, const int matrixSize)
{
    for (int i = 0; i < matrixSize * matrixSize; i++) {
        matrix[i] = rand() % 10 + 1; // 1 to 10
    }
}

void PrintMatrix(int* matrix, const int matrixSize)
{
    // print matrix in a .csv style format
    std::string output;
    for (int i = 0; i < matrixSize * matrixSize; i++) {
        std::string numAsStr = std::to_string(matrix[i]);
        if ((i + 1) % matrixSize == 0) {
            numAsStr += "\n";
        } else {
            numAsStr += ",";
        }
        output += numAsStr;
    }

    std::cout << output << std::endl;
}

void OutputToTxt(int* matrix, const int matrixSize)
{
    // generate output.txt in a .csv style format
    std::ofstream output;
    output.open("output.txt");
    if (output.is_open()) {
        for (int i = 0; i < matrixSize * matrixSize; i++) {
            std::string numAsStr = std::to_string(matrix[i]);
            if ((i + 1) % matrixSize == 0) {
                numAsStr += "\n";
            } else {
                numAsStr += ",";
            }
            output << numAsStr;
        }
        output.close();
    }  
}

int main(int argc, char** argv)
{
    srand((unsigned)time(0)); // seed the random number generator

    // initialize the MPI environment
    MPI_Init(&argc, &argv);

    // get the rank and size
    int rank, size;
    const int root = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int matrixSize = 512;

    // create the matrices
    int *a = new int[matrixSize * matrixSize];
    int *b = new int[matrixSize * matrixSize];
    int *c = new int[matrixSize * matrixSize];

    if (rank == root) {
        FillRandomMatrix(a, matrixSize);
        FillRandomMatrix(b, matrixSize);
    }

    const int partitionSize = matrixSize * matrixSize / size;
    
    // scatter rows of matrix a to all workers into tmp1 (worker for each row)
    int *tmp1 = new int[partitionSize];
    MPI_Scatter(a, partitionSize, MPI_INT, tmp1, partitionSize, MPI_INT, root, MPI_COMM_WORLD);

    // copy matrix b to each worker
    // necessary because only the root node randomizes the matrices
    MPI_Bcast(b, matrixSize * matrixSize, MPI_INT, root, MPI_COMM_WORLD);

    // matrix multiplication by row, store the result in tmp2
    int *tmp2 = new int[partitionSize];
    for (int i = 0; i < partitionSize; i++) {
        int sum = 0;
        for (int j = 0; j < matrixSize; j++) {
            sum += tmp1[j] * b[i + (j * matrixSize)];
        }
        tmp2[i] = sum;
    }

    // gather the result back into c
    MPI_Gather(tmp2, partitionSize, MPI_INT, c, partitionSize, MPI_INT, root, MPI_COMM_WORLD);

    // output the final matrix to output.txt
    if (rank == root) {
        OutputToTxt(c, matrixSize);
    }

    // exit mpi
    MPI_Finalize();

    // deallocate memory
    delete[] a, b, c, tmp1, tmp2;

    return 0;
}

// mpic++ matrix_mpi.cpp -o matrix_mpi && mpirun -np 4 -hostfile ./cluster ./matrix_mpi