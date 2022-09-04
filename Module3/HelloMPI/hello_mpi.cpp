#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    int num_tasks, rank, name_len, tag = 1; 
    char name[MPI_MAX_PROCESSOR_NAME];
    // Initialize the MPI environment
    MPI_Init(&argc,&argv);

    // Get the number of tasks/process
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

    // Get the rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Find the processor name
    MPI_Get_processor_name(name, &name_len);

    // Print off a hello world message
    printf("Hello SIT315. You get this message from %s, rank %d out of %d\n", name, rank, num_tasks);

    // Finalize the MPI environment
    MPI_Finalize();
} // mpic++ hello_mpi.cpp -o hello_mpi && mpirun -np 4 -hostfile ./cluster ./hello_mpi