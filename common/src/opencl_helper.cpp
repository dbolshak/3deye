#include <opencl_helper.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <utility>

using namespace std;

opencl_helper* opencl_helper::opencl_helper_instance = NULL;

//------------------------------------------------------------------------------

opencl_helper::opencl_helper(profiling_level profiling_level)
{
	set_default_values();
	init_opencl(profiling_level);
    
    allocated_total = 0;
}

//------------------------------------------------------------------------------

void opencl_helper::set_default_values()
{
	cur_device_id						=	NULL;
	cur_context                         =	NULL;
	cur_command_queue					=	NULL;
	cur_platform_id						=	NULL;
	opencl_routine_return_value         =	0;
	profiling                           =	ENABLE_PROFILING;
	
	// FIXME - should be set for kernel for appropriate value
	//	for buffer - 1D
	//	for images_2D - 2D 
	num_of_dimensions					=	1;

	//memset(global_work_size,0,sizeof(global_work_size));
	//memset(local_work_size,0,sizeof(local_work_size));
	DBG(cout," default values was set!" );
}

//------------------------------------------------------------------------------

void opencl_helper::init_opencl(profiling_level profiling_level)
{
	/*
		FIXME
			1. read gpu limits
			2. in case poor resources or do not having appropriate GPU
			- try to switch at CPU processing with warnings for user
			3. if limits for cpu and gpu less then minimum (should be in constants.h
			then say sorry and die
	*/
	if (profiling_level == ENABLE_PROFILING) 
		profiling = CL_QUEUE_PROFILING_ENABLE;
	
	cl_uint ret_num_devices		=	0;
	cl_uint ret_num_platforms	=	0;
	CL_CHECK(	clGetPlatformIDs(1, &cur_platform_id, &ret_num_platforms)					);
	CL_CHECK(	clGetDeviceIDs(cur_platform_id, CL_DEVICE_TYPE_GPU, 1, &cur_device_id, &ret_num_devices)	);
	cur_context	  =	CL_CHECK_ERR(	clCreateContext(NULL, 1, &cur_device_id, NULL, NULL, &opencl_routine_return_value)		);
	cur_command_queue =	CL_CHECK_ERR(	clCreateCommandQueue(cur_context, cur_device_id, profiling, &opencl_routine_return_value)	);

	DBG(cout, "init_opencl was successful");
    read_gpu_limits( /* FIXME gpu_limits should be internal field and should be used */ );

}

//------------------------------------------------------------------------------
/*  NOTE:
 *  1) opencl_gpu_limits can be created on stack - it leads to segfault
 *  2) you should actually take into account this values
 *
 *  discussions about best practices:
 *  http://stackoverflow.com/questions/10096443/what-is-the-algorithm-to-determine-optimal-work-group-size-and-number-of-workgro/10098063#10098063
 *  http://stackoverflow.com/questions/3957125/questions-about-global-and-local-work-size
 *
 *  generaly you should query:
 *  constraints on work-group size
 *  CL_KERNEL_WORK_GROUP_SIZE - kernel_dependent
 *  CL_DEVICE_MAX_WORK_GROUP_SIZE - device dependent
 *
 * */
