#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <omp.h>

using namespace std::chrono;
using namespace std;

#define NUM_THREADS 4
#define NUM_TESTS 1

void RandomVector(int *vector, int size)
{
    for (int i = 0; i < size; i++)
    {
        // give each index a random number between 1 and 100
        vector[i] = rand() % 100 + 1;
    }
}

int main()
{
    unsigned long size = 100000;

    cout << "Test parallel OMP program with size = " << size << endl;
    for (int test = 0; test < NUM_TESTS; test++)
    {
        srand((unsigned)time(0));

        int *v1, *v2, *v3;

        // dynamically allocate memory for vectors
        v1 = (int*)malloc(size * sizeof(int));
        v2 = (int*)malloc(size * sizeof(int));
        v3 = (int*)malloc(size * sizeof(int));

        // assign random values to v1 and v2
        RandomVector(v1, size);
        RandomVector(v2, size);

        // start the timer
        auto start = high_resolution_clock::now();

        int total = 0;

        // add v1 and v2 and put the result into v3
        #pragma omp parallel num_threads(NUM_THREADS) default(none) firstprivate(size) shared(v1, v2, v3, total)
        {
            int sum = 0;
            #pragma omp (private sum) for schedule(static)
            for (int i = 0; i < size; i++)
            {
                v3[i] = v1[i] + v2[i];
                sum += v3[i];
            }
            #pragma omp critical
            {
                total += sum;
            }
        }
        #pragma omp barrier

        // stop the timer
        auto stop = high_resolution_clock::now();

        // calculate time taken by addition
        auto duration = duration_cast<microseconds>(stop - start);
        cout << duration.count() << endl;

        cout << total << endl;
    }

    // cout << "Time taken by function: "
    //      << duration.count() << " microseconds" << endl;

    return 0;
}

// g++ VectorAddParallelOMP.cpp -o VectorAddParallelOMP -fopenmp && ./VectorAddParallelOMP