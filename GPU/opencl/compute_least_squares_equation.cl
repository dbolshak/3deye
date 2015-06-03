#define GENERATE_OPENCL_CODE
#define INCLUDED_IN_IMPLEMENTATION_FILE
#include <elementary_points.h>
#include <base_point_types.h>
#include <intrinsics.h>
#include <constants.h>

#define SOBEL_SCALE 1./8.

__kernel
void compute_least_squares_equation (

    __read_only __global    Point_Intensity *   I_Prev,
    __read_only __global    float           *   D_Prev,
    __read_only __global    Point_Intensity *   I_Curr,
    __read_only __global    unsigned char   *   DI_Dx,
    __read_only __global    unsigned char   *   DI_Dy,
    __read_only __global    point_2i        *   Correspondence_Prev,
    __read_only __global    point_2i        *   Correspondence_Curr,
    __read_only __global    float           *   Correspondence_Diff,
                            float               Sigma,
                            matrix_3f           Rotation,
                            vector_3f           Translation,
                            intrinsics          Camera_Params,
                            int                 Num_Of_Correspondence,
    __write_only __global    float           *   Indexes,
    __write_only __global    float           *   Indexes_Vector

)
{
    int id = get_global_id(0);
    float C[6];
    C[0] = C[1] = C[2] = C[3] = C[4] = C[5];

    if ( id >= Num_Of_Correspondence )
        return;

        point_2i    p_prev      =   Correspondence_Prev[id];
        point_2i    p_curr      =   Correspondence_Curr[id];
        float       p_diff      =   Correspondence_Diff[id];

        // calculate actual weight
        float   current_weight  =   Sigma + fabs ( p_diff );
        current_weight          =   current_weight > DBL_EPSILON ? 1./current_weight : 1.;
        float   sobelScale      =   current_weight * SOBEL_SCALE;

        // calculate 3d coordinates of current point
        float inv_fx    =   1. / Camera_Params.fx;
        float inv_fy    =   1. / Camera_Params.fy;

        float new_x     =   ( p_prev.x - Camera_Params.cx ) * inv_fx;
        float new_y     =   ( p_prev.y - Camera_Params.cy ) * inv_fy;
        float new_z     =   D_Prev [ p_prev.y * Camera_Params.cols + p_prev.x ];

        new_x = new_x * new_z;
        new_y = new_y * new_z; 

        point_3f p_3d   = { new_x, new_y, new_z };

        //  Project point from previous frame to current
        point_3f    p_projection    =   add_3f ( multiply ( &Rotation, &p_3d ), Translation ); 

        // calculate coeffecients for equation
        float inv_z =   1.  /   p_projection.z;
        float   v0  =   inv_z * Camera_Params.fx * sobelScale * DI_Dx [ p_curr.y * Camera_Params.cols + p_curr.x ];
        float   v1  =   inv_z * Camera_Params.fy * sobelScale * DI_Dy [ p_curr.y * Camera_Params.cols + p_curr.x ];
        float   v2  =   inv_z * ( - ( v0 * p_projection.x + v1 * p_projection.y ) );

        C[0] = -p_projection.z * v1 + p_projection.y * v2;
        C[1] =  p_projection.z * v0 - p_projection.x * v2;
        C[2] = -p_projection.y * v0 + p_projection.x * v1;
        C[3] = v0;
        C[4] = v1;
        C[5] = v2;

    Indexes [ id  +   0 ]  =   C[0];
    Indexes [ id  +   1 ]  =   C[1];
    Indexes [ id  +   2 ]  =   C[2];
    Indexes [ id  +   3 ]  =   C[3];
    Indexes [ id  +   4 ]  =   C[4];
    Indexes [ id  +   5 ]  =   C[5];

    // calculate partial vector
    for ( int i = 0; i < TRANSFORM_DIM_RGB; i++)
        Indexes_Vector [ id + i ] = C [ i ] * p_diff * current_weight;

}
