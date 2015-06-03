#define GENERATE_OPENCL_CODE
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

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
	*tsdf = (float) tsdf_value.value / (float) MAX_DEPTH_VALUE;
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
void scan_slice_of_both_volumes ( 
                                    __read_only     __global Point_Tsdf     * Tsdf_Volume,
                                    __read_only     __global Point_Color    * Color_Volume, 
                                    point_3i      Min_Bounds,
                                    point_3i      Max_Bounds,
                                    unsigned int Min_Size,
                                    unsigned int Max_Size,
                                    round_buffer	Round_Buffer,
                                    __write_only    __global    unsigned int    *   Predicats,
                                    __write_only __global unsigned int * Num_Of_Points_In_Slice
                                )
{

    size_t width = Round_Buffer.voxels_size.x;
    size_t height = Round_Buffer.voxels_size.y;

    int x = get_global_id(0);
    int y = get_global_id(1);

    if ( x > width || y > height )
        return;

	local size_t smem [ 256 /*CTA_SIZE*/];

	int tid = 	get_local_id(2) * get_local_size(0) * get_local_size(1) +
                get_local_id(1) * get_local_size(0) +
                get_local_id(0);	//flattenedThreadId

    int depth = Max_Size - Min_Size;

    for ( int z = Min_Size; z < Max_Size; z++ )
    {
        unsigned int cur_index = ( z % depth ) * width * height + y * width + x;


        /* clean data  */
        Predicats [ cur_index ] = 0;
        Predicats [ cur_index + width * height * depth ] = 0;
        Predicats [ cur_index + 2 * width * height * depth] = 0;
        Predicats [ cur_index + 3 * width * height * depth] = 0;
        int local_count = 0;

        bool in_black_zone = check_whether_it_within_the_black_zone ( x,y,z,  Min_Bounds, Max_Bounds, &Round_Buffer);

        barrier(CLK_LOCAL_MEM_FENCE);

		if ( in_black_zone )
		{

		    int W;
		    float F = fetch_tsdf_value (x, y, z, Tsdf_Volume, & Round_Buffer, &W);
		    if ( W != 0.f && F != 1.f ) {
    		    // process dx
    	    	if ( x + 1 <  width)
    		    {
    			    int Wn;
    			    float Fn = fetch_tsdf_value (x + 1, y, z, Tsdf_Volume, & Round_Buffer, &Wn);
    
    			    if ( 	(Wn != 0 && Fn != 1.f) && 
    			 	        ( (F > 0 && Fn < 0) || (F < 0 && Fn > 0) )
    			       )
    			    {
                        local_count++;
                        Predicats [ cur_index ] = 1;
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
                        local_count++;
                        Predicats [ cur_index + width * height * depth ] = 1;
                	}
                }                /*  if (y + 1 < VOLUME_Y) */
                
                // process dz
                if (z + 1 < Max_Size /* Round_Buffer.voxels_size.z */ ) // guaranteed by loop
                {
                	int Wn;
                	float Fn = fetch_tsdf_value (x, y, z + 1, Tsdf_Volume, & Round_Buffer, &Wn);
                
                	if ( 	(Wn != 0 && Fn != 1.f) && 
                		((F > 0 && Fn < 0) || (F < 0 && Fn > 0)) 
                	   )
                	{
                        local_count++;
                        Predicats [ cur_index + 2 * width * height * depth] = 1;
                	}
                } /* if (z + 1 < VOLUME_Z) */
            } /* if (W!=0 && F !=1)  */

            size_t index = 0;
            calculate_address ( x,y,z, &Round_Buffer, &index);
            
            Color current_point = Color_Volume [ index ].data;
            if ( Color_Point_Is_Set ( current_point ) ) {
                    local_count++;
                    Predicats [ cur_index + 3 * width * height * depth ] = 1;
            }
        }   /* in black zone  */

        /* to let other thread to run through all condition above */
        barrier(CLK_LOCAL_MEM_FENCE);
        smem[tid] = local_count;
        barrier(CLK_LOCAL_MEM_FENCE);
            
/*      REDUCE START */
        int  val =  smem [tid];
        
        if (tid < 128)  smem[tid] = val = val + smem[tid + 128]; barrier (CLK_LOCAL_MEM_FENCE);
        if (tid < 64)   smem[tid] = val = val + smem[tid +  64]; barrier (CLK_LOCAL_MEM_FENCE);
        if (tid < 32)   smem[tid] = val = val + smem[tid +  32]; barrier (CLK_LOCAL_MEM_FENCE);
        if (tid < 32)   smem[tid] = val = val + smem[tid +  16]; barrier (CLK_LOCAL_MEM_FENCE);
        if (tid < 32)   smem[tid] = val = val + smem[tid +   8]; barrier (CLK_LOCAL_MEM_FENCE);
        if (tid < 32)   smem[tid] = val = val + smem[tid +   4]; barrier (CLK_LOCAL_MEM_FENCE);
        if (tid < 32)   smem[tid] = val = val + smem[tid +   2]; barrier (CLK_LOCAL_MEM_FENCE);
        if (tid < 32)   smem[tid] = val = val + smem[tid +   1]; barrier (CLK_LOCAL_MEM_FENCE);
/*      REDUCE END    */

       if ( tid == 0 ) {
            atomic_add ( Num_Of_Points_In_Slice, smem[0]);
        }

    }
}
