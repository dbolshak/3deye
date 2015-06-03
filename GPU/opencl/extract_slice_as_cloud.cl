#define GENERATE_OPENCL_CODE

#include <elementary_points.h>
#include <base_point_types.h>
#include <intrinsics.h>
#include <round_buffer.h>

void
get_tsdf ( Point_Tsdf * point_tsdf_value, float* tsdf, int* weight)
{
	Tsdf tsdf_value;
	tsdf_value = point_tsdf_value->data;
	*weight = tsdf_value.weight ;
	*tsdf = (float) tsdf_value.value / (float) MAX_DEPTH_VALUE; // __int2float_rn
}
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
float
fetch_tsdf_value (  int X, 
                    int Y, 
                    int Z, 
                    OPENCL_READ_ONLY_MEM_PTR Point_Tsdf * Tsdf_Volume, 
                    const round_buffer * Tsdf_Buffer, 
                    int * weight)
{
	size_t Address_For_Read;
	calculate_address ( X,Y,Z, Tsdf_Buffer, &Address_For_Read );

	float tsdf;
	Point_Tsdf cur_point = Tsdf_Volume [ Address_For_Read ];
	get_tsdf ( & cur_point, & tsdf, weight );

	return tsdf;
}
bool
Color_Point_Is_Set ( Color cvr )
{ 
    return (cvr.r != 0 || cvr.g != 0 || cvr.b !=0 || cvr.w !=0 );
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

__kernel
void extract_slice_as_cloud (
                            __read_only     __global unsigned int   * Predicats,
                            __read_only     __global Point_Tsdf     * Tsdf_Volume,
                            __read_only     __global Point_Color    * Color_Volume, 
                            unsigned int    Min_Size,
                            unsigned int    Max_Size,
                            point_3f        Cell_Size,
						    point_3i      Min_Bounds,
						    point_3i      Max_Bounds,
                            round_buffer    Round_Buffer,
                            __write_only    __global Point_XYZI * Points_Vertexes,
                            __write_only    __global Point_Color * Points_Color
                            )
{
    size_t width = Round_Buffer.voxels_size.x;
    size_t height = Round_Buffer.voxels_size.y;

    int x = get_global_id(0);
    int y = get_global_id(1);

    if ( x > width || y > height )
        return;

    int depth = Max_Size - Min_Size;

	point_3f V;
	V.x = (x + 0.5f) * Cell_Size.x;
	V.y = (y + 0.5f) * Cell_Size.y;

    for ( int z = Min_Size; z < Max_Size; z++ )
    {
        bool in_black_zone = check_whether_it_within_the_black_zone ( x, y, z, Min_Bounds, Max_Bounds, &Round_Buffer );
		if ( in_black_zone )
		{
            bool set_any_vertex = false;
		    int W;
		    float F = fetch_tsdf_value (x, y, z, Tsdf_Volume, &Round_Buffer, &W);
		     if ( W != 0.f && F != 1.f ) {
    		    
                V.z = (z + 0.5f) * Cell_Size.z;
    		    
                // process dx
    	    	if ( x + 1 <  width)
    		    {
    			    int Wn;
    			    float Fn = fetch_tsdf_value (x + 1, y, z, Tsdf_Volume, &Round_Buffer, &Wn);
    
    			    if ( 	(Wn != 0 && Fn != 1.f) && 
    			 	    ( (F > 0 && Fn < 0) || (F < 0 && Fn > 0) )
    			        )
    			    {
                XYZI result = { x+1, y, z, 1.f };
                        Points_Vertexes [ Predicats [ ( z % depth ) * width * height + y * width + x ] ].data = result;
                        size_t index = 0;
                        calculate_address ( x+1,y,z, &Round_Buffer, &index );
                        
                        Color current_point = Color_Volume [ index ].data;
                        Points_Color [ Predicats [ ( z % depth ) * width * height + y * width + x ] ].data = current_point;

                        set_any_vertex = true;
    			    }
    		    }               /* if (x + 1 < VOLUME_X) */
    
                // process dy
                if ( y + 1 < height )
                {
                	int Wn;
                	float Fn = fetch_tsdf_value (x, y + 1, z, Tsdf_Volume, &Round_Buffer, &Wn);
                
                	if (	(Wn != 0 && Fn != 1.f) && 
                		( (F > 0 && Fn < 0) || (F < 0 && Fn > 0))  
                	   )
                	{
                XYZI result = { x, y+1, z, 1.f };
                        Points_Vertexes [ Predicats [ ( z % depth ) * width * height + y * width + x + width * height * depth ] ].data = result;

                        // check Color volume
                        size_t index = 0;
                        calculate_address ( x,y+1,z, &Round_Buffer, &index );
                        
                        Color current_point = Color_Volume [ index ].data;
                        Points_Color [ Predicats [ ( z % depth ) * width * height + y * width + x + width * height * depth ] ].data = current_point;

                        set_any_vertex = true;
                	}
                }                /*  if (y + 1 < VOLUME_Y) */
                
                // process dz
                if (z + 1 < Round_Buffer.voxels_size.z) // guaranteed by loop
                {
                	int Wn;
                	float Fn = fetch_tsdf_value (x, y, z + 1, Tsdf_Volume, &Round_Buffer, &Wn);
                
                	if ( 	(Wn != 0 && Fn != 1.f) && 
                		((F > 0 && Fn < 0) || (F < 0 && Fn > 0)) 
                	   )
                	{

                XYZI result = { x, y, z+1, 1.f };
                        Points_Vertexes [ Predicats [ ( z % depth ) * width * height + y * width + x + 2 * width * height * depth] ].data = result;
                        // check Color volume           
                        size_t index = 0;
                        calculate_address ( x,y,z+1, &Round_Buffer, &index );

                        Color current_point = Color_Volume [ index ].data;
                        Points_Color [ Predicats [ ( z % depth ) * width * height + y * width + x + 2 * width * height * depth ] ].data = current_point;

                        set_any_vertex = true;
                	}
                } /* if (z + 1 < VOLUME_Z) */
            } /* if (W!=0 && F !=1)  */


            // TODO FIXME - in case we do not have color here
            // we can get it from nearest neighbor or so
            
            size_t index = 0;
            calculate_address ( x,y,z, &Round_Buffer, &index );

            Color current_point = Color_Volume [ index ].data;
            if ( Color_Point_Is_Set ( current_point ) )
            {
                XYZI result = { x, y, z, 1.f };
                Points_Color [ Predicats [ ( z % depth ) * width * height + y * width + x + 3 * width * height * depth ] ].data = current_point;
                Points_Vertexes [ Predicats [ ( z % depth ) * width * height + y * width + x   + 3 * width * height * depth ] ].data = result;
            }

        }   /* in black zone  */
    }
}