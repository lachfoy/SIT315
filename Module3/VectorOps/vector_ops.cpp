#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define PRINT 1

int SZ = 8;
int *v;

cl_mem bufV; // stores a collection of processing elements
cl_device_id device_id; // id of device
cl_context context; // the context is an abstraction layer for interacting with the platform
cl_program program; // kernel program
cl_kernel kernel; // kernel function

// command queue
cl_command_queue queue;
cl_event event = NULL;

int err;

// set up a new device (either GPU or CPU is no GPU is available)
cl_device_id create_device();

// sets up a device, a context, a command queue and the program kernel
void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname);

// compiles the program for a context
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename);

// allocates buffer for kernel memory
void setup_kernel_memory();

// copy the arguments to the kernel function
void copy_kernel_args();

// deallocates all memory used
void free_memory();

void init(int *&A, int size)
{
    A = (int *)malloc(sizeof(int) * size);
    for (long i = 0; i < size; i++) {
        A[i] = rand() % 100; // any number less than 100
    }
}

void print(int *A, int size)
{
    if (PRINT == 0) {
        return;
    }

    if (PRINT == 1 && size > 15) {
        for (long i = 0; i < 5; i++) { // rows
            printf("%d ", A[i]); // print the cell value
        }
        printf(" ..... ");
        for (long i = size - 5; i < size; i++) { // rows
            printf("%d ", A[i]); // print the cell value
        }
    } else {
        for (long i = 0; i < size; i++) { // rows
            printf("%d ", A[i]); // print the cell value
        }
    }

    printf("\n----------------------------\n");
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        SZ = atoi(argv[1]);
    }

    init(v, SZ);

    size_t global[1] = { (size_t)SZ }; // SZ number elements of a single dimension

    print(v, SZ); // initial vector

    setup_openCL_device_context_queue_kernel((char *)"./vector_ops.cl", (char *)"square_magnitude");
    
    setup_kernel_memory();
    copy_kernel_args();

    // enqueues a command to execute a kernel on a device, sending the data to the buffer
    // args are command_queue, kernel, work_dim (dimensions), global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global, NULL, 0, NULL, &event);

    clWaitForEvents(1, &event);

    // reads from the buffer
    // args are command_queue, buffer, blocking_read, offset, size, pointer, num_events_in_wait_list, event_wait_list, event
    clEnqueueReadBuffer(queue, bufV, CL_TRUE, 0, SZ * sizeof(int), &v[0], 0, NULL, NULL);

    // result vector
    print(v, SZ);

    // frees memory for device, kernel, queue, etc.
    free_memory();
}

cl_device_id create_device()
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

void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname)
{
    device_id = create_device();
    cl_int err;

    // ToDo: Add comment (what is the purpose of clCreateBuffer function?)
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    
    if (err < 0) {
        perror("Couldn't create a context");
        exit(1);
    }

    program = build_program(context, device_id, filename);

    // ToDo: Add comment (what is the purpose of clCreateCommandQueueWithProperties function?)
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

cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename)
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

void setup_kernel_memory()
{
    // creates a buffer object
    // args are context, flags, size, host_prt, errcode_ret
    // the second parameter of the clCreateBuffer is cl_mem_flags flags. Check the OpenCL documention to find out what is it's purpose and read the List of supported memory flag values
    bufV = clCreateBuffer(context, CL_MEM_READ_WRITE, SZ * sizeof(int), NULL, NULL);

    // copy vectors to the buffer
    clEnqueueWriteBuffer(queue, bufV, CL_TRUE, 0, SZ * sizeof(int), &v[0], 0, NULL, NULL);
}

void copy_kernel_args()
{
    // sets a value for the specified kernel's arg
    // args are kenel, arg_index, arg_size, arg_value
    clSetKernelArg(kernel, 0, sizeof(int), (void *)&SZ);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufV);

    if (err < 0) {
        perror("Couldn't create a kernel argument");
        printf("error = %d", err);
        exit(1);
    }
}

void free_memory()
{
    // free the buffers
    clReleaseMemObject(bufV);

    // free opencl objects
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    free(v);
}