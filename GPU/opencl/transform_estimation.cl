#define GENERATE_OPENCL_CODE
#include <constants.h>

__kernel
void
transform_estimation (  __read_only     __global float * gbuf,
                        __write_only    __global float * output,
                        int buf_length
                        )
{
	int tid = get_local_id(0);

    int start = get_group_id(0) * 1200;
    int end = start + buf_length;

	float sum = 0.f;

    for ( int id = start + tid; id < end; id += DEFAULT_STRIDE ) 
        sum += gbuf[id];

	local float smem [ DEFAULT_STRIDE ];

	smem[tid] = sum;
	barrier (CLK_LOCAL_MEM_FENCE);

/*                  REDUCE START                    */

    int internal_id = get_local_id(2) * get_local_size(0) * get_local_size(1) +
			get_local_id(1) * get_local_size(0) +
			get_local_id(0);	//flattenedThreadId
	float  val =  smem [internal_id]; // used through the whole function!

    if (internal_id < 128)  smem [internal_id] = val = val + smem[internal_id + 128]; barrier (CLK_LOCAL_MEM_FENCE);
    if (internal_id <  64)  smem [internal_id] = val = val + smem[internal_id +  64]; barrier (CLK_LOCAL_MEM_FENCE);

    if (internal_id < 32)
		  smem[internal_id] = val = val + smem[internal_id +  32]; barrier (CLK_LOCAL_MEM_FENCE);
    if (internal_id < 32)
		  smem[internal_id] = val = val + smem[internal_id +  16]; barrier (CLK_LOCAL_MEM_FENCE);
    if (internal_id < 32)
		  smem[internal_id] = val = val + smem[internal_id +   8]; barrier (CLK_LOCAL_MEM_FENCE);
    if (internal_id < 32)
		  smem[internal_id] = val = val + smem[internal_id +   4]; barrier (CLK_LOCAL_MEM_FENCE);
    if (internal_id < 32)
		  smem[internal_id] = val = val + smem[internal_id +   2]; barrier (CLK_LOCAL_MEM_FENCE);
    if (internal_id < 32)
		  smem[internal_id] = val = val + smem[internal_id +   1]; barrier (CLK_LOCAL_MEM_FENCE);
/*                  REDUCE END                      */
	
	if (tid == 0) {
		output [ get_group_id ( 0 ) ] = smem[0];
   }
}
