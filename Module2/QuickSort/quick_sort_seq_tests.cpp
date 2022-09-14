// g++ quick_sort_seq_tests.cpp -o seq_tests
// ./seq_tests 1000

#include <iostream>
#include <string>
#include <cstdlib> // rand, srand
#include <ctime> // time
#include <chrono>

using namespace std;
using namespace chrono;

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

void QuickSort(int* array, const int low, const int high)
{
    // base case
    if (low >= high || low < 0) return;

    // partition to get a pivot index
    int p = Partition(array, low, high);

    // recurse
    QuickSort(array, low, p - 1); // left side
    QuickSort(array, p + 1, high); // right side
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

        // start the timer
        auto start = high_resolution_clock::now();

        // sort the array
        QuickSort(array, 0, array_size - 1);

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

    return 0;
}