__kernel void Multiply(int matrixSize, __global int* tmp1, __global int* b, __global int* tmp2)
{
    // thread identifiers
    const int globalIndex = get_global_id(0);   
    int sum = 0;
    for (int i = 0; i < matrixSize; i++) {
        sum += tmp1[i] * b[globalIndex + (i * matrixSize)];
    }

    tmp2[globalIndex] = sum;
}
