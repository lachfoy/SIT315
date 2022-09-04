#include <cstdio>
#include <cstdlib> // malloc, rand, srand
#include <time.h> // time
#include <chrono>

using namespace std::chrono;

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

    // add v1 and v2 and put the result into v3
    for (int i = 0; i < size; i++)
    {
        v3[i] = v1[i] + v2[i];
    }

    // stop the timer
    auto stop = high_resolution_clock::now();

    // calculate time taken by addition
    auto duration = duration_cast<microseconds>(stop - start);
    printf("time taken by add: %li microseconds\n", duration.count());

    // clean up our memory
    delete[] v1, v2, v3;

    return 0;
} // g++ add_seq.cpp -o add_seq