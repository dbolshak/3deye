#define GENERATE_OPENCL_CODE
#define INCLUDED_IN_IMPLEMENTATION_FILE

__kernel
void clean_buff ( __global float * gPtr, int width )
{
    int id = get_global_id(0);

    for ( int i = 0; i < 27; i++ )
        gPtr [ i * width + id ] = 0.f;
}
