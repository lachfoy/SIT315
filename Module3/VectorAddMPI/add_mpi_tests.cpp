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
    // init rand
    srand((unsigned)time(0));

    unsigned long size = 1000;
    printf("size = %lu\n", size);

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    const int num_tests = 10;
    for (int i = 0; i < num_tests; i++)
    {
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
        int rank, tasks;
        const int root = 0;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &tasks);

        // get the partition size by dividing the size by the number of tasks
        int partition_size = size / tasks;

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
        MPI_Gather(tmp1, partition_size, MPI_INT, v3, partition_size, MPI_INT, root, MPI_COMM_WORLD);

        // stop the timer
        auto stop = high_resolution_clock::now();

        // calculate time taken by addition
        if (rank == root)
        {
            auto duration = duration_cast<microseconds>(stop - start);
            printf("time taken by add: %li microseconds\n", duration.count());
        }

        // clean up our memory
        delete[] v1, v2, v3, tmp1, tmp2;

        // finalize the MPI environment
        MPI_Finalize();
    }
    
    return 0;
} // mpic++ add_mpi.cpp -o add_mpi && mpirun -np 4 -hostfile ./cluster ./add_mpi