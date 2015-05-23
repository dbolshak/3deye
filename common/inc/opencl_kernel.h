#ifndef OPENCL_KERNEL
#define OPENCL_KERNEL

#include <opencl_utils.h>	// for OpenCL-related data-types
#include <gpu_array.h>

#include <vector>
using std::vector;

class opencl_kernel
{
	private:
		vector < gpu_array_base  *> kernel_args;
		
		// which is used in conjunction with 
		// singletone opencl_helper
		cl_program 		program;
		cl_kernel 		kernel;
		string			kernel_name;


        dim3		global_work_size; // 0 - mean no dimension
	// ie 1, 0,0 - 1 dimension
	// 512, 1024 - 2 dimensions
        dim3		local_work_size;
		
		// you shoud never call them
		// at present
	
		// copy constructor
		opencl_kernel (const opencl_kernel& another_object);
		// operator=
		opencl_kernel & operator=(const opencl_kernel& rhs);

	public:
		// because I do not want that user can create such objects	
		explicit opencl_kernel(const string & path_to_kernel_file, profiling_level profiling = DISABLE_PROFILING );
		~opencl_kernel();

		const cl_kernel		get_kernel();
		const cl_program	get_program();
		const string		get_name();

        void show_kernel_info();
        static  void    show_mem_usage();

        void    execute( const size_t * new_global_wotrk_size, const size_t * new_local_work_size );
		void	execute(const size_t * initial_global_work_size, bool Try_To_Determine_Group_Sizes = false);
		void	execute(size_t width, size_t height, size_t depth, bool Try_To_Determine_Group_Sizes = false);

		void	prepare();
		void	fill_hosts_data ();
		
		//	you never should call it independenly of opencl_helper	
		//	FIXME - bad design
		void	set_kernel ( cl_kernel init_kernel );
		void	set_program( cl_program init_program );
		void	set_name ( const string	& kernel_name);

		void	set_global_work_size(size_t width, size_t height, size_t depth);
		/*size_t* */dim3	get_global_work_size();
		void	set_global_work_size(const size_t* initial_global_work_size);
		int	get_num_dimensions();
// FIXME delete _Work_ from all func ?
		void	set_local_work_size(const size_t *initial_local_work_size);
		void	set_local_work_size(size_t width, size_t height, size_t depth);
		/*size_t* */dim3	get_local_work_size();
	/** \brief general purpose routine */
	template < typename T>
	void
	add_kernel_args ( T * arg )
	{
		add_kernel_arg ( arg );
	};

	template <typename T, typename ...Args>
	void
	add_kernel_args( T * arg, Args ... args )
	{
		add_kernel_arg ( arg );
		add_kernel_args ( args ... );
	};
	


        /** \brief used for small scalars only!  */
        template<class T>
        void add_kernel_arg(    T * host_data, 
                                gpu_transfer_type data_transfer_direction = IN_GPU_USE_ONLY 
                                ) {
			gpu_array_base* new_arg = new gpu_array<T> (	host_data );
			kernel_args.push_back( new_arg );

        };

	/*	
		FIXME 07082014

		void	Add_Kernel_Arg( void * host_data,
                                const size_t * host_data_size,
                                gpu_mem_type mem_type,
                                gpu_transfer_type data_transfer_direction = TO_GPU,
                                gpu_data_type gpu_data_type = BUFFER_1D );

		void	Add_Kernel_Arg( void * host_data,
                                const size_t * host_data_size,
                                gpu_transfer_type data_transfer_direction = TO_GPU,
                                gpu_data_type gpu_data_type = BUFFER_1D );
*/
		template <class T>
		void	add_kernel_arg( T * host_data,
                                dim3 host_data_size,
                                gpu_mem_type mem_type,
                                gpu_transfer_type data_transfer_direction = TO_GPU
                                /*gpu_data_type gpu_data_type = BUFFER_1D */)
		{
			gpu_array_base* new_arg = new gpu_array<T> (  host_data, 
                                                                    host_data_size,
                                                                    mem_type,
                                                                    data_transfer_direction/*,
                                                                    gpu_data_type*/
                                                                 );
			kernel_args.push_back( new_arg );

		};

		template <class T>
		void	add_kernel_arg( T * host_data,
                                dim3 host_data_size,
                                gpu_transfer_type data_transfer_direction = TO_GPU
                              )
		{
			
			gpu_array_base* new_arg = new gpu_array<T> (host_data, host_data_size, data_transfer_direction );
			kernel_args.push_back( new_arg );
		};

        void    add_kernel_arg( gpu_array_base * new_kernel_argument );
		
		void	run();
        
        void    free_args();
        void    show_args_info();

        template <class T>
        void run_templated ();

};

#endif
