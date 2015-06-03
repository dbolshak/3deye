#define GENERATE_OPENCL_CODE
#define INCLUDED_IN_IMPLEMENTATION_FILE

#include <elementary_points.h>
#include <intrinsics.h>
#include <base_point_types.h>
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
set_tsdf ( OPENCL_MEM_PTR Point_Tsdf * tsdf_value, float tsdf, int weight) 
{
	Tsdf new_value;
 	int fixedp = max (-MAX_DEPTH_VALUE, min (MAX_DEPTH_VALUE, ROUND_TOWARDS_ZERO (tsdf * MAX_DEPTH_VALUE)));
	new_value.value =  fixedp;
	new_value.weight = weight ;
	tsdf_value->data  = new_value;

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
get_tsdf_with_shift (  int X, 
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


point_3i
getVoxel (point_3f point, point_3f Cell_Size) 
{

	int vx = ROUND_TOWARDS_NEGATIVE_INFINITY ( point.x / Cell_Size.x);	// __float2int_rd         // round to negative infinity
	int vy = ROUND_TOWARDS_NEGATIVE_INFINITY ( point.y / Cell_Size.y);
	int vz = ROUND_TOWARDS_NEGATIVE_INFINITY ( point.z / Cell_Size.z);

	return make_point_3i (vx, vy, vz);
}

float
readTsdf ( OPENCL_MEM_PTR Point_Tsdf * Tsdf_Volume, /*OPENCL_CONST_PTR*/ const round_buffer * Tsdf_Buffer, int x, int y, int z) 
{
    
    size_t index = 0;
    calculate_address( x,y,z, Tsdf_Buffer, &index );

    float tsdf;
    int weight;

	Point_Tsdf cur_point = Tsdf_Volume [ index ];
	get_tsdf ( &cur_point, &tsdf, &weight );

    return tsdf;
}

float interpolateTrilineary (	OPENCL_MEM_PTR Point_Tsdf * Tsdf_Volume, 
                                point_3f point,
                                point_3f Cell_Size,
                                const round_buffer * Tsdf_Buffer
                            ) 
{
	point_3i g = getVoxel ( point, Cell_Size);

	if (g.x <= 0 || g.x >= Tsdf_Buffer->voxels_size.x - 1)
		return NAN;

	if (g.y <= 0 || g.y >= Tsdf_Buffer->voxels_size.y - 1)
		return NAN;

	if (g.z <= 0 || g.z >= Tsdf_Buffer->voxels_size.z - 1)
		return NAN;

	float vx = (g.x + 0.5f) * Cell_Size.x;
	float vy = (g.y + 0.5f) * Cell_Size.y;
	float vz = (g.z + 0.5f) * Cell_Size.z;

	g.x = (point.x < vx) ? (g.x - 1) : g.x;
	g.y = (point.y < vy) ? (g.y - 1) : g.y;
	g.z = (point.z < vz) ? (g.z - 1) : g.z;

	float a = point.x / Cell_Size.x- (g.x + 0.5f) ; if (a<0) { g.x--; a+=1.0f; };
	float b = point.y / Cell_Size.y- (g.y + 0.5f) ; if (b<0) { g.y--; b+=1.0f; };
	float c = point.z / Cell_Size.z- (g.z + 0.5f) ; if (c<0) { g.z--; c+=1.0f; };

          float res = (1 - a) * (
                                  (1 - b) * (
                                          readTsdf (Tsdf_Volume, Tsdf_Buffer,g.x + 0, g.y + 0, g.z + 0 ) * (1 - c) +
                                          readTsdf (Tsdf_Volume, Tsdf_Buffer,g.x + 0, g.y + 0, g.z + 1 ) *      c 
                                          )
                                  + b * (
                                          readTsdf (Tsdf_Volume, Tsdf_Buffer,g.x + 0, g.y + 1, g.z + 0 ) * (1 - c) +
                                          readTsdf (Tsdf_Volume, Tsdf_Buffer,g.x + 0, g.y + 1, g.z + 1 ) *      c  
                                          )
                                  )
                          + a * (
                                  (1 - b) * (
                                          readTsdf (Tsdf_Volume, Tsdf_Buffer,g.x + 1, g.y + 0, g.z + 0 ) * (1 - c) +
                                          readTsdf (Tsdf_Volume, Tsdf_Buffer,g.x + 1, g.y + 0, g.z + 1 ) *      c 
                                          )
                                  + b * (
                                          readTsdf (Tsdf_Volume, Tsdf_Buffer,g.x + 1, g.y + 1, g.z + 0 ) * (1 - c) +
                                          readTsdf (Tsdf_Volume, Tsdf_Buffer,g.x + 1, g.y + 1, g.z + 1 ) *      c 
                                          )
                                  );
	return res;
}

bool
checkInds (point_3i * g, point_3i Volume_Size_In_Voxels) 
{
	return (g->x >= 0 && g->y >= 0 && g->z >= 0 && g->x < Volume_Size_In_Voxels.x && g->y < Volume_Size_In_Voxels.y && g->z < Volume_Size_In_Voxels.z );
}

float
getMinTime ( const point_3f *volume_max, const point_3f *origin, const point_3f * dir)
{
	float txmin = ( (dir->x > 0 ? 0.f : volume_max->x) - origin->x) / dir->x;
	float tymin = ( (dir->y > 0 ? 0.f : volume_max->y) - origin->y) / dir->y;
	float tzmin = ( (dir->z > 0 ? 0.f : volume_max->z) - origin->z) / dir->z;
	
	return max ( max (txmin, tymin), tzmin);
}

float
getMaxTime ( const point_3f* volume_max, const point_3f* origin, const point_3f* dir)
{
	float txmax = ( (dir->x > 0 ? volume_max->x : 0.f) - origin->x) / dir->x;
	float tymax = ( (dir->y > 0 ? volume_max->y : 0.f) - origin->y) / dir->y;
	float tzmax = ( (dir->z > 0 ? volume_max->z : 0.f) - origin->z) / dir->z;

	return min (min (txmax, tymax), tzmax);
}

point_3f
get_ray_next (int x, int y, intrinsics intr) 
{
	point_3f ray_next;
	ray_next.x = (x - intr.cx) / intr.fx;
	ray_next.y = (y - intr.cy) / intr.fy;
	ray_next.z = 1;
	return ray_next;
}
__kernel void raycast(	__global    Point_Tsdf  *   Tsdf_Volume,
                        __write_only    __global    Point_XYZ  *   Vertex_Map,
                        __write_only    __global    Point_NXYZ *   Normal_Map,
                        point_3f       Translation,
                        matrix_3f      Rotation,
                        float          Truncation_Distance,
                        intrinsics     Intrinsics,
                        round_buffer	Tsdf_Buffer,
                        point_3f       Cell_Size
			)
{
	int x = get_global_id(0);
	int y = get_global_id(1);

    size_t  width = Intrinsics.cols;
    size_t  height = Intrinsics.rows;

	if ( x >= width || y >= height )	// FIXME I am not sure that such check is really necessary
		return;

    XYZ none = {NAN, NAN, NAN };

    Vertex_Map [ width * y + x ].data = none;
    Normal_Map [ width * y + x ].data = none;

    point_3i   Volume_Size_In_Voxels   =   Tsdf_Buffer.voxels_size;
    point_3f    Volume_Size_In_Meters   =   Tsdf_Buffer.volume_size;
	float time_step = Truncation_Distance *	0.8;

	point_3f ray_start          = Translation;
    point_3f ray_next_unknown   = get_ray_next (x, y, Intrinsics);
    point_3f ray_next_unknown_2 = multiply ( &Rotation, & ray_next_unknown );
	point_3f ray_next           = add_3f ( ray_next_unknown_2,  Translation);

    point_3f for_normalize = subtract_3f (ray_next, ray_start);
	point_3f ray_dir       = normalized_3f ( for_normalize );

	//ensure that it isn't a degenerate case
	ray_dir.x = (ray_dir.x == 0.f) ? 1e-15 : ray_dir.x;
	ray_dir.y = (ray_dir.y == 0.f) ? 1e-15 : ray_dir.y;
	ray_dir.z = (ray_dir.z == 0.f) ? 1e-15 : ray_dir.z;

	// compute time when entry and exit volume
	float time_start_volume = getMinTime (&Volume_Size_In_Meters, &ray_start, &ray_dir);
	float time_exit_volume  = getMaxTime (&Volume_Size_In_Meters, &ray_start, &ray_dir);

	const float min_dist = 0.f;         //in meters
	time_start_volume = max (time_start_volume, min_dist);
	if (time_start_volume >= time_exit_volume)
        return;

	float time_curr = time_start_volume;

	point_3f ray_dir_at_current_time = ray_dir;
	ray_dir_at_current_time.x *= time_curr;
	ray_dir_at_current_time.y *= time_curr;
	ray_dir_at_current_time.z *= time_curr;


    point_3f ray_unknown = 	add_3f ( ray_start, ray_dir_at_current_time );
	point_3i g = getVoxel ( ray_unknown , Cell_Size );
    // FIXME - for uknown reason without explicit cast opencl can't choose appropriate function, based on data type 
	g.x = max (0, (int) min ( (int) g.x, (int) Volume_Size_In_Voxels.x - 1) );
	g.y = max (0, (int) min ( (int) g.y, (int) Volume_Size_In_Voxels.y - 1) );
	g.z = max (0, (int) min ( (int) g.z, (int) Volume_Size_In_Voxels.z - 1) );

	float tsdf = readTsdf (Tsdf_Volume, &Tsdf_Buffer, g.x, g.y, g.z);

	//infinite loop guard
	const float max_time = 3 * (Volume_Size_In_Meters.x + Volume_Size_In_Meters.y + Volume_Size_In_Meters.z);


	for (; time_curr < max_time; time_curr += time_step)
	{
        
		float tsdf_prev = tsdf;

		ray_dir_at_current_time  = ray_dir;

		float current_time = time_curr + time_step;

		ray_dir_at_current_time.x *=current_time; 
		ray_dir_at_current_time.y *=current_time; 
		ray_dir_at_current_time.z *=current_time; 

        point_3f ray_unknown_3 = add_3f (ray_start ,ray_dir_at_current_time );

		point_3i g = getVoxel ( ray_unknown_3 , Cell_Size);
        

		if (!checkInds (&g, Volume_Size_In_Voxels))
			break;

		tsdf = readTsdf (Tsdf_Volume, & Tsdf_Buffer /* Volume_Size_In_Voxels*/, g.x, g.y, g.z);

		if (tsdf_prev < 0.f && tsdf > 0.f)
			break;

		if (tsdf_prev > 0.f && tsdf < 0.f)           //zero crossing
		{
            
            point_3f p1 = add_3f ( ray_start ,ray_dir_at_current_time );
			float Ftdt = interpolateTrilineary (	Tsdf_Volume, 
								p1 ,
								Cell_Size,
								& Tsdf_Buffer
                                );
			
			if (isnan (Ftdt))
				break;
			
			ray_dir_at_current_time  = ray_dir;
			ray_dir_at_current_time.x *=time_curr; 
			ray_dir_at_current_time.y *=time_curr; 
			ray_dir_at_current_time.z *=time_curr; 

            point_3f p2 = add_3f (ray_start,ray_dir_at_current_time );

			float Ft = interpolateTrilineary (	Tsdf_Volume,
								p2,
								Cell_Size,
								& Tsdf_Buffer
                                 );
			if (isnan (Ft))
				break;
			float Ts = time_curr - time_step * Ft / (Ftdt - Ft);

			ray_dir_at_current_time  = ray_dir;
			ray_dir_at_current_time.x *=Ts; 
			ray_dir_at_current_time.y *=Ts; 
			ray_dir_at_current_time.z *=Ts; 
			
			point_3f vetex_found = add_3f ( ray_start ,ray_dir_at_current_time );

			Point_XYZ vertex_result;
			vertex_result.data.x = vetex_found.x;
			vertex_result.data.y = vetex_found.y;
			vertex_result.data.z = vetex_found.z;
		
            //      Save Vertex Data            
            Vertex_Map [ width * y + x ] = vertex_result;

			ray_dir_at_current_time  = ray_dir;
			ray_dir_at_current_time.x *=time_curr; 
			ray_dir_at_current_time.y *=time_curr; 
			ray_dir_at_current_time.z *=time_curr; 

            point_3f p3 = add_3f( ray_start ,ray_dir_at_current_time );

			point_3i g = getVoxel (  p3 , Cell_Size);
			if (g.x > 1 && g.y > 1 && g.z > 1 && g.x < Volume_Size_In_Voxels.x - 2 && g.y < Volume_Size_In_Voxels.y - 2 && g.z < Volume_Size_In_Voxels.z - 2)
			{
				point_3f t;
				point_3f n;

				t = vetex_found;
				t.x += Cell_Size.x;
				float Fx1 = interpolateTrilineary (	Tsdf_Volume,
									t,
									Cell_Size,
									& Tsdf_Buffer
                                    );

				t = vetex_found;
				t.x -= Cell_Size.x;
				float Fx2 = interpolateTrilineary (	Tsdf_Volume,
									t,
									Cell_Size,
									& Tsdf_Buffer
                                     );

				n.x = (Fx1 - Fx2);

				t = vetex_found;
				t.y += Cell_Size.y;
				float Fy1 = interpolateTrilineary (	Tsdf_Volume,
									t,
									Cell_Size,
									& Tsdf_Buffer
                                    );

				t = vetex_found;
				t.y -= Cell_Size.y;
				float Fy2 = interpolateTrilineary (	Tsdf_Volume,
									t,
									Cell_Size,
									& Tsdf_Buffer
                                    );

				n.y = (Fy1 - Fy2);

				t = vetex_found;
				t.z += Cell_Size.z;
				float Fz1 = interpolateTrilineary (	Tsdf_Volume,
									t,
									Cell_Size,
									& Tsdf_Buffer
                                    );

				t = vetex_found;
				t.z -= Cell_Size.z;
				float Fz2 = interpolateTrilineary (	Tsdf_Volume,
									t,
									Cell_Size,
									& Tsdf_Buffer
                                    );

				n.z = (Fz1 - Fz2);

				n = normalized_3f (n);

				Point_NXYZ normal_result;
				normal_result.data.x = n.x;
				normal_result.data.y = n.y;
				normal_result.data.z = n.z;
            
                Normal_Map [ y * width + x ] = normal_result;

			}
			break;
		}	// zero crossing

	} 
}
