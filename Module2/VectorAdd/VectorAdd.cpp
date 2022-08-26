#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>

using namespace std::chrono;
using namespace std;

#define NUM_TESTS 10

void randomVector(int *vector, int size)
{
    for (int i = 0; i < size; i++)
    {
        // give each index a random number between 1 and 100
        vector[i] = rand() % 100 + 1;
    }
}

int main()
{
    unsigned long size = 1000000;
    cout << "Test sequential program with size = " << size << endl;
    for (int i = 0; i < NUM_TESTS; i++)
    {
        srand((unsigned)time(0));

        int *v1, *v2, *v3;

        // dynamically allocate memory for vectors
        v1 = (int*)malloc(size * sizeof(int));
        v2 = (int*)malloc(size * sizeof(int));
        v3 = (int*)malloc(size * sizeof(int));

        // assign random values to v1 and v2
        randomVector(v1, size);
        randomVector(v2, size);

        // start the timer
        auto start = high_resolution_clock::now();

        // add v1 and v2 and put the result into v3
        for (int i = 0; i < size; i++)
        {
            v3[i] = v1[i] + v2[i];
        }

        // stop the timer
        auto stop = high_resolution_clock::now();

        // calculate time taken by addition
        auto duration = duration_cast<microseconds>(stop - start);
        cout << duration.count() << endl;
    }

    // cout << "Time taken by function: "
    //      << duration.count() << " microseconds" << endl;

    return 0;
}

// g++ VectorAdd.cpp -o VectorAdd && ./VectorAdd