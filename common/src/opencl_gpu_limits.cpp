#include <opencl_gpu_limits.h>
#include <opencl_helper.h>
#include <cstring> // memcpy

opencl_gpu_limits::opencl_gpu_limits() {
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
	cur_helper->read_gpu_limits(this);
}

void
opencl_gpu_limits::Set_All_Data(cl_ulong shared_mem_size,
				cl_ulong global_mem_size,
				cl_uint	num_of_compute_units,
				cl_ulong max_work_group_sz,
				size_t*	max_work_item_sz,
				cl_ulong max_mem_alloc_sz 
				)
{

	shared_memory_size	=	shared_mem_size;
	global_memory_size	=	global_mem_size; 
	number_of_compute_units =	num_of_compute_units;
	max_work_group_size	=	max_work_group_sz;
	
	memcpy(max_work_item_size, max_work_item_sz, sizeof(size_t)* MAX_GROUP_SIZE );

	max_mem_alloc_size	=	max_mem_alloc_sz;
}
