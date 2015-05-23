#include <opencl_helper.h>
#include <algorithm>

/* FIXME MOBE TO MORE PROPER PLACE */
string
mem_type_to_str ( gpu_mem_type id )
{
    string res = "Unknown";

    switch ( id )
    {
        case LOCAL: res = "LOCAL";break;
        case GLOBAL: res = "GLOBAL";break;
        case PRIVATE: res = "PRIVATE";break;
        case CONSTANT: res = "CONSTANT";break;
        case IMAGE: res = "IMAGE";break;
    };

    return res;
}

string
transfer_type_to_str ( gpu_transfer_type id )
{
    string res = "Unknown";

    switch ( id )
    {
        case TO_NONE: res = "TO_NONE";break;
        case TO_CPU: res = "TO_CPU";break;
        case TO_GPU: res = "TO_GPU";break;
        case TO_GPU_AND_BACK: res = "TO_GPU_AND_BACK";break;
        case IN_GPU_USE_ONLY: res = "IN_GPU_USE_ONLY";break;
    };

    return res;
}
//--------------------------------------------------------------------------------

opencl_kernel::opencl_kernel(const string & path_to_kernel_file, profiling_level profiling)
{
	opencl_helper* cur_helper = opencl_helper::Get_Instance(profiling);
	cur_helper->build_kernel(this, path_to_kernel_file);
    
    set_local_work_size(0,0,0);
    set_global_work_size(0,0,0);
}

//--------------------------------------------------------------------------------

void
opencl_kernel::prepare()
{
	if ( kernel_args.empty() )
		throw "opencl_kernel::prepare can't execute kernel without args";
	
	opencl_helper* cur_helper = opencl_helper::Get_Instance();

	for (int i = 0 ; i < kernel_args.size(); i++) {
#ifdef DEBUG
        cout << "before preparing argument "<< i<< " with size "<< kernel_args[i]->get_total_size() << " for kernel " << kernel_name<< endl;
#endif
		cur_helper->prepare_kernel_argument(this, i, kernel_args[i]);
	}
}

//------------------------------------------------------------------------------

void
opencl_kernel :: free_args()
{
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
    for (int i =0; i < kernel_args.size(); i++) {
        if (! kernel_args[i]){
            cerr << "opencl_kernel->free_args() - try to delete args which are already freed "<< endl;
            continue;
        }
        if (kernel_args[i]->get_data_transfer_type() != IN_GPU_USE_ONLY ) {
#ifdef DEBUG
            cout << "release " << i << " arg " << " for " << kernel_name << " transfer_type -  " << transfer_type_to_str ( kernel_args[i]->get_data_transfer_type()) << mem_type_to_str (kernel_args[i]->get_arg_mem_type() )  << " size " << kernel_args[i]->get_total_size() << endl;
#endif
		    cur_helper->release_kernel_argument(kernel_args[i]);
        }
    }
    kernel_args.clear();

}

//------------------------------------------------------------------------------

void
opencl_kernel :: show_args_info()
{
    cout << " Argument types for kernel "<< get_name () << endl;
    for (int i = 0; i < kernel_args.size(); i++ ) {
        cout << "I - " << i;
        cout << " transfer_type - "<< transfer_type_to_str ( kernel_args[i]->get_data_transfer_type() ) << " ";
        cout << "mem_type - "<< mem_type_to_str ( kernel_args[i]->get_arg_mem_type() ) << endl;
    }
}

void
opencl_kernel::show_mem_usage()
{
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
    cur_helper->show_how_many_memory_is_allocated();
}

opencl_kernel::~opencl_kernel()
{
	//	FIXME free all resources
    free_args();	
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
	cur_helper->release_kernel(this);
}

const cl_kernel
opencl_kernel::get_kernel()
{
	return kernel;
}

const cl_program
opencl_kernel::get_program()
{
	return program;
}

const string
opencl_kernel::get_name()
{
	return kernel_name;
}

void
opencl_kernel::set_kernel(cl_kernel init_kernel)
{
	kernel	=	init_kernel;
}

void
opencl_kernel::set_program(cl_program init_program)
{
	program	=	init_program;
}

void
opencl_kernel::set_name(const string & init_kernel_name)
{
	kernel_name	=	init_kernel_name;
}

// FIXME - rethink