void opencl_helper::read_gpu_limits(opencl_gpu_limits* gpu_limits)
{
	// FIXME - a bit ugly and too prolix
	cl_ulong	shared_memory_size;
	cl_ulong	global_memory_size; 
	cl_uint		number_of_compute_units;
//	cl_ulong	max_work_group_size;
	size_t		max_work_item_size[MAX_GROUP_SIZE];
	cl_ulong	max_mem_alloc_size;

	CL_CHECK(clGetDeviceInfo(cur_device_id, CL_DEVICE_LOCAL_MEM_SIZE,	sizeof(cl_ulong), &shared_memory_size, NULL));
	CL_CHECK(clGetDeviceInfo(cur_device_id, CL_DEVICE_GLOBAL_MEM_SIZE,	sizeof(cl_ulong), &global_memory_size, NULL));
	CL_CHECK(clGetDeviceInfo(cur_device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(cl_ulong), &device_max_work_group_size, NULL)); // total number of workitems in workgroup
	CL_CHECK(clGetDeviceInfo(cur_device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*3, &max_work_item_size, NULL)); // max size for all dim
	CL_CHECK(clGetDeviceInfo(cur_device_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &max_mem_alloc_size, NULL));
	CL_CHECK(clGetDeviceInfo(cur_device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &number_of_compute_units, NULL));


    if (gpu_limits)
    	gpu_limits->Set_All_Data(shared_memory_size, global_memory_size,
                        number_of_compute_units, device_max_work_group_size,
                        max_work_item_size, max_mem_alloc_size);

	// FIXME 
/*
	cl_uint uiNumSupportedFormats = 0;

	// 2D
	clGetSupportedImageFormats(context, CL_MEM_READ_ONLY, 
				   CL_MEM_OBJECT_IMAGE2D,   
				   NULL, NULL, &uiNumSupportedFormats);
	cl_image_format* ImageFormats = new cl_image_format[uiNumSupportedFormats];
	clGetSupportedImageFormats(context, CL_MEM_READ_ONLY, 
				   CL_MEM_OBJECT_IMAGE2D,   
				   uiNumSupportedFormats, ImageFormats, NULL);

	for(unsigned int i = 0; i < uiNumSupportedFormats; i++) 
	{  
		    cout << oclImageFormatString(ImageFormats[i].image_channel_order) 
	    << " - " << oclImageFormatString(ImageFormats[i].image_channel_data_type)
		<< endl;
	}

	delete[] ImageFormats;
*/
	// FIXME to separate function
    // cout << "max_work_item_size " << max_work_item_size[0] << "x" << max_work_item_size[1] << "x"<< max_work_item_size[2] << endl;

	// CL_DEVICE_MEM_BASE_ADDR_ALIGN
	DBG(cout, shared_memory_size );
	DBG(cout, global_memory_size );
	DBG(cout, max_work_group_size); 
	DBG(cout, max_mem_alloc_size);
	DBG(cout, number_of_compute_units);
	DBG(cout, " read_gpu_limits was successful"); 

	cout << " read_gpu_limits was successful" << endl; 
	cout << " shared_memory_size      : " << shared_memory_size << endl ;
	cout << " global_memory_size      : " << global_memory_size << endl;
	cout << " max_mem_alloc_size      : " << max_mem_alloc_size << endl;
	cout << " number_of_compute_units : " << number_of_compute_units << endl;
}

//------------------------------------------------------------------------------

opencl_helper::~opencl_helper()
{
	free_gpu_resources();
}

//------------------------------------------------------------------------------

void opencl_helper::free_gpu_resources()
{
	CL_CHECK(	clReleaseCommandQueue(cur_command_queue)	);
	CL_CHECK(	clReleaseContext(cur_context)			);
}

//------------------------------------------------------------------------------

opencl_helper* opencl_helper::Get_Instance(profiling_level profiling_level)
{
	if (!opencl_helper_instance)
		opencl_helper_instance = new opencl_helper(profiling_level);
	return opencl_helper_instance;
}

//------------------------------------------------------------------------------

void opencl_helper::build_kernel(opencl_kernel* new_kernel, const string& path_to_kernel_file)
{
	string kernel_source = get_kernel_source_from_file(path_to_kernel_file);
	string kernel_name = get_kernel_name_from_file(path_to_kernel_file);

	// FIXME amazing >_< - no any allocation	
	char* source_str = (char*) kernel_source.c_str();
	size_t source_size = kernel_source.length();

	cl_program program = CL_CHECK_ERR(	clCreateProgramWithSource(cur_context, 1, (const char**) &source_str,&source_size, &opencl_routine_return_value)	);
	int _err =				clBuildProgram(program, 1, &cur_device_id, DEFAULT_GPU_COMPILE_FLAGS, NULL, NULL);
	if (_err!=CL_SUCCESS)
	{
		// Determine the size of the log
		size_t build_log_size = 0;
		clGetProgramBuildInfo(program, cur_device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);

		// Allocate memory for the log
		char *build_log = (char *) malloc(build_log_size);

		// Get the log
		clGetProgramBuildInfo(program, cur_device_id, CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);

		// Print the log
		cout << "Kernel " << kernel_name << " build error!"<< endl << build_log << endl;
		free (build_log);

		abort();
	}

	cl_kernel kernel = CL_CHECK_ERR(		clCreateKernel(program, kernel_name.c_str(), &opencl_routine_return_value)				);
	new_kernel->set_program(program);
	new_kernel->set_kernel(kernel);
	new_kernel->set_name(kernel_name);
}

