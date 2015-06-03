// FIXME - move lines below into separate headers and include it at every *.cl source
#define GENERATE_OPENCL_CODE

#include <elementary_points.h>
#include <base_point_types.h>
#include <intrinsics.h>
#include <round_buffer.h>

void
calculate_address ( int x, int y, int z, const round_buffer * Tsdf_Buffer, size_t *Address_For_Update)
{

	point_3i resulting_pos;
	resulting_pos.x = (x + Tsdf_Buffer->origin_GRID.x) % Tsdf_Buffer->voxels_size.x;
	resulting_pos.y = (y + Tsdf_Buffer->origin_GRID.y) % Tsdf_Buffer->voxels_size.y;
	resulting_pos.z = (z + Tsdf_Buffer->origin_GRID.z) % Tsdf_Buffer->voxels_size.z;

	*Address_For_Update = 	Tsdf_Buffer->voxels_size.x*Tsdf_Buffer->voxels_size.y * resulting_pos.z +
				Tsdf_Buffer->voxels_size.x * resulting_pos.y +
				resulting_pos.x;

    // take into account posibility of shift:
    *Address_For_Update += Tsdf_Buffer->start_index;
    if ( *Address_For_Update > Tsdf_Buffer->buffer_length )
        *Address_For_Update %= Tsdf_Buffer->buffer_length;

}

bool
check_whether_it_within_the_black_zone ( int x, int y, int z, point_3i Min_Bounds, point_3i Max_Bounds, round_buffer * Round_Buffer )
{
    int idX = x + Round_Buffer->origin_GRID.x;
    int idY = y + Round_Buffer->origin_GRID.y;
    int idZ = z + Round_Buffer->origin_GRID.z;

    return      (  ( idX >= Min_Bounds.x && idX <= Max_Bounds.x ) || 
                   ( idY >= Min_Bounds.y && idY <= Max_Bounds.y ) ||
                   ( idZ >= Min_Bounds.z && idZ <= Max_Bounds.z )
                );
}
__kernel void clean_color_slice( 	__global Point_Color * Color_Volume,
					point_3i minBounds,
					point_3i maxBounds,
					round_buffer Color_Buffer
				)
{
	int x = get_global_id(0);
	int y = get_global_id(1);


	if ( ( y >= Color_Buffer.voxels_size.y ) || ( x >= Color_Buffer.voxels_size.x ) )
		return;

    int depth = Color_Buffer.voxels_size.z;

    size_t Address_For_Update;
    for (int z = 0; z < depth; z++) {
	if ( check_whether_it_within_the_black_zone  ( x, y, z, minBounds, maxBounds, & Color_Buffer ) ) {
				calculate_address ( x, y, z, & Color_Buffer, &Address_For_Update);

				Color_Volume[Address_For_Update].data.r= 0;
				Color_Volume[Address_For_Update].data.g= 0;
				Color_Volume[Address_For_Update].data.b= 0;
				Color_Volume[Address_For_Update].data.w= 0;
            }
     }
}
