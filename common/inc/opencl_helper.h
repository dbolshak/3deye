#ifndef OPENCL_HELPER
#define OPENCL_HELPER

#include <opencl_gpu_limits.h>
#include <opencl_kernel.h>

/** 
used for executing opencl kernel at gpu in linear order
usage:
	opencl_helper* cur_helper = opencl_helper::Get_Instance();
	kernel_argument_list kernel_args;
	kernel_args.add("arg_name",size_t arg_size, void* arg_data);
	cur_helper->kernel_run("my_kernel.cl", kernel_args, width[,height]);

    we can control GPU mem manipuation from this object
*/
class opencl_helper
{
        unsigned int    allocated_total;
	public:
		static opencl_helper* Get_Instance(profiling_level profiling=DISABLE_PROFILING);
	
		/*
			we use following naming conventions:
			file name - kernel_name.cl
					which contains _single_ opencl kernel
					with name equal to "kernel_name"
		*/	
		void build_kernel(opencl_kernel* new_kernel, const string& path_to_kernel_file);
		void release_kernel(opencl_kernel* new_kernel);
		
		void read_gpu_limits(opencl_gpu_limits * gpu_limits = NULL);
		
        bool is_local_work_size_valid ( opencl_kernel * kernel_for_query );
        void show_kernel_info(opencl_kernel* kernel_for_query);
        void show_how_many_memory_is_allocated();

		void prepare_kernel(opencl_kernel* new_kernel, vector <gpu_array_base*> kernel_args);


		void prepare_kernel_argument (opencl_kernel* kernel_to_run, int argument_num, gpu_array_base * arg_to_prepare);
		void release_kernel_argument(gpu_array_base* arg_to_release);
		void transfer_kernel_argument_to_cpu(gpu_array_base * arg_to_transfer, bool perform_check = true);
		void transfer_kernel_argument_to_cpu(gpu_array_base * arg_to_transfer, void * host_ptr, bool perform_check = true);
		
        void transfer_kernel_argument_to_kernel_argument (gpu_array_base * src, gpu_array_base * dst);
		void transfer_kernel_argument_to_gpu(gpu_array_base * arg_to_transfer);
		void execute_kernel(opencl_kernel* kernel_to_execute);

		void allocate_kernel_argument(gpu_array_base * arg_to_allocate); // dev_alloc
		void determine_group_size(opencl_kernel * target_kernel);
	private:
        // FIXME in happy future move it in private gpu_limits field and operate with it
        cl_ulong device_max_work_group_size;
		static opencl_helper* opencl_helper_instance;

		// methods
		opencl_helper(profiling_level profiling);
		~opencl_helper();

		
		void set_default_values();

		void init_opencl(profiling_level profiling);
		
		void free_gpu_resources();

		static string get_kernel_name_from_file	(const string& path_to_kernel_file);
		static string get_kernel_source_from_file (const string& path_to_kernel_file);
		
		// data - todo in separate class
		cl_platform_id		cur_platform_id;
		cl_device_id		cur_device_id;
		cl_context		cur_context;
		cl_command_queue 	cur_command_queue;
		
		/* Used as return value for every call to OpenCL routines */
		cl_int			opencl_routine_return_value;

		int			profiling;
		int			num_of_dimensions;


};
#endif