//------------------------------------------------------------------------------

void opencl_helper::release_kernel(opencl_kernel* kernel_for_release)
{

	CL_CHECK(	clReleaseKernel(kernel_for_release->get_kernel())	);
	CL_CHECK(	clReleaseProgram(kernel_for_release->get_program())	);
}

//------------------------------------------------------------------------------

string opencl_helper::get_kernel_source_from_file(const string &path_to_kernel_file)
{
	string kernel_source = "NONE";

	ifstream file_with_kernel (path_to_kernel_file.c_str(), ios::in|ios::binary|ios::ate);
        if (file_with_kernel.is_open())
        {
    		file_with_kernel.seekg(0, std::ios::end);
		kernel_source.resize(file_with_kernel.tellg());
		file_with_kernel.seekg(0, std::ios::beg);
		file_with_kernel.read(&kernel_source[0], kernel_source.size());
		file_with_kernel.close();
	}
	else
	{
                // something wrong is happen
                // what we are going todo
		cout << "can't read file " << path_to_kernel_file << endl;
		abort();
        }
	
	return kernel_source;
}

//------------------------------------------------------------------------------

string opencl_helper::get_kernel_name_from_file(const string &path_to_kernel_file)
{
	string kernel_file_name = "NONE";

	string intermediate_name = "";

        intermediate_name = string(find_if(	path_to_kernel_file.rbegin(), path_to_kernel_file.rend(),
                      			MatchPathSeparator() ).base(),
        				path_to_kernel_file.end() );

	kernel_file_name = intermediate_name.substr( 0, intermediate_name.find_last_of( '.' ) );

	// FIXME check for length?	
	return kernel_file_name;
}

//------------------------------------------------------------------------------

//	FIXME	width/height should be encapsulated in opencl_kernel ???
//	at present it is used at every opencl_kernel_argument - probably we should change smth
void opencl_helper::determine_group_size(opencl_kernel * target_kernel)
{
	dim3 sizes = target_kernel->get_global_work_size();
	size_t width	= sizes.data[WIDTH];
	size_t height	= sizes.data[HEIGHT];
	size_t depth	= sizes.data[DEPTH]; 

	size_t		global_work_size[MAX_GROUP_SIZE];
	size_t		local_work_size[MAX_GROUP_SIZE];
	size_t kernel_work_size = 0;	
	CL_CHECK(clGetKernelWorkGroupInfo(target_kernel->get_kernel(), cur_device_id,CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &kernel_work_size, NULL));
        // FIXME -  we also can query CL_KERNEL_LOCAL_MEM_SIZE
	
	// copy from epgpu
	if ( height == 0 )
	{
		// single dimension case
		string epgpu;
		local_work_size[0] = width;
		while(local_work_size[0]>kernel_work_size) 
			local_work_size[0] = (local_work_size[0]+1)/2;
		local_work_size[1] = 0;
		local_work_size[2] = 0;
		
		global_work_size[0] = (width+local_work_size[0]-1)/local_work_size[0]*local_work_size[0];
		global_work_size[1] = 0;
		global_work_size[2] = 0;
		num_of_dimensions = 1;
	}
	else if ( depth == 0 )
	{
		// double dimension case
		string epgpu;

		// FIXME - ugly algo!
	
		local_work_size[0] = width;
		local_work_size[1] = height;

		while (local_work_size[0]>kernel_work_size)
			local_work_size[0] = (local_work_size[0]+1)/2;
		while (local_work_size[1]>kernel_work_size)
			local_work_size[1] = (local_work_size[1]+1)/2;

		while (local_work_size[0]*local_work_size[1]>kernel_work_size)
		{
			if (local_work_size[0]>2*local_work_size[1])
				local_work_size[0] = (local_work_size[0]+1)/2;
			else
				local_work_size[1] = (local_work_size[1]+1)/2;
		}
		local_work_size[2] = 0;

		global_work_size[0] = (width+local_work_size[0]-1)/local_work_size[0]*local_work_size[0];
		global_work_size[1] = (width+local_work_size[1]-1)/local_work_size[1]*local_work_size[1];
		global_work_size[2] = 0;

		num_of_dimensions = 2;
	}
	else	// 3-dimensional case
	{
		cout << " 3-dimensional case is not supported!" << endl;
		throw " 3-dimensional case is not supported!";
	}

	global_work_size[0] = kernel_work_size;

	// FIXME - set correct global/local work group size
	// yeah, yeah - deep copy here
	target_kernel->set_global_work_size( global_work_size[0], global_work_size[1], global_work_size[2] );
	target_kernel->set_local_work_size( & local_work_size[0] );
}

