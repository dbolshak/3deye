#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#define GENERATE_OPENCL_CODE
#define INCLUDED_IN_IMPLEMENTATION_FILE

#include <elementary_points.h>
#include <base_point_types.h>
#include <constants.h>

bool Is_Depth_Valid ( float depth )
{
    // NOTE 32767 - max ushort,
    // when you cast ushort to float - you just multiply 0.001
    // that's why I use here 35 as a high boundary
    return ( depth > 0 && depth < 35.f );
}

bool Whether_Point_Within_Img ( point_2i p, point_2i img_sz )
{
    return (    p.x >= 0 && p.x < img_sz.x &&
                p.y >= 0 && p.y < img_sz.y );

}

/*
    NOTE:
            R', t' should be recalculated for every pyramid level
*/
__kernel
void    scan_list_of_valid_correspondence ( /* In params */
                                            __read_only __global    Point_Intensity * I_Prev,
                                            __read_only __global    float   *   D_Prev,
                                            __read_only __global    Point_Intensity * I_Curr,
                                            __read_only __global    float   *   D_Curr,
                                            __read_only __global    point_2i *  List_Of_Interest_Points,
                                            int Num_Of_Interest_Point,
                                            matrix_3f R, /* actually this is R' = K * Rn * K^(-1)  */
                                            vector_3f t, /* actually this is t' = K * tn */
                                            point_2i   img_size,
                                            float   Depth_Threshold,
                                            /* Out params  */
                                            
                                            __write_only __global   int * Num_Of_Correspondence,
                                            __write_only __global   float * Total_Diff, /* Note due to absence in OpenCL atomic_add for float and laziness it is not single value, but array with size = num_of_group */  
                                            __write_only __global   point_2i    * Correspondence_Prev,
                                            __write_only __global   point_2i    * Correspondence_Curr,
                                            __write_only __global   float       * Correspondence_Diff        /*100 - mean they do not correspondent - ugly hack to avoid multiple invokation of kernel ? */
                                           )
{

    int id  =   get_global_id ( 0 );
    int width   =   img_size.x;
    int height  =   img_size.y;

    Correspondence_Prev [ id ].x = 0; Correspondence_Prev [ id ].y = 0;
    Correspondence_Curr [ id ].x = 0; Correspondence_Curr [ id ].y = 0;
    Correspondence_Diff [ id ] = 100.f;

    local float smem [ DEFAULT_STRIDE ];
    local float local_diff_sum [ DEFAULT_STRIDE ];
	
	int tid = 	get_local_id(2) * get_local_size(0) * get_local_size(1) +
	            get_local_id(1) * get_local_size(0) +
	            get_local_id(0);	//flattenedThreadId

    smem [ tid ] = 0;
	local_diff_sum [tid ] = 0;
    barrier (CLK_LOCAL_MEM_FENCE);

    if ( id < Num_Of_Interest_Point )
    {
        point_3i p1 = { List_Of_Interest_Points [ id ].x,
                        List_Of_Interest_Points [ id ].y, 
                        1 
                      };

        float z = D_Curr [ width * p1.y + p1.x ];   // FIXME what if p1.x\y out of range ?

        if ( Is_Depth_Valid ( z ) )
        {
            point_3f i_have_to_do_smth_with_base_operators = { p1.x, p1.y, p1.z };
            point_3f p2 = multiply ( &R, &i_have_to_do_smth_with_base_operators /* &p1 */);
            p2.x *= z;
            p2.y *= z;
            p2.z *= z;
            point_3f p = add_3f (  p2, t );
            
            // probably we need some special treatment here ?
            point_2i p_projection = {   p.x/p.z, 
                                        p.y/p.z  
                                    };
            
            if ( Whether_Point_Within_Img ( p_projection, img_size ) )
            {
                float z_prev = D_Prev [ width * p_projection.y + p_projection.x ];
                if ( Is_Depth_Valid ( z_prev )  && fabs ( p.z - z_prev ) >  Depth_Threshold ) 
                {
                                
                    Correspondence_Prev [ id ].x = p1.x; Correspondence_Prev [ id ].y = p1.y;
                    Correspondence_Curr [ id ].x = p_projection.x; Correspondence_Curr [ id ].y = p_projection.y;
                    Correspondence_Diff [ id ] = I_Curr [ p1.y * width + p1.x ].data.value - I_Curr [ p_projection.y * width + p_projection.x ].data.value;
                    
	                local_diff_sum [tid ] = Correspondence_Diff [ id ];
                    smem [ tid ] = 1;
                }   // whether depth projection on prev frame is valid

            }   // projection within image bounds

        }   // if ( z > 0 ) aka valid depth 
    }   // if ( id < Num_Of_Interest_Point )

	barrier (CLK_LOCAL_MEM_FENCE);
    float   val =  smem [tid];
    float   val_sum =   local_diff_sum [ tid ];
			
    if (tid < 128) {
        smem[tid] = val = val  + smem[tid + 128]; 
        local_diff_sum[tid] = val_sum = val_sum + local_diff_sum [ tid + 128 ];
    }
    barrier (CLK_LOCAL_MEM_FENCE);

    if (tid < 64)  {
        smem [tid] = val = val + smem[tid +  64]; 
        local_diff_sum[tid] = val_sum = val_sum + local_diff_sum [ tid + 64 ];
    }
    barrier (CLK_LOCAL_MEM_FENCE);
	if (tid < 32) {
        smem[tid] = val = val + smem[tid +  32];
        local_diff_sum[tid] = val_sum = val_sum + local_diff_sum [ tid + 32];
    }
    barrier (CLK_LOCAL_MEM_FENCE);
	if (tid < 32) {
        smem[tid] = val = val + smem[tid +  16]; 
        local_diff_sum[tid] = val_sum = val_sum + local_diff_sum [ tid + 16 ];
    }
    barrier (CLK_LOCAL_MEM_FENCE);
	if (tid < 32) {
        smem[tid] = val = val + smem[tid +   8];
        local_diff_sum[tid] = val_sum = val_sum + local_diff_sum [ tid + 8];
    }
    barrier (CLK_LOCAL_MEM_FENCE);
	if (tid < 32) { 
        smem[tid] = val = val + smem[tid +   4]; 
        local_diff_sum[tid] = val_sum = val_sum + local_diff_sum [ tid + 4];
    }
    barrier (CLK_LOCAL_MEM_FENCE);
	if (tid < 32) {
        smem[tid] = val = val + smem[tid +   2]; 
        local_diff_sum[tid] = val_sum = val_sum + local_diff_sum [ tid + 2];
    }
    barrier (CLK_LOCAL_MEM_FENCE);
	if (tid < 32) {
        smem[tid] = val = val + smem[tid +   1];
        local_diff_sum[tid] = val_sum = val_sum + local_diff_sum [ tid + 1];
    }
    barrier (CLK_LOCAL_MEM_FENCE);

	if ( tid == 0 )
    {
            atomic_add ( Num_Of_Correspondence, smem[0] );
            Total_Diff [ get_group_id ( 0 ) ] = local_diff_sum[0];
    }
}
