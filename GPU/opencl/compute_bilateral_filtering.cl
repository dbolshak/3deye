#define GENERATE_OPENCL_CODE
#define INCLUDED_IN_IMPLEMENTATION_FILE
#include <constants.h>
#include <elementary_points.h>
#include <base_point_types.h>


__kernel
void
compute_bilateral_filtering (   __read_only __global Point_Depth * src ,
                                __write_only __global Point_Depth * dst ,
                                point_2us sizes, 
                                unsigned int max_distance_mm
                            )
{
	int col_num = get_global_id(0);
	int row_num = get_global_id(1);

	const int R	=	6;
	const int D	=	R * 2 + 1;

    ushort value = src [ row_num * sizes.x + col_num ].data.value;
	
	int max_x = min( col_num - D/2 + D, (int)sizes.x - 1 ); 
	int max_y = min( row_num - D/2 + D, (int)sizes.y - 1 );

	int min_x = max( col_num - D/2 , 0 );
	int min_y = max( row_num - D/2 , 0 );

	float sum1 = 0.0f;
	float sum2 = 0.0f;

	for ( int cy = min_y; cy < max_y; ++cy)
	{
		for ( int cx = min_x; cx < max_x; ++cx)
		{
            ushort tmp = src [ cy * sizes.x + cx ].data.value;

			int space2 = ( col_num - cx ) * ( col_num - cx ) + ( row_num - cy ) * ( row_num - cy );
			int color2 = ( value - tmp ) * ( value - tmp );

			float weight = exp( - ( space2 * SIGMA_SPACE_INV_HALF + color2 * SIGMA_COLOR_INV_HALF ) );// cuda __expf

			sum1 += tmp * weight;
			sum2 += weight;
		}
	}

	int result = convert_int_rte( sum1/sum2);
	short res = max ( 0, min (result, MAX_DEPTH_VALUE) );

    if (res > max_distance_mm )    // truncate_depth
        res = 0.f;

    dst[ row_num * sizes.x + col_num ].data.value = convert_ushort_rte (sum1);

}
