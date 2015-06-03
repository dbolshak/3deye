#define GENERATE_OPENCL_CODE
#define INCLUDED_IN_IMPLEMENTATION_FILE
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#include <elementary_points.h>

__kernel
void scan_for_interest_point ( __read_only __global unsigned char * DI_dx,
                               __read_only __global unsigned char * DI_dy,
                               float scale, /* Sobel scale ? */
                               __write_only __global int * Indexes,
                               __write_only __global bool* Flags,
                               __write_only __global int * Num_Of_Interest_Point,
                               point_2i img_size
                             )
{

    int x = get_global_id(0);
    int y = get_global_id(0);

    int width = img_size.x;

    Indexes [ y * width + x ] = 0;
    Flags [ y * width + x ] = false;
    float g_squared = DI_dx [ y * width + x ] * DI_dx [ y * width + x ] + DI_dy [ y * width + x ] * DI_dy [ y * width + x ];

 	local size_t smem [ 256 ];

	int tid = 	get_local_id(2) * get_local_size(0) * get_local_size(1) +
                get_local_id(1) * get_local_size(0) +
                get_local_id(0);	//flattenedThreadId
  
    if ( g_squared >= scale )
    {
        Indexes [ y * width + x ] = 1;
        Flags [ y * width + x ] = true;
        smem [ tid ] = 1;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

/*  REDUCE START */
    int  val =  smem [tid];
        
    if (tid < 128)  smem[tid] = val = val + smem[tid + 128]; barrier (CLK_LOCAL_MEM_FENCE);
    if (tid < 64)   smem[tid] = val = val + smem[tid +  64]; barrier (CLK_LOCAL_MEM_FENCE);
    if (tid < 32)   smem[tid] = val = val + smem[tid +  32]; barrier (CLK_LOCAL_MEM_FENCE);
    if (tid < 32)   smem[tid] = val = val + smem[tid +  16]; barrier (CLK_LOCAL_MEM_FENCE);
    if (tid < 32)   smem[tid] = val = val + smem[tid +   8]; barrier (CLK_LOCAL_MEM_FENCE);
    if (tid < 32)   smem[tid] = val = val + smem[tid +   4]; barrier (CLK_LOCAL_MEM_FENCE);
    if (tid < 32)   smem[tid] = val = val + smem[tid +   2]; barrier (CLK_LOCAL_MEM_FENCE);
    if (tid < 32)   smem[tid] = val = val + smem[tid +   1]; barrier (CLK_LOCAL_MEM_FENCE);
/*  REDUCE END    */

    if ( tid == 0 ) {
        atomic_add ( Num_Of_Interest_Point, smem[0] );
    }
}
