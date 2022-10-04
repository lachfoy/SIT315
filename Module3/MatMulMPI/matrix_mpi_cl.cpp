#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <cstdlib>
#include <mpi.h> // "/usr/lib/mpich/include"
#include <chrono>
#include <CL/cl.h>

using namespace std::chrono;

cl_mem bufTmp1; // stores a collection of processing elements
cl_mem bufB; // stores a collection of processing elements
cl_mem bufTmp2; // stores a collection of processing elements

cl_device_id device_id; // id of device
cl_context context; // the context is an abstraction layer for interacting with the platform
cl_program program; // kernel program
cl_kernel kernel; // kernel function

// command queue
cl_command_queue queue;
cl_event event = NULL;

int err;

// sets up a device, a context, a command queue and the program kernel
void SetupOpenCLDeviceContextQueueKernel(char *filename, char *kernelname);

// set up a new device (either GPU or CPU is no GPU is available)
cl_device_id CreateDevice();

// compiles the program for a context
cl_program BuildProgram(cl_context ctx, cl_device_id dev, const char *filename);

void FillRandomMatrix(int* matrix, const int matrixSize)
{
    for (int i = 0; i < matrixSize * matrixSize; i++) {
        matrix[i] = rand() % 10 + 1; // 1 to 10
    }
}

void PrintMatrix(int* matrix, const int matrixSize)
{
    // print matrix in a .csv style format
    std::string output;
    for (int i = 0; i < matrixSize * matrixSize; i++) {
        std::string numAsStr = std::to_string(matrix[i]);
        if ((i + 1) % matrixSize == 0) {
            numAsStr += "\n";
        } else {
            numAsStr += ",";
        }
        output += numAsStr;
    }

    std::cout << output << std::endl;
}

void OutputToTxt(int* matrix, const int matrixSize)
{
    // generate output.txt in a .csv style format
    std::ofstream output;
    output.open("output.txt");
    if (output.is_open()) {
        for (int i = 0; i < matrixSize * matrixSize; i++) {
            std::string numAsStr = std::to_string(matrix[i]);
            if ((i + 1) % matrixSize == 0) {
                numAsStr += "\n";
            } else {
                numAsStr += ",";
            }
            output << numAsStr;
        }
        output.close();
    }  
}

int main(int argc, char** argv)
{
    srand((unsigned)time(0)); // seed the random number generator

    // initialize the MPI environment
    MPI_Init(&argc, &argv);

    // get the rank and size
    int rank, size;
    const int root = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int matrixSize = 2048;
    const int partitionSize = matrixSize * matrixSize / size;

    // set up OpenCL context
    SetupOpenCLDeviceContextQueueKernel((char*)"./matrix.cl", (char*)"Multiply");

    size_t global[1] = { (size_t)partitionSize }; // number elements of a single dimension

    // allocate memory for matrices
    int *a = new int[matrixSize * matrixSize];
    int *b = new int[matrixSize * matrixSize];
    int *c = new int[matrixSize * matrixSize];
    
    // allocate memory for temp data
    int *tmp1 = new int[partitionSize];
    int *tmp2 = new int[partitionSize];

    if (rank == root) {
        FillRandomMatrix(a, matrixSize);
        FillRandomMatrix(b, matrixSize);
    }

    auto start = std::chrono::high_resolution_clock::now(); // start timer

    // scatter rows of matrix a to all workers into tmp1 (worker for each row)
    MPI_Scatter(a, partitionSize, MPI_INT, tmp1, partitionSize, MPI_INT, root, MPI_COMM_WORLD);

    // copy matrix b to each worker
    // necessary because only the root node randomizes the matrices
    MPI_Bcast(b, matrixSize * matrixSize, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // set up the OpenCL buffers
    bufTmp1 = clCreateBuffer(context, CL_MEM_READ_ONLY, partitionSize * sizeof(int), NULL, NULL);
    bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, matrixSize * matrixSize * sizeof(int), NULL, NULL);
    bufTmp2 = clCreateBuffer(context, CL_MEM_READ_WRITE, partitionSize * sizeof(int), NULL, NULL);

    // copy data to the buffers
    clEnqueueWriteBuffer(queue, bufTmp1, CL_TRUE, 0, partitionSize * sizeof(int), &tmp1[0], 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, matrixSize * matrixSize * sizeof(int), &b[0], 0, NULL, NULL);
    //clEnqueueWriteBuffer(queue, bufTmp2, CL_TRUE, 0, partitionSize * sizeof(int), &tmp2[0], 0, NULL, NULL);

    // sets a value for the specified kernel's arg
    // args are kenel, arg_index, arg_size, arg_value
    clSetKernelArg(kernel, 0, sizeof(int), (void*)&matrixSize);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)bufTmp1);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)bufB);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)bufTmp2);

    if (err < 0) {
        perror("Couldn't create a kernel argument");
        printf("error = %d", err);
        exit(1);
    }

    // enqueues a command to execute a kernel on a device, sending the data to the buffer
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global, NULL, 0, NULL, &event);

    clWaitForEvents(1, &event);

    // reads from the buffer
    clEnqueueReadBuffer(queue, bufTmp2, CL_TRUE, 0, partitionSize * sizeof(int), &tmp2[0], 0, NULL, NULL);

    // gather the result back into c
    MPI_Gather(tmp2, partitionSize, MPI_INT, c, partitionSize, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    auto stop = std::chrono::high_resolution_clock::now(); // stop timer

    // print the result time
    if (rank == root) {
        auto duration = duration_cast<microseconds>(stop - start);
        std::cout << duration.count() << std::endl;
    }

    // output the final matrix to output.txt
    if (rank == root) {
        OutputToTxt(c, matrixSize);
    }

    // deallocate memory
    clReleaseMemObject(bufTmp1);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufTmp2);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    delete[] a, b, c, tmp1, tmp2;

    // exit mpi
    MPI_Finalize();

    return 0;
}

