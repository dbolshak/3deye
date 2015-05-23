#ifndef ROUND_BUFFER
#define ROUND_BUFFER

#include <constants.h>  /* point_* definition from elemetary_point.h and macros def*/

/** \brief Structure to handle buffer addresses 

memory representation
 0 1 2 3 ...
 _ _ _ _
|_|_|_|_|     0
|_|_|_|_|     1
|_|_|_|_|     2
|_|_|_|_|     3
             ...

be default - origin is ( 0,0 )
but when buffer is close to overflowing
first data is pushed from GPU,
origin is changed, and new data is written 
to first slice, but origin is re-calculated
so it would be (1,1)
*/

typedef struct round_buffer
{
    /* common for both volume - Color and Points 
     * do not need to create complex solution for updating it at every shift
     * just update cpu-version and transfer to gpu
     * */
    int start_index;
    int end_index;
    int buffer_length;

	/** \brief Internal cube origin for rolling buffer.*/
	point_3i origin_GRID; // NOTE changed after shift
	
	/** \brief Cube origin in world coordinates.*/
	point_3f origin_GRID_global; // NOTE changed after shift
	
	/** \brief Current metric origin of the cube, in world coordinates.*/ 
	point_3f origin_metric;
	
	/** \brief Size of the volume, in meters. Default - 3.0,3.0,3.0 */
	point_3f volume_size;
	
	/** \brief Number of voxels in the volume, per axis. Default - 512, 512, 512 */
	point_3i  voxels_size;

#ifndef GENERATE_OPENCL_CODE
	void set_default_values ();
	void set_voxels_size (size_t new_width, size_t new_height, size_t new_depth );
	void set_memory_index ();
	round_buffer();
	round_buffer(const size_t * dimensions);
	round_buffer(const dim3 & dimensions);
	round_buffer(size_t voxel_metric_size_width, size_t voxel_metric_size_heigh, size_t voxel_metric_size_depth);
	round_buffer (point_3i voxels_size, point_3f metric_size );
	friend istream & operator>> ( istream & in, round_buffer & object_instance );
	friend ostream & operator<< ( ostream & out, const round_buffer & object_instance );
#endif

} round_buffer;
#endif // ROUND_BUFFER
