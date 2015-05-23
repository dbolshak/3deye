//------------------------------------------------------------------------------

template <class T>
gpu_array <T> :: gpu_array ( T  * host_data, 
                            dim3 host_data_size, 
                            gpu_transfer_type data_transfer_direction ) : host_ptr(NULL)
{
    set_prepare_status (true);
	set_host_ptr(host_data, host_data_size);
	set_data_transfer_type(data_transfer_direction);
	set_data_type_by_size ();
    set_arg_mem_type (GLOBAL);
}
//------------------------------------------------------------------------------

template <class T>
gpu_array <T> :: gpu_array ( T * host_data, 
                             dim3 host_data_size,
                            bool allocate_and_transfer,
                            gpu_transfer_type data_transfer_direction )
{
    create_and_set_gpu_only ( host_data, 
                            host_data_size,
                            allocate_and_transfer,
                            data_transfer_direction );
}

//------------------------------------------------------------------------------

template <class T>
gpu_array <T> :: gpu_array ( T * host_data, 
                            gpu_transfer_type data_transfer_direction )
{
	create_and_set_private_only (	host_data,
					data_transfer_direction,
					PRIVATE );
}

//------------------------------------------------------------------------------

template <class T>
gpu_array<T> :: gpu_array ( dim3 gpu_data_size,
                            bool should_allocate,
                            gpu_transfer_type data_transfer_direction ) : host_ptr(NULL)
{
    create_and_set_gpu_only ( NULL, 
                            gpu_data_size,
                            should_allocate,
                            data_transfer_direction );
}

//------------------------------------------------------------------------------

template <class T>
gpu_array<T> :: gpu_array ( unsigned int gpu_data_size,
                            bool should_allocate,
                            gpu_transfer_type data_transfer_direction ) : host_ptr(NULL)
{
	dim3 i_will_fix_it_promise (gpu_data_size);
    create_and_set_gpu_only ( NULL, 
                            i_will_fix_it_promise ,
                            should_allocate,
                            data_transfer_direction );
}

//------------------------------------------------------------------------------

template <class T>
gpu_array<T> :: gpu_array ( T * host_data,
                            dim3 gpu_data_size,
                            gpu_mem_type mem_type,
                            gpu_transfer_type data_transfer_direction ) : host_ptr(NULL)
{
    if ( data_transfer_direction != IN_GPU_USE_ONLY ) {
        cerr << "using following constructor you can't create kernel_arg with type other than IN_GPU_USE_ONLY"<< endl;
        throw;
    }

    if ( mem_type != PRIVATE ) {
        cerr << "using following constructor you can't create kernel_arg with type other than PRIVATE ! "<< endl;
        throw;
    }
	set_prepare_status (false);
	set_host_ptr_impl ( host_data );
	set_size( gpu_data_size);
	set_data_transfer_type(data_transfer_direction);
	set_data_type_by_size ();
	set_arg_mem_type(mem_type);

    cerr << "gpu_array constructor - I'm not sure that you can use it for any data which are not POD-like structs... "<< endl;

}
//------------------------------------------------------------------------------

template <class T>
gpu_array<T> :: gpu_array ( dim3 gpu_data_size,
                            gpu_mem_type mem_type,
                            gpu_transfer_type data_transfer_direction ) : host_ptr(NULL)
{
    if ( data_transfer_direction != IN_GPU_USE_ONLY ) {
        cerr << "using following constructor you can't create kernel_arg with type other than IN_GPU_USE_ONLY"<< endl;
        throw;
    }

    set_prepare_status (false);
	set_size(gpu_data_size);
	set_data_transfer_type(data_transfer_direction);
	set_data_type_by_size ();
    set_arg_mem_type(mem_type);
}

//------------------------------------------------------------------------------

template <class T>
void
gpu_array <T> :: create_and_set_gpu_only   (  
                                T * host_data, 
                                dim3 & data_size,
                                bool allocate_and_transfer,
                                gpu_transfer_type data_transfer_direction )
{
    if ( data_transfer_direction != IN_GPU_USE_ONLY ) {
        cerr << "using following constructor you can't create kernel_arg with type other than IN_GPU_USE_ONLY"<< endl;
        throw;
    }   // FIXME use assert instead!

    set_prepare_status ( true );
	set_data_transfer_type ( data_transfer_direction );
	set_data_type_by_size ();
    set_arg_mem_type ( GLOBAL );

    if ( host_data )
    	set_host_ptr ( host_data, data_size );
    else {
	    set_size ( data_size );
        if ( !allocate_and_transfer )
            cerr << "Well, I hope you know what are you doing - you do not allocate IN_GPU arg and kernel would not do it for you"<< endl;
    }

	// FIXME - this is right !set_data_type_by_size ();
    if ( allocate_and_transfer ) {
        call_allocate_and_transfer ( host_data );
        set_prepare_status ( false );
    }
}

