__kernel void square_magnitude(const int size, __global int* v)
{
    // thread identifiers
    const int globalIndex = get_global_id(0);   
 
    // uncomment to see the index each PE works on
    printf("Kernel process index :(%d)\n ", globalIndex);

    v[globalIndex] = v[globalIndex] * v[globalIndex];
}
