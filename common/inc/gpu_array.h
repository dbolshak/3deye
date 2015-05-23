#ifndef KERNEL_ARGUMENT
#define KERNEL_ARGUMENT

#include <opencl_utils.h>
#include <gpu_array_base.h>

/*
	FIXME
		1. gpu_array<T> constructor should use dim3 &
		and NOT by value dim3
		2. implicit conversion dim3 to int-like types
*/

/*
 *  I intentionally use public inheritance due to the fact 
 *  that I intensely use pointer to base to refer to an object
 *  of derived type. Check:
 *  http://stackoverflow.com/questions/9661936/inheritance-a-is-an-inaccessible-base-of-b  
 *
 * */
template <class T>
class gpu_array : public gpu_array_base
{
    friend class opencl_helper;
	// FIXME made all class private
	// or made it internal for opencl_kernel?
	public:

        /** \brief common constructor for 
         *  TODO     which types of kernel parameters
         *  NOTE    gpu_data_type is evaluated based on host_data_size
         * */
        gpu_array ( T * host_data,
                    dim3 host_data_size,
                    gpu_transfer_type data_transfer_direction = TO_GPU);
                    
        /** \brief  common constructor for
         *  TODO which types of kernel parameters
         *
         * */
        gpu_array ( T * host_data,
                    dim3 host_data_size,
                    bool should_allocate_and_transfer,
                    gpu_transfer_type data_transfer_direction = IN_GPU_USE_ONLY);
        
        /** \brief  common constructor for
         *  TODO which types of kernel parameters
         *
         * */
        gpu_array ( dim3 host_data_size,
                    bool should_allocate = true,
                    gpu_transfer_type data_transfer_direction = IN_GPU_USE_ONLY );

        gpu_array ( unsigned int host_data_size,
                    bool should_allocate = true,
                    gpu_transfer_type data_transfer_direction = IN_GPU_USE_ONLY );

        /** \brief this constructor used for create private args for kernels aka scalar */
        /*
         *      If it is call with arg_mem_type not PRIVATE - throw exception
         * */
        gpu_array ( T * host_data,
                    dim3 host_data_size,
                    gpu_mem_type arg_mem_type,
                    gpu_transfer_type data_transfer_direction = IN_GPU_USE_ONLY);

        gpu_array ( T * host_data,
                    gpu_transfer_type data_transfer_direction = IN_GPU_USE_ONLY);

        /** \brief this constructor used for create shared args for kernels aka local */
        gpu_array ( dim3 host_data_size,
                    gpu_mem_type arg_mem_type,
                    gpu_transfer_type data_transfer_direction = IN_GPU_USE_ONLY);
		
        ~gpu_array();   // actually all work is performed by base destructor


        //              POINTERS ROUTINE 
		// NOTE - override method from base class
		// http://www.parashift.com/c++-faq/hiding-rule.html
		// http://nerdland.net/2009/06/covariant-templatized-virtual-copy-constructors/
		void*	get_host_ptr () { return get_host_ptr_impl (); };
		T*	get_host_ptr_impl ();

        // original interface
        void    set_host_ptr ( void* ptr ) { set_host_ptr_impl ( (T*) ptr);  };
        void	set_host_ptr ( void * initial_host_ptr, dim3 & new_data_size)
        { set_host_ptr_impl ( (T*) initial_host_ptr, new_data_size); } ;

		void	set_host_ptr_impl (T* initial_host_ptr, dim3 & new_data_size);
		void	set_host_ptr_impl(T* initial_host_ptr );


        //          SIZE and DIMENSIONS ROUTINE

        /** \brief return total amount of (!)BYTES(!) to manipulate with memory pointer 
         *  FIXME - vulnerable to overflow
         * */

        /** \brief return size of data in bytes */
        unsigned int    get_total_size () const;
        //      MEMORY ROUTINE

        void    memcpy_from_host ( void * host_ptr ) {
	        set_host_ptr ( (T*) host_ptr );
            memcpy_to_dev ( host_ptr );
        };
        void    memcpy_from_host_impl (T * host_ptr );

        void memcpy_to_host ( void * host_dst ) {
            memcpy_to_host ( (T*) host_dst) ; 
        };
        void memcpy_to_host ( T * host_dst );

        size_t elem_size() const { 
            return sizeof(T);
        };

    private:

        //          INTERNAL    HELPERS
        
	void
	create_and_set_gpu_only   (  
                                T * host_data, 
                                dim3 & data_size,
                                bool allocate_and_transfer,
                                gpu_transfer_type data_transfer_direction );

	void
	create_and_set_private_only (
                                T * host_data, 
                                gpu_transfer_type data_transfer_direction,
                            	gpu_mem_type mem_type );

		//		TODO
		// FIXME - would be great if it can determine transfer_direction / gpu_data_type automatically
		// 07072014 gpu_array (void * host_data, size_t * host_data_size );
		// so no object can be created on stack - only heap allocation
		// probably lifecycle should be managable only by opencl_kernel/opencl_helper?
		// the only valid way to use it - use public constructor
		gpu_array ( const gpu_array<T> & other );
		gpu_array ();
		// add here operator=(opencl_kernel_argument & ... )

		//			DATA
        /** \brief host-side pointer to external data,
         *          managment (ie [de-]allocation )  of this data is NOT responsibility 
         *          of gpu_array class - it is responsibility of client's code!
         * */
		T	*host_ptr;
		
			
		// FIXME 
		// add routine to convert this obj to user's object, using (ptr_, method - functor) as param? 
};

#include <gpu_array.hpp>

#endif // gpu_array<T>
