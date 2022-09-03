
#include <mpi.h>
#include <cstdio>
#include <cstring>

// send hello world
void PrimaryTask()
{
    // message is a const char buffer
    const char* msg = "Hello World!";
    
    // get the comm size
    int tasks;
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    
    // for each task (process) send a message
    // start at index 1, because index 0 will always be the primary
    for (int rank = 1; rank < tasks; ++rank)
    {
        // strlen + 1 for null terminator
        MPI_Send(msg, strlen(msg) + 1, MPI_CHAR, rank, 0, MPI_COMM_WORLD);
    }
}

// receive hello world
void SecondaryTask()
{
    // MPI_Probe and MPI_Get_count are used to dynamically set the string buffer size
    MPI_Status status;
    MPI_Probe(0, 0, MPI_COMM_WORLD, &status);// probe for an incoming message from 0

    // we can query the buf_size from the status struct that we just filled
    int buf_size;
    MPI_Get_count(&status, MPI_CHAR, &buf_size);

    // finally read the message contents into the buffer
    char buf[buf_size];
    MPI_Recv(&buf[0], buf_size, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Recieved: %s\n", buf);
}

int main(int argc, char** argv)
{
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) // 0 rank is the primary process
    {
        PrimaryTask(); // send hello world
    }
    else
    {
        SecondaryTask(); // recieve hello world
    }

    // Finalize the MPI environment
    MPI_Finalize();
}
