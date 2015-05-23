#include <round_buffer.h>
#include <string_utils.h>
#include <sstream>

void
round_buffer :: set_default_values ()
{
    origin_GRID         =   DEFAULT_VOLUME_VOXEL_ORIGIN;
    origin_GRID_global  =   DEFAULT_VOLUME_METRIC_ORIGIN;
    origin_metric       =   origin_GRID_global;
    voxels_size         =   DEFAULT_VOLUME_VOXELS_SIZE;
    volume_size         =   DEFAULT_VOLUME_METRIC_SIZE;

    set_memory_index ();
}
void
round_buffer :: set_voxels_size (size_t new_width, size_t new_height, size_t new_depth )
{
    voxels_size.x = new_width;
    voxels_size.y = new_height;
    voxels_size.z = new_depth;
}

round_buffer::round_buffer()
{
    set_default_values();
}

/*      Blindly set mem_max, mem_min and start_point indexes
 *      according to voxels_dimensions
 *
 * */
void
round_buffer :: set_memory_index ()
{
    start_index     =  0;
    buffer_length   =  voxels_size.x * 
                        voxels_size.y *
                        voxels_size.z ;
}

round_buffer::round_buffer(size_t   volume_voxels_size_width, size_t volume_voxels_size_height, size_t volume_voxels_size_depth )
{
    set_default_values();
    set_voxels_size (
                        volume_voxels_size_width,
                        volume_voxels_size_height,
                        volume_voxels_size_depth
                    );
    set_memory_index ();
}

round_buffer :: round_buffer ( const size_t * dimensions )
{
    set_default_values();
    set_voxels_size ( dimensions[WIDTH], dimensions[HEIGHT], dimensions[DEPTH] );
    set_memory_index ();
}

round_buffer :: round_buffer ( const dim3 & dimensions )
{
    set_default_values();
    set_voxels_size ( dimensions.width, dimensions.height, dimensions.depth );
    set_memory_index ();
}

round_buffer :: round_buffer ( point_3i new_voxels_size, point_3f new_metric_size ) {

    set_default_values();

    voxels_size = new_voxels_size;
    volume_size = new_metric_size;
    set_memory_index ();
}

istream &
operator >> ( istream & in, round_buffer & obj )
{
    string tmp_str = "";
	in >> tmp_str;
	replaceAll(tmp_str, ROUND_BUFFER_DELIMITER, " ");
    
    /*  
     *  string format should be
     *  Tsdf_Value-Tsdf_Weight
     *  where 
     *  Tsdf_Value in range ()
     *  Tsdf_Weight in range ()
     *  and '-' - is TSDF_DELIMITER
    */
// FIXME use stream operator for point_* structs !
	istringstream in_tmp(tmp_str);
    in_tmp  >> obj.origin_GRID >> obj.origin_GRID_global >>
               obj.origin_metric >> obj.volume_size >>
               obj.voxels_size;
    obj.set_memory_index();
    return in;
}

ostream &
operator << ( ostream & out, const round_buffer & obj )
{
    out << obj.origin_GRID          << ROUND_BUFFER_DELIMITER 
        << obj.origin_GRID_global   << ROUND_BUFFER_DELIMITER
        << obj.origin_metric        << ROUND_BUFFER_DELIMITER
        << obj.volume_size          << ROUND_BUFFER_DELIMITER
        << obj.voxels_size          << ROUND_BUFFER_DELIMITER
        << obj.start_index          << ROUND_BUFFER_DELIMITER
        << obj.buffer_length        << ROUND_BUFFER_DELIMITER
        << " ";

    return out;
}
