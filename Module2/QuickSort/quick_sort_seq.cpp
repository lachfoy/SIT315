#include <iostream>
#include <string>

using namespace std;

void PrintArray(const int* array, const int array_size)
{
    cout << "[";
    for (int i = 0; i < array_size - 1; i++)
    {
        cout << to_string(array[i]) << ","; 
    }
    cout << to_string(array[array_size]) << "]\n"; // print the last item
}

int main() 
{
    int array_size = 5;
    int* array = new int[array_size];

    array[0] = 3;
    array[1] = 1;
    array[2] = 5;
    array[3] = 2;
    array[4] = 4;

    cout << "Starting array: ";
    PrintArray(array, array_size);

    return 0;
}

// g++ quick_sort_seq.cpp -o test