void SetupOpenCLDeviceContextQueueKernel(char *filename, char *kernelname)
{
    device_id = CreateDevice();
    cl_int err;

    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    
    if (err < 0) {
        perror("Couldn't create a context");
        exit(1);
    }

    program = BuildProgram(context, device_id, filename);

    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);

    if (err < 0) {
        perror("Couldn't create a command queue");
        exit(1);
    };

    kernel = clCreateKernel(program, kernelname, &err);
    if (err < 0) {
        perror("Couldn't create a kernel");
        printf("error =%d", err);
        exit(1);
    };
}

cl_device_id CreateDevice()
{
    cl_platform_id platform;
    cl_device_id dev;

    int err;

    /* Identify a platform */
    err = clGetPlatformIDs(1, &platform, NULL);

    if (err < 0) {
        perror("Couldn't identify a platform");
        exit(1);
    }

    // Access a device
    // GPU
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
    if (err == CL_DEVICE_NOT_FOUND) {
        // CPU
        printf("GPU not found\n");
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
    }

    if (err < 0) {
        perror("Couldn't access any devices");
        exit(1);
    }

    return dev;
}

cl_program BuildProgram(cl_context ctx, cl_device_id dev, const char *filename)
{
    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;

    /* Read program file and place content into buffer */
    program_handle = fopen(filename, "r");

    if (program_handle == NULL)
    {
        perror("Couldn't find the program file");
        exit(1);
    }

    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    // loads kernel program from source buffer of chars (a plaintext file)
    program = clCreateProgramWithSource(ctx, 1, (const char **)&program_buffer, &program_size, &err);

    if (err < 0) {
        perror("Couldn't create the program");
        exit(1);
    }

    free(program_buffer);

    // Build program
    // the fourth parameter accepts options that configure the compilation.
    // these are similar to the flags used by gcc.
    // for example, you can define a macro with the option -DMACRO=VALUE and turn off optimization with -cl-opt-disable.
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err < 0) {
        /* Find size of log and print to std output */
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        program_log = (char *)malloc(log_size + 1);
        program_log[log_size] = '\0';

        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);

        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    return program;
}

// mpic++ matrix_mpi_cl.cpp -o matrix_mpi_cl -lOpenCL && mpirun -np 4 -hostfile ./cluster ./matrix_mpi_cl