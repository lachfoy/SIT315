__kernel void AddVector(const int size, __global int* v1, __global int* v2, __global int* v3)
{
    // thread identifiers
    const int globalIndex = get_global_id(0);   
 
    // uncomment to see the index each PE works on
    //printf("Kernel process index: (%d)\n ", globalIndex);

    v3[globalIndex] = v1[globalIndex] + v2[globalIndex];
}
