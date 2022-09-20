#include <iostream>
#include <string>
#include <cstdlib> // rand, srand
#include <ctime> // time
#include <omp.h>

using namespace std;

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
    #pragma omp parallel sections
    {
    #pragma omp section
        QuickSort(array, low, p - 1); // left side

    #pragma omp section
        QuickSort(array, p + 1, high); // right side
    }
}

int main() 
{
    // initialize rand with seed
    srand(unsigned(time(0)));

    const int array_size = 5;

    // create a randomized array
    int* array = RandomArray(array_size);

    cout << "before sort: ";
    PrintArray(array, array_size);

    // sort the array
    QuickSort(array, 0, array_size - 1);
    
    cout << "after sort: ";
    PrintArray(array, array_size);

    // free memory
    delete[] array;

    return 0;
}