//------------------------------------------------------------------------------

template <class T>
void
gpu_array <T> :: create_and_set_private_only   (  
                                T * host_data, 
                                gpu_transfer_type data_transfer_direction,
                            	gpu_mem_type mem_type
			)
{
    if ( data_transfer_direction != IN_GPU_USE_ONLY ) {
        cerr << "using following constructor you can't create kernel_arg with type other than IN_GPU_USE_ONLY"<< endl;
        throw;
    }

    if ( mem_type != PRIVATE ) {
        cerr << "using following constructor you can't create kernel_arg with type other than PRIVATE ! "<< endl;
        throw;
    }
	dim3 data_size;	// 1,1,1
	set_prepare_status (false);
	set_host_ptr_impl ( host_data );
	set_size( data_size);
	set_data_transfer_type(data_transfer_direction);
	set_data_type_by_size ();
	set_arg_mem_type(mem_type);
}

//------------------------------------------------------------------------------

template <class T>
gpu_array<T> :: gpu_array ()
{
    cerr << "gpu_array<T> :: gpu_array () Say : You should never call default constructor" << endl;
	throw ;
}

//------------------------------------------------------------------------------
/*  
 *      Do not erase old code with intention do not mistakenly implement it here
 * */
template <class T>
gpu_array<T> :: ~gpu_array ()
{
	// FIXME - do not forget that host_ptr do not allocated - it is just a entry point for data
	// free(host_ptr);
}

//------------------------------------------------------------------------------

template <class T>
gpu_array<T> :: gpu_array ( const gpu_array<T> & other )
{
    cerr << "gpu_array<T> :: gpu_array ( const gpu_array<T> & other ) Say : \nYou should never call copy constructor!"<< endl;
	throw ;
}

//------------------------------------------------------------------------------

template <class T>
void
gpu_array<T> :: set_host_ptr_impl ( T * initial_host_ptr, dim3 & new_data_size)
{
	if ( initial_host_ptr )  {
		set_size(new_data_size);
		host_ptr = initial_host_ptr;
	}
	else  {
		if ( ! initial_host_ptr )
		{
			cout << "gpu_array<T> :: set_host_ptr() Say : \n set_host_ptr() - initial_host_ptr is empty" << endl;
			throw ;
		}
		if ( host_ptr)
		{
			cout << "gpu_array<T> :: set_host_ptr() Say : \nset_host_ptr() - host_ptr is not empty!" << endl;
			throw ;
		}
	}
}

//------------------------------------------------------------------------------

/*
 *          Note it is internal routine for manipulating with opencl_kernel_argument as buffer
 *          for transfer data to the GPU :
 *          set host_ptr with the same size as total_size in this object's instance
 *
 * */
template <class T>
void
gpu_array<T>:: set_host_ptr_impl ( T * initial_hostdata )
{
    if ( ! initial_hostdata ) {
        cerr << "opencl_kernel_argument :: set_host_ptr bad pointer for transfer to GPU! " << endl;
        throw;
    }

    host_ptr = initial_hostdata;
}

//------------------------------------------------------------------------------

template <class T>
T * 
gpu_array<T> :: get_host_ptr_impl () 
{
	return host_ptr;
}

//------------------------------------------------------------------------------

template <class T>
void
gpu_array <T> :: memcpy_to_host ( T * host_dst )
{
    if ( ! host_dst ) {
        cerr << "Warning you pass unallocated pointer to host_dst to function memcpy_from_dev! it would be allocated for " << get_total_size() << " bytes " << endl;
        host_dst = (T*) malloc ( get_total_size() );
    }
    memcpy_from_dev ( host_dst );
}

//------------------------------------------------------------------------------

template <class T>
void
gpu_array<T> :: memcpy_from_host_impl ( T * host_dst )
{
    memcpy_to_dev ( host_dst );
}

//------------------------------------------------------------------------------

template <class T>
unsigned int
gpu_array<T> ::get_total_size() const
{
// FIXME    cout << "get_total_size - " << total_size * sizeof(T) << endl;
	return total_size * sizeof(T);
}

//------------------------------------------------------------------------------
