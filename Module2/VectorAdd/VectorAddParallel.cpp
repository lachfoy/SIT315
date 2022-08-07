#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <pthread.h>

using namespace std::chrono;
using namespace std;

#define NUM_THREADS 4

struct AddTask
{
    int *v1; // reference to vector
    int *v2; // reference to vector
    int *v3; // reference to vector
    int start; // start point of partition
    int end; // end point of partition
};

void* addVector(void* args)
{
    AddTask *addTask = (AddTask*)args;
    for (int i = addTask->start; i < addTask->end; i++)
    {
        addTask->v3[i] = addTask->v1[i] + addTask->v2[i];
    }

    return nullptr;
}

void randomVector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
    {
        // give each index a random number
        vector[i] = rand() % 100;
    }
}

int main()
{
    // decompose array into partition
    // for each partition assign a thread to do the work
    
    // initialize rand with seed
    srand((unsigned)time(nullptr));

    unsigned long size = 100000000;
    int *v1, *v2, *v3;

    // create thread pools
    pthread_t threadPool[NUM_THREADS];

    
    // start timer
    auto start = high_resolution_clock::now();

    // allocate memory for the vectors
    v1 = (int*)malloc(size * sizeof(int*));
    v2 = (int*)malloc(size * sizeof(int*));
    v3 = (int*)malloc(size * sizeof(int*));

    // randomize v1 and v2
    randomVector(v1, size);
    randomVector(v2, size);

    // partition the vectors according to the number of threads
    int partitionSize = size / NUM_THREADS;

    // allocate the first half of available threads to the first vector
    for(int i = 0; i < NUM_THREADS; i++)
    {
        // create task for the thread
        AddTask *addTask = (AddTask*)malloc(sizeof(AddTask));

        // pass reference to the vectors
        addTask->v1 = v1;
        addTask->v2 = v2;
        addTask->v3 = v3;

        // pass in the partition start and end point
        addTask->start = i * partitionSize;
        addTask->end = addTask->start + partitionSize - 1;

        // create thread
        pthread_create(&threadPool[i], NULL, addVector, (void*)addTask);
    }

    // join back the threads into the main thread
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threadPool[i], NULL);
    }

    // stop the timer
    auto stop = high_resolution_clock::now();


    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}

// g++ VectorAddParallel.cpp -o VectorAddParallel -lpthread && ./VectorAddParallel