void
opencl_helper::show_how_many_memory_is_allocated()
{
    cout << "Allocated total - " << allocated_total << endl;
}
//------------------------------------------------------------------------------

void
opencl_helper::release_kernel_argument( gpu_array_base * arg_to_release)
{
    if ( arg_to_release->get_arg_mem_type() == LOCAL) {
        cerr << " Try to free LOCAL arg!" << endl;
        throw;
    }
#ifdef DEBUG
    cout << "opencl_helper->release_kernel_argument() before deleting "<< arg_to_release->get_total_size() << endl;
    cout << "opencl_helper->release_kernel_argument() device ptr - "<< arg_to_release->get_device_ptr () << endl;
#endif	
    CL_CHECK(  clReleaseMemObject( *(arg_to_release->get_device_ptr())) );
    allocated_total -=  arg_to_release->get_total_size();
}

//------------------------------------------------------------------------------
/**
	allocation only
	without explicit copy
*/
void
opencl_helper::allocate_kernel_argument ( gpu_array_base * arg_to_allocate)
{
	cl_mem gpu_kernel_param = NULL;

	// FIXME 072014
	size_t ptr_to_sizes[3];
	dim3 fixme = arg_to_allocate->get_size();
	ptr_to_sizes[WIDTH] = fixme.x;
	ptr_to_sizes[HEIGHT] = fixme.y;
	ptr_to_sizes[DEPTH] = fixme.z;

	if (!ptr_to_sizes)
		throw "allocate_kernel_argument - can't determine ptr_size";

	switch ( arg_to_allocate->get_data_type() )
	{
        case CONSTANT_BUFFER:
		case BUFFER_1D:
        {
#ifdef DEBUG
            cout << "opencl_helper::allocate_kernel_argument for " << arg_to_allocate->get_total_size() << endl;
#endif
            gpu_kernel_param = CL_CHECK_ERR( clCreateBuffer(cur_context, CL_MEM_READ_WRITE, arg_to_allocate->get_total_size(), NULL, &opencl_routine_return_value)  );
            allocated_total +=  arg_to_allocate->get_total_size();
        }
        break;
		case IMAGE_2D:
        {
            cl_image_desc img_desc {
                CL_MEM_OBJECT_IMAGE2D,
                fixme.width,
                fixme.height,
                fixme.depth,
                1, // image_array_size
                0, // img row pitch
                0, // img slice pitch
                0, // num_mip_level, num_samples
                0 /*NULL    // buffer*/
            }; 
            gpu_kernel_param = CL_CHECK_ERR (   clCreateImage (cur_context, CL_MEM_READ_WRITE, &DEFAULT_IMG_FORMAT,
                                                &img_desc,
                                                NULL,
                                                &opencl_routine_return_value) 
                                            );
        }
        break;
		default:
        break;
	}
	arg_to_allocate->set_device_ptr( gpu_kernel_param );

#ifdef DEBUG
    cout << "allocate_kernel_argument device ptr " << arg_to_allocate->get_device_ptr() << endl;
#endif

}
//------------------------------------------------------------------------------

