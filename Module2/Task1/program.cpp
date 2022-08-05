#include <iostream>
#include <random>
#include <fstream>
#include <string>

// A, B and C are all N*N
// C = A * B
// initialize A and B with random values

const static int N = 3;

int main()
{
    int a[N][N] = {
        {1, 2, 2},
        {3, 2, 3},
        {2, 2, 3}
    };

    int b[N][N] = {
        {1, 1, 3},
        {3, 2, 1},
        {3, 2, 3}
    };

    int c[N][N] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    std::ofstream output;
    output.open("output.txt");
    if (output.is_open())
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                std::string numAsStr = std::to_string(c[i][j]);
                if (c[i][j] < 10) numAsStr = " " + numAsStr;

                std::string curOutput;
                if (i == 0 && j == 0) curOutput = "[[" + numAsStr + ", ";
                else if (i == (N - 1) && j == (N - 1)) curOutput = numAsStr + "]]";
                else if (j == (N - 1)) curOutput = numAsStr + "],\n";
                else if (j == 0) curOutput = " [" + numAsStr + ", ";
                else curOutput = numAsStr + ", ";

                output << curOutput;
            }
        }
        output.close();
    }

    std::cout << "hello\n";
    return 0;
}

//g++ program.cpp -o program && ./program