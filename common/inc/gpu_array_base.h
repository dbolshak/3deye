#ifndef GPU_ARRAY_BASE 
#define GPU_ARRAY_BASE

#include <constants.h>
#include <dim3.h>
/*
	In order to implement fully generic container of derived I need to define 
	a base class with virtual interface to rely on in derived classes.

	after first compilation fail I will try to use this class as a common storage
for all unrelated to particular data entities related to internals of cpu-gpu interaction

	probably you should delete redundant virtual modifiers

*/

class gpu_array_base
{
	protected:
		cl_mem              device_ptr;
		gpu_transfer_type   data_transfer_direction;
		gpu_data_type       data_type;
		dim3                sizes;	// by default - 0 - width, 1 - height, 2 - depth
		unsigned int        total_size;

        /*  for manipulating user controled device mem ptr */
        bool                need_to_prepare;
        gpu_mem_type        mem_type;

	public:
		gpu_array_base() {};
		virtual ~gpu_array_base(); 

		/* common interface */

        /* setters \ getters  */

		virtual gpu_mem_type 
		get_arg_mem_type() const;

		virtual void
		set_arg_mem_type ( gpu_mem_type new_mem_type );

		virtual void
		set_data_transfer_type( gpu_transfer_type new_type);

		virtual gpu_transfer_type 
		get_data_transfer_type() const;

		virtual void
		set_data_type ( gpu_data_type initial_data_type );

		virtual void
		set_data_type_by_size ( );
		
		virtual gpu_data_type
		get_data_type() const;

		virtual void 
		set_prepare_status ( bool prepare_status );

		virtual unsigned int get_total_size () const;
        virtual bool        should_be_prepared() const;
		
		//              POINTERS ROUTINE 
        
		virtual 
        void*	get_host_ptr ( ) = 0;

		virtual 
        void	set_host_ptr ( void * initial_host_ptr, dim3 & new_data_size) = 0;
		virtual 
        void	set_host_ptr ( void * initial_host_ptr ) = 0;

		virtual void
		set_device_ptr ( cl_mem & initial_device_ptr );

		virtual cl_mem * 
		get_device_ptr ();

        virtual void    copy_to ( gpu_array_base * dst);

		virtual void    memcpy_from_dev ( void * host_dst) ;
        virtual void    memcpy_to_dev   ( void * host_src );

		virtual void call_allocate_and_transfer ( bool need_to_transfer_data_to_gpu );

        virtual void    memcpy_from_host ( void* dump_ptr  ) = 0; 

        virtual void    memcpy_to_host ( void * host_ptr ) = 0;

		virtual void    dev_alloc ();
		virtual void	dev_free ();

		virtual void reset_gpu_part ();


		//          SIZE and DIMENSIONS ROUTINE

       	/** \brief return total amount of (!)BYTES(!) to manipulate with memory pointer 
       	 *  FIXME - vulnerable to overflow
       	 * */


		virtual unsigned int
		width () const;

		virtual unsigned int
		height () const;

		virtual unsigned int
		depth () const;

		virtual dim3
		get_size() const;

		virtual void
		set_size ( const dim3 & new_data_size );

        virtual size_t
        elem_size() const = 0;

};

#endif