void
opencl_helper::transfer_kernel_argument_to_gpu( gpu_array_base * arg_to_transfer)
{
   if (         (   arg_to_transfer->get_data_transfer_type() != TO_GPU             ) && 
                (   arg_to_transfer->get_data_transfer_type() != TO_GPU_AND_BACK    ) &&
                (   arg_to_transfer->get_data_transfer_type() != IN_GPU_USE_ONLY    ) 

      )
            return; // it is should be OUT param (or error)
	size_t ptr_to_sizes[3];
	dim3 fixme = arg_to_transfer->get_size();
	ptr_to_sizes[WIDTH] = fixme.x;
	ptr_to_sizes[HEIGHT] = fixme.y;
	ptr_to_sizes[DEPTH] = fixme.z;
	switch ( arg_to_transfer->get_data_type() )
    {
        case BUFFER_1D:
        {
#ifdef DEBUG                                    
            cout << "Want to send - " << arg_to_transfer->get_total_size() << endl;
            cout << "device_ptr - " << arg_to_transfer->get_device_ptr () << endl;
            cout << "data - " << arg_to_transfer->get_host_ptr() << endl;
#endif                    
            CL_CHECK(       clEnqueueWriteBuffer(
                                cur_command_queue,
                                *(arg_to_transfer->get_device_ptr()),
                                CL_TRUE,
                                0,      /* offset - think about it */
                                arg_to_transfer->get_total_size(),
                                arg_to_transfer->get_host_ptr(),
                                0,
                                NULL,
                                NULL));
        }
        break;
        
        case IMAGE_2D:
        {
            size_t size3DOrig[3] = {0, 0, 0};
            size_t size3D[3] = { ptr_to_sizes[WIDTH], ptr_to_sizes[HEIGHT], ptr_to_sizes [DEPTH] };
#ifdef DEBUG
            cout << "Want to send - " << arg_to_transfer->get_total_size() << endl;
            cout << " - " << ptr_to_sizes[WIDTH] << " - "<< ptr_to_sizes[HEIGHT] << " - " << ptr_to_sizes [DEPTH] << endl;
            cout << arg_to_transfer->get_host_ptr() << endl;
#endif
            CL_CHECK ( clEnqueueWriteImage (
                                cur_command_queue,
                                *(arg_to_transfer->get_device_ptr()),
                                CL_TRUE,        /* blocking read  */
                                size3DOrig, size3D,
                                ptr_to_sizes[WIDTH]*arg_to_transfer->elem_size(),
                                0, 
                                arg_to_transfer->get_host_ptr(),
                                0, 
                                NULL, NULL) );
        }
        break;
        
        default:
                    // actually WTF
        break;

    }
}
//------------------------------------------------------------------------------

void
opencl_helper :: transfer_kernel_argument_to_cpu ( gpu_array_base * arg_to_transfer, bool perform_check)
{
    transfer_kernel_argument_to_cpu(arg_to_transfer, arg_to_transfer->get_host_ptr(), perform_check);
}

//------------------------------------------------------------------------------

void
opencl_helper::transfer_kernel_argument_to_cpu ( gpu_array_base * arg_to_transfer, void * host_ptr, bool perform_check)
{
    if (perform_check) {
    	if ( (arg_to_transfer->get_data_transfer_type() != TO_CPU) && 
	    	(arg_to_transfer->get_data_transfer_type() != TO_GPU_AND_BACK) )
		    return;	// it is should be IN param (or error)
    }
	size_t ptr_to_sizes[3];
	dim3 fixme = arg_to_transfer->get_size();
	ptr_to_sizes[WIDTH] = fixme.x;
	ptr_to_sizes[HEIGHT] = fixme.y;
	ptr_to_sizes[DEPTH] = fixme.z;
	switch ( arg_to_transfer->get_data_type() )
	{
		case BUFFER_1D:
        {
#ifdef DEBUG                    
					cout << "Want to get - " << arg_to_transfer->get_total_size() << endl;
#endif                    
				CL_CHECK(       clEnqueueReadBuffer(
							cur_command_queue,
							*(arg_to_transfer->get_device_ptr()),
							CL_TRUE,
							0,		/* offset - thik about it */ 
							arg_to_transfer->get_total_size(),
							host_ptr,
							0,
							NULL,
							NULL
							));
        }
        break;
		case IMAGE_2D:
        {
            size_t size3DOrig[3] = {0, 0, 0};
#ifdef DEBUG
            cout << "Want to get - " << arg_to_transfer->get_total_size() << endl;
            cout << " - " << ptr_to_sizes[WIDTH] << " - "<< ptr_to_sizes[HEIGHT] << " - " << ptr_to_sizes [DEPTH] << endl;
#endif
            size_t size3D[3] = { ptr_to_sizes[WIDTH], ptr_to_sizes[HEIGHT], ptr_to_sizes [DEPTH]};
            CL_CHECK ( clEnqueueReadImage (
							cur_command_queue,
							*(arg_to_transfer->get_device_ptr()),
							CL_TRUE,        /* blocking read  */
							size3DOrig, size3D,
							 ptr_to_sizes[WIDTH]*arg_to_transfer->elem_size() ,
                            0, 
							host_ptr,
							0, 
							NULL, NULL) );
        }
        break;
		default:
				// actually WTF
		break;

	}

}

