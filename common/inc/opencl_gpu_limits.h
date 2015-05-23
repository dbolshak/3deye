#ifndef OPENCL_GPU_LIMITS
#define OPENCL_GPU_LIMITS

#include <opencl_utils.h>
#include <constants.h>

/*
should be calculated during startup 
for appropriate use at all routines
*/

class opencl_gpu_limits
{
	// NOTE: cl_ulong - unsigned long long
	cl_ulong	shared_memory_size;
	cl_ulong	global_memory_size; 
	cl_uint		number_of_compute_units;
	cl_ulong	max_work_group_size;
	size_t		max_work_item_size[ MAX_GROUP_SIZE ];
	cl_ulong	max_mem_alloc_size;
	// warp size 32 - nvidia or 64 - ati

public:
	opencl_gpu_limits();
	
	void	Set_All_Data(	cl_ulong shared_mem_size,
				cl_ulong global_mem_size,
				cl_uint	num_of_compute_units,
				cl_ulong max_work_group_sz,
				size_t*	max_work_item_sz,
				cl_ulong max_mem_alloc_sz 
				);
};

#endif