opencl_kernel::opencl_kernel( const opencl_kernel& another_object)
{
	throw "YOU SHOULD NOT CALL COPY_CONSTRUCTOR !!!";
}

opencl_kernel & opencl_kernel::operator=(const opencl_kernel& rhs)
{
	throw "YOU SHOULD NEVER CALL OPERATOR= for opencl_kernel object";

	if (this != &rhs) {
		
	}
	return *this;
}

void
opencl_kernel::run()
{
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
	cur_helper->execute_kernel(this);
}

/*
	fill args kernel_args with data from gpu
*/
void
opencl_kernel::fill_hosts_data (/* kernel args list */)
{
	if ( kernel_args.empty() )
		throw "opencl_kernel::fill_hosts_data can't execute kernel without args";
	
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
	
	for (int i = 0 ; i < kernel_args.size(); i++) {
		cur_helper->transfer_kernel_argument_to_cpu(kernel_args[i]);
    }
}

void
opencl_kernel::set_global_work_size(size_t width, size_t height, size_t depth)
{
	global_work_size.data[WIDTH]	=	width;
	global_work_size.data[HEIGHT]	=	height;
	global_work_size.data[DEPTH]	=	depth;

}

void
opencl_kernel::set_global_work_size(const size_t* initial_global_work_size)
{
	// FIXME - check pointer
	set_global_work_size(initial_global_work_size[WIDTH], initial_global_work_size[HEIGHT], initial_global_work_size[DEPTH]);
}

int
opencl_kernel::get_num_dimensions()
{
	int num_of_dimensions = 0;
	
	for (int i = 0; i < MAX_GROUP_SIZE; i++ )
		if ( global_work_size.data[i] > 0 )
			num_of_dimensions++;

	if ( (num_of_dimensions > 3) || (num_of_dimensions == 0) )
		throw "opencl_kernel::Get_Num_Dimensions";
	
	return num_of_dimensions;
}

dim3
opencl_kernel::get_global_work_size()
{
	return global_work_size;
}

void
opencl_kernel::set_local_work_size(const size_t * initial_local_work_size)
{
	if (!initial_local_work_size) {
		cout << " opencl_kernel::Set_Local_Work_Size - bad pointer "<< endl;
		throw "opencl_kernel::Set_Local_Work_Size - bad pointer";
	}


	set_local_work_size ( initial_local_work_size [ WIDTH ], initial_local_work_size [ HEIGHT ], initial_local_work_size [ DEPTH ] );
}

void
opencl_kernel::set_local_work_size(size_t  new_width, size_t new_height, size_t new_depth)
{
	local_work_size.data[WIDTH] = new_width;
	local_work_size.data[HEIGHT] = new_height;
	local_work_size.data[DEPTH] = new_depth;
}

dim3
opencl_kernel::get_local_work_size()
{
	return local_work_size;
}

void
opencl_kernel::add_kernel_arg (gpu_array_base * new_arg)
{
    if (!new_arg) {
        cerr << "opencl_kernel Add_Kernel_Arg bad arg!" << endl;
        throw;
    }

    kernel_args.push_back( new_arg );
}

void
opencl_kernel::execute( const size_t * initial_global_work_size, bool Try_To_Determine_Group_Sizes )
{
	execute(initial_global_work_size[WIDTH], initial_global_work_size[HEIGHT], initial_global_work_size[DEPTH]);
}

void
opencl_kernel::show_kernel_info() 
{
    opencl_helper* cur_helper = opencl_helper::Get_Instance();
    cur_helper->show_kernel_info(this);
    show_args_info();
}

void
opencl_kernel::execute( size_t width, size_t height, size_t depth, bool Try_To_Determine_Group_Sizes )
{
	set_global_work_size(width, height, depth);
	
    if ( Try_To_Determine_Group_Sizes ) {
	    opencl_helper* cur_helper = opencl_helper::Get_Instance();
    	cur_helper->determine_group_size(this);
    }
	
	prepare();
	run();
	fill_hosts_data();
}

/*
 *      blindly use provided local and global work size
 * */
void
opencl_kernel :: execute (const size_t * global_work_size, const size_t * local_work_size)
{
    set_global_work_size(global_work_size);
    set_local_work_size(local_work_size);

	prepare();
	run();
	fill_hosts_data();
}