//------------------------------------------------------------------------------
void
opencl_helper :: transfer_kernel_argument_to_kernel_argument ( gpu_array_base * src, gpu_array_base * dst)
{
	switch ( src->get_data_type() )
	{
		case BUFFER_1D:
				{
                    if (dst->get_data_type() != src->get_data_type() )
                    {
                        cerr << "opencl_helper::transfer_kernel_argument_to_kernel_argument data_type mismatchs! "<< endl;
                        throw;
                    }

                    if ( dst->get_total_size() < src->get_total_size () )
                    {
                        cerr << "opencl_helper::transfer_kernel_argument_to_kernel_argument dst size less than src:" << dst->get_total_size() << " vs "<< src->get_total_size() << endl;
                        throw;
                    }

                    CL_CHECK ( clEnqueueCopyBuffer (  cur_command_queue,
                                                        *(src->get_device_ptr()),
                                                        *(dst->get_device_ptr()),
                                                        0,
                                                        0,
                                                        src->get_total_size(),
                                                        0,
                                                        NULL,
                                                        NULL));
			}
				break;
		case IMAGE_2D:
                        {
                            cerr << "transfer_kernel_argument_to_kernel_argument data_type unsupported! "<< endl;
                            throw;
                        }
        default:
                    break;
    }
}


//------------------------------------------------------------------------------
void
opencl_helper::prepare_kernel_argument (opencl_kernel* kernel_to_run, int argument_num, gpu_array_base * arg_to_prepare)
{
#ifdef DEBUG
        cout << "prepare_kernel_argument param num - " << argument_num << " for kernel  "
	<< kernel_to_run->get_name() << endl;
        cout << "data size is " << arg_to_prepare->get_total_size () << endl;
#endif
    switch ( arg_to_prepare->get_arg_mem_type() ) 
    {
        case GLOBAL: {
                        if ( arg_to_prepare->should_be_prepared() ) {
                            if ( arg_to_prepare->get_data_transfer_type() == IN_GPU_USE_ONLY ) {
                                cout << "KILL THEM'ALL - WE TRY TO allocate IN_GPU_USE_ONLY " << argument_num << endl;
                            throw;
                            }
                        allocate_kernel_argument(arg_to_prepare);
                        transfer_kernel_argument_to_gpu(arg_to_prepare);
                        }
                        CL_CHECK( clSetKernelArg(kernel_to_run->get_kernel(), argument_num, sizeof(cl_mem), (void*) arg_to_prepare->get_device_ptr() ) );
                     }
                     break;
        case  LOCAL:
                    {
                        CL_CHECK( clSetKernelArg(kernel_to_run->get_kernel(), argument_num, arg_to_prepare->get_total_size(), NULL ) );
                    }
                    break;
        case PRIVATE:
                    {
                        CL_CHECK( clSetKernelArg(kernel_to_run->get_kernel(), argument_num, arg_to_prepare->get_total_size(),  (void*) arg_to_prepare->get_host_ptr() ) );
                    }
                    break;
        default :
                    {
                        cerr << "opencl_helper::prepare_kernel_argument sorry to say but I'm not familar how to use this type of args - "<< arg_to_prepare->get_arg_mem_type() << endl;
                        throw;
                    }
                    break;
    }
}

//------------------------------------------------------------------------------

