// g++ quick_sort_parallel_tests.cpp -o parallel_tests -lpthread
// ./parallel_tests 1000

#include <iostream>
#include <string>
#include <cstdlib> // rand, srand
#include <ctime> // time
#include <pthread.h>
#include <chrono>

using namespace std;
using namespace chrono;

struct QuickSortInfo
{
    int* array;
    int low;
    int high;
};

int* RandomArray(const int array_size)
{
    int* array = new int[array_size];

    for (int i = 0; i < array_size; i++) {
        array[i] = rand() % 100 + 1;
    }

    return array;
}

void PrintArray(const int* array, const int array_size)
{
    cout << "[";
    for (int i = 0; i < array_size - 1; i++) {
        cout << to_string(array[i]) << ","; 
    }
    cout << to_string(array[array_size - 1]) << "]\n"; // print the last item
}

int Partition(int* array, const int low, const int high)
{
    int pivot = array[high]; // choose the last element as the pivot

    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (array[j] <= pivot) {
            i++;
            swap(array[i], array[j]);
        }
    }

    i++;
    swap(array[i], array[high]);

    return i;
}

void* QuickSort(void* args)
{
    QuickSortInfo* info = (QuickSortInfo*)args;

    int low, high, p;

    low = info->low;
    high = info->high;

    if (low < high) {
        // create threads and attr
        pthread_attr_t attr;
        pthread_t thread1, thread2;

        // create structs for passing args to threads
        QuickSortInfo thread1info, thread2info;

        // copy the array into the seperate thread tasks
        thread1info.array = info->array;
        thread2info.array = info->array;

        // attr change thread detach state
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        // create pivot
        p = Partition(info->array, low, high);

        // recurse
        thread1info.low = low;
        thread1info.high = p - 1;

        pthread_create(&thread1, &attr, QuickSort, &thread1info);

        thread2info.low = p + 1;
        thread2info.high = high;

        pthread_create(&thread2, &attr, QuickSort, &thread2info);

        // join threads
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    }
}

int main(int argc, char *argv[])
{
    // initialize rand with seed
    srand(unsigned(time(0)));

    const int array_size = atoi(argv[1]);
    cout << "size=" << array_size << endl;

    const int num_tests = 10;
    for (int i = 0; i < num_tests; i++) {
        // create a randomized array
        int* array = RandomArray(array_size);

        // cout << "before sort: ";
        // PrintArray(array, array_size);

        // sort the array
        QuickSortInfo info;
        info.array = array;
        info.low = 0;
        info.high = array_size - 1;

        

        pthread_t thread;

        // start the timer
        auto start = high_resolution_clock::now();
        pthread_create(&thread, NULL, QuickSort, &info);
        pthread_join(thread, NULL); // joined once work is finished
        // stop the timer
        auto stop = high_resolution_clock::now();

        // calculate time taken by addition
        auto duration = duration_cast<microseconds>(stop - start);
        cout << duration.count() << endl;


        

        
        
        // cout << "after sort: ";
        // PrintArray(array, array_size);

        // free memory
        delete[] array;
    }

    pthread_exit(NULL);

    

    return 0;
}