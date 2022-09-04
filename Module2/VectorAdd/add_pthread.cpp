#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <pthread.h>

using namespace std::chrono;
using namespace std;

#define NUM_THREADS 4
#define NUM_TESTS 10

void RandomVector(int *vector, int size)
{
    for (int i = 0; i < size; i++)
    {
        // give each index a random number between 1 and 100
        vector[i] = rand() % 100 + 1;
    }
}

struct AddTask
{
    int *v1; // reference to vector
    int *v2; // reference to vector
    int *v3; // reference to vector
    int start; // start point of partition
    int end; // end point of partition
};

void* AddVector(void *args)
{
    AddTask *task = (AddTask*)args;
    for (int i = task->start; i < task->end; i++)
    {
        task->v3[i] = task->v1[i] + task->v2[i];
    }

    return nullptr;
}

int main()
{
    unsigned long size = 10000000;
    cout << "Test parallel program with size = " << size << endl;
    for (int i = 0; i < NUM_TESTS; i++)
    {
        srand((unsigned)time(0));

        int *v1, *v2, *v3;

        // allocate memory for the vectors
        v1 = (int*)malloc(size * sizeof(int));
        v2 = (int*)malloc(size * sizeof(int));
        v3 = (int*)malloc(size * sizeof(int));

        // randomize v1 and v2
        RandomVector(v1, size);
        RandomVector(v2, size);

        // start the timer
        auto start = high_resolution_clock::now();

        // create thread pools
        pthread_t thread_pool[NUM_THREADS];

        // partition the vectors according to the number of threads
        int partition_size = size / NUM_THREADS;

        // allocate the first half of available threads to the first vector
        for(int i = 0; i < NUM_THREADS; i++)
        {
            // create task for the thread
            AddTask *task = (AddTask*)malloc(sizeof(AddTask));

            // pass reference to the vectors
            task->v1 = v1;
            task->v2 = v2;
            task->v3 = v3;

            // pass in the partition start and end point
            task->start = i * partition_size;
            task->end = task->start + partition_size - 1;

            // create thread
            pthread_create(&thread_pool[i], NULL, AddVector, (void*)task);
        }

        // join back the threads into the main thread
        for (int i = 0; i < NUM_THREADS; i++)
        {
            pthread_join(thread_pool[i], NULL);
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

// g++ add_pthread.cpp -o add_pthread -lpthread && ./add_pthread