void opencl_helper::execute_kernel(opencl_kernel* kernel_to_execute)
{
	cl_event kernel_completion;

#ifdef DEBUG    
    cout << "Try to execute kernel " << kernel_to_execute->get_name()  << endl;
	cout << "opencl_helper::execute_kernel - global_work_size - " << kernel_to_execute->get_global_work_size() << endl;
	cout << "opencl_helper::execute_kernel - local_work_size - " << kernel_to_execute->get_local_work_size() << endl;
#endif    
    if ( kernel_to_execute->get_local_work_size().x == 0 ) {
     
	    CL_CHECK(clEnqueueNDRangeKernel(cur_command_queue, kernel_to_execute->get_kernel(), kernel_to_execute->get_num_dimensions(), NULL, &(kernel_to_execute->get_global_work_size().data[0]), NULL, 0, NULL, &kernel_completion));
    }
    else {
        if ( ! is_local_work_size_valid(kernel_to_execute) ) {
            cerr << "Local work size for this kernel incorrect. Check it!"<< endl;
            throw;
        }
        CL_CHECK(clEnqueueNDRangeKernel(cur_command_queue, kernel_to_execute->get_kernel(), kernel_to_execute->get_num_dimensions(), NULL, &(kernel_to_execute->get_global_work_size().data[0]), &(kernel_to_execute->get_local_work_size().data[0]) , 0, NULL, &kernel_completion) );
    }

	CL_CHECK(clFinish(cur_command_queue));
	CL_CHECK(clWaitForEvents(1, &kernel_completion));
	
	if (profiling == CL_QUEUE_PROFILING_ENABLE)
	{
		cl_ulong	ev_start_time	= (cl_ulong)0;
		cl_ulong	ev_end_time	= (cl_ulong)0;
		
		CL_CHECK(clGetEventProfilingInfo(kernel_completion, CL_PROFILING_COMMAND_START,sizeof(cl_ulong), &ev_start_time, NULL));
		CL_CHECK(clGetEventProfilingInfo(kernel_completion, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_end_time, NULL));
		double kernel_execute_time = (double) (ev_end_time - ev_start_time)/1000000.0; // in usec
        cout << "kernel "<< kernel_to_execute->get_name() <<" running time is - "<< kernel_execute_time << " ms"<< endl;
		// DBG(cout, kernel_execute_time);
	}
	CL_CHECK(clReleaseEvent(kernel_completion));
}

//------------------------------------------------------------------------------

void opencl_helper::show_kernel_info(opencl_kernel* kernel_to_execute)
{
    size_t fuck;
    CL_CHECK( clGetKernelWorkGroupInfo (  kernel_to_execute->get_kernel(),
                    cur_device_id,
                    CL_KERNEL_WORK_GROUP_SIZE,
                    sizeof(size_t) ,
                    &fuck,
                    NULL)  );
    cout << "CL_KERNEL_WORK_GROUP_SIZE - "<< fuck << endl;


    // FIXME - add another kernel-specific data
}

//------------------------------------------------------------------------------

bool opencl_helper :: is_local_work_size_valid ( opencl_kernel * kernel_for_check )
{
    size_t max_workgroup_size;          // local_work group size at kernel arguments
    CL_CHECK( clGetKernelWorkGroupInfo (  kernel_for_check->get_kernel(),
                    cur_device_id,
                    CL_KERNEL_WORK_GROUP_SIZE,
                    sizeof(size_t) ,
                    &max_workgroup_size,
                    NULL)  );

    size_t * user_local_size = kernel_for_check->get_local_work_size().data;

    size_t total_user_size = user_local_size[WIDTH] * user_local_size[HEIGHT] * user_local_size [DEPTH];
    if (total_user_size > max_workgroup_size ) {

        cerr << "is_local_work_size_valid determine that " << endl <<
            "max_local_size - " << max_workgroup_size << endl <<
            "but user_work_size - " << user_local_size[WIDTH] << 
            "x" << user_local_size[HEIGHT] << "x" <<
            user_local_size[DEPTH] << " total - " << total_user_size << endl;
        return false;
    }
    
    if ( total_user_size > device_max_work_group_size){

        cerr << "is_local_work_size_valid determine that " << endl <<
        " maximum work size for current device is -  " << device_max_work_group_size << 
        "but user_work_size - " << user_local_size[WIDTH] << "x" 
        << user_local_size[HEIGHT] << "x" <<
        user_local_size[DEPTH] << " total - " << total_user_size << endl;

        return false;
    }

    return true;    
}
    
//------------------------------------------------------------------------------
