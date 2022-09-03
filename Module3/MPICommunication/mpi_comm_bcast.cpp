#include <mpi.h>
#include <cstdio>
#include <cstring>

int main(int argc, char** argv)
{
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    int rank;
    const int root = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const char* msg = "Hello World!";
    int buf_size = strlen(msg) + 1;
    char buf[buf_size];

    if (rank == root)
    {
	// only the root copies the message into the buffer directly
	// the other nodes receive it from the broadcast 
        strncpy(buf, msg, buf_size);
    }

    printf("Before Bcast, rank %i buf = %s\n", rank, buf);
    MPI_Bcast(&buf[0], buf_size, MPI_CHAR, root, MPI_COMM_WORLD);
    printf("After Bcast, rank %i buf = %s\n", rank, buf);

    // Finalize the MPI environment
    MPI_Finalize();
}
