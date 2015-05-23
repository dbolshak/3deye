#include <gpu_array_base.h>
#include <opencl_helper.h>

//-----------------------------------------------------------------------------
gpu_array_base :: ~gpu_array_base() {
    dev_free();
};

void
gpu_array_base :: dev_free () {
    if ( mem_type != PRIVATE ) {
	    opencl_helper* cur_helper = opencl_helper::Get_Instance();
	    cur_helper->release_kernel_argument ( this );
    }
}

void
gpu_array_base::copy_to ( gpu_array_base * dst ) {
    opencl_helper* cur_helper = opencl_helper::Get_Instance();
    cur_helper->transfer_kernel_argument_to_kernel_argument ( this, dst );
}

/*      unsafe version which do not check pointer!
 *      use memcpy_to_host instead
 * */
void  gpu_array_base::memcpy_from_dev ( void * host_dst) {
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
	cur_helper->transfer_kernel_argument_to_cpu ( this, host_dst, false );
};

//-----------------------------------------------------------------------------
void   gpu_array_base::memcpy_to_dev   ( void* host_src ) {
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
	cur_helper->transfer_kernel_argument_to_gpu ( this );
};

//-----------------------------------------------------------------------------
void gpu_array_base :: call_allocate_and_transfer ( bool need_to_transfer_data_to_gpu ) {
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
	cur_helper->allocate_kernel_argument ( this );

	if ( need_to_transfer_data_to_gpu ) // it is possible that we want to use in_gpu mem, not data from host 
		cur_helper->transfer_kernel_argument_to_gpu ( this );
};

//-----------------------------------------------------------------------------
void    gpu_array_base:: dev_alloc () {
		
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
	cur_helper->allocate_kernel_argument(this);

	need_to_prepare = false;

};

//-----------------------------------------------------------------------------
void gpu_array_base::reset_gpu_part () {
	device_ptr = NULL;
};

//-----------------------------------------------------------------------------
gpu_mem_type 
gpu_array_base::get_arg_mem_type() const {
    return mem_type;
};

//-----------------------------------------------------------------------------
void
gpu_array_base::set_arg_mem_type ( gpu_mem_type new_mem_type ) {
    mem_type = new_mem_type;
    switch (new_mem_type)
    {
		case GLOBAL : break;
		case LOCAL :  { reset_gpu_part(); } break;
		case PRIVATE: { reset_gpu_part(); } break;
		default : { cerr << "Unknown mem_type at gpu_array<T>::set_arg_mem_type: " << new_mem_type << endl; throw; } break;
    }
};


//-----------------------------------------------------------------------------
void
gpu_array_base::set_data_transfer_type( gpu_transfer_type new_type) {
	data_transfer_direction = new_type;
}

//-----------------------------------------------------------------------------
gpu_transfer_type 
gpu_array_base::get_data_transfer_type() const {
	return data_transfer_direction;
}

//-----------------------------------------------------------------------------
void
gpu_array_base::set_data_type ( gpu_data_type initial_data_type ) {
	data_type	=	initial_data_type;	
}

//-----------------------------------------------------------------------------
void
gpu_array_base::set_data_type_by_size ( ) {

   data_type = BUFFER_1D;

   if ( ( sizes.y != 1 ) && ( sizes.z == 1 ) )
       data_type = IMAGE_2D;

  // FIXME - add another options 
}

//-----------------------------------------------------------------------------
gpu_data_type
gpu_array_base::get_data_type() const {
	return data_type;
}

//-----------------------------------------------------------------------------
void 
gpu_array_base::set_prepare_status ( bool prepare_status ) {
    need_to_prepare = prepare_status;
}

//-----------------------------------------------------------------------------
unsigned int
gpu_array_base::get_total_size() const {
	return total_size;
}

//-----------------------------------------------------------------------------
bool
gpu_array_base::should_be_prepared() const {
    return need_to_prepare; //should_be_prepared;
}

//-----------------------------------------------------------------------------
void
gpu_array_base::set_device_ptr ( cl_mem & initial_device_ptr ) {
	device_ptr = initial_device_ptr;
}

//-----------------------------------------------------------------------------
cl_mem * 
gpu_array_base::get_device_ptr () {
	return & device_ptr;
}

//-----------------------------------------------------------------------------
unsigned int
gpu_array_base::width () const {
    return sizes.x;
}

//-----------------------------------------------------------------------------
unsigned int
gpu_array_base::height () const {
    return sizes.y;
}

//-----------------------------------------------------------------------------
unsigned int
gpu_array_base::depth () const {
    return sizes.z;
}

//-----------------------------------------------------------------------------
dim3
gpu_array_base::get_size() const {
	return sizes;
}

//-----------------------------------------------------------------------------
void
gpu_array_base::set_size ( const dim3 & new_data_size ) {
	if ( new_data_size.x == 0 || new_data_size.y == 0 || new_data_size.z == 0 ) {
		cout << "opencl_kernel_argument::set_size - one of dimensions is equal to zero" << endl;
		throw;
	}
	sizes = new_data_size;
	total_size = sizes.x * sizes.y * sizes.z;
}

//-----------------------------------------------------------------------------
size_t
gpu_array_base::elem_size() const {
    return 0;
}