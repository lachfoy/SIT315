#include <cstdio>
#include <cstdlib> // malloc, rand, srand
#include <time.h> // time
#include <chrono>
#include <mpi.h>

using namespace std::chrono;

void RandomVector(int *vector, int size)
{
    for (int i = 0; i < size; i++)
    {
        // give each index a random number between 1 and 100
        vector[i] = rand() % 100 + 1;
    }
}

int main(int argc, char** argv)
{
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // init rand
    srand((unsigned)time(0));

    unsigned long size = 10000000;
    printf("test program with size = %lu\n", size);

    // dynamically allocate memory for vectors
    int *v1 = new int[size];
    int *v2 = new int[size];
    int *v3 = new int[size];

    // assign random values to v1 and v2
    RandomVector(v1, size);
    RandomVector(v2, size);

    // start the timer
    auto start = high_resolution_clock::now();

    // Get the rank
    int rank;
    const int root = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // get the partition size by dividing the size by the number of tasks
    const int partition_size = size / rank;

    // scatter each array into temporary storage    
    int *tmp1 = new int[partition_size];
    int *tmp2 = new int[partition_size];
    MPI_Scatter(v1, partition_size, MPI_INT, tmp1, partition_size, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Scatter(v2, partition_size, MPI_INT, tmp2, partition_size, MPI_INT, root, MPI_COMM_WORLD);

    // add tmp1(v1) and tmp2(v2) and store the result in tmp1
    for (int i = 0; i < partition_size; i++)
    {
        tmp1[i] += tmp2[i];
    }

    // redistribute the resulting sums into v3
    MPI_Gather(v3, partition_size, MPI_INT, tmp1, partition_size, MPI_INT, root, MPI_COMM_WORLD);

    // stop the timer
    auto stop = high_resolution_clock::now();

    // calculate time taken by addition
    auto duration = duration_cast<microseconds>(stop - start);
    printf("time taken by add: %li microseconds\n", duration.count());

    // clean up our memory
    delete[] v1, v2, v3, tmp1, tmp2;

    // finalize the MPI environment
    MPI_Finalize();
} // mpic++ add_mpi.cpp -o add_mpi && mpirun -np 4 -hostfile ./cluster ./add_mpi