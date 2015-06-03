#define GENERATE_OPENCL_CODE
#define INCLUDED_IN_IMPLEMENTATION_FILE
#include <elementary_points.h>
#include <base_point_types.h>

__kernel 
void transform_map(    __global Point_XYZ * Vertex_Map_Src,
                        __global Point_NXYZ * Normal_Map_Src,
                        matrix_3f Rotation,
                        point_3f Translation, 
                        point_2us img_size,
            			__global Point_XYZ * Vertex_Map_Dst,
                        __global Point_NXYZ * Normal_Map_Dst)
{
	int x = get_global_id(0);
	int y = get_global_id(1);

    size_t width = img_size.x;
    size_t height = img_size.y;

	const float qnan = NAN;

	if ( x >= width && y >= height )
        return;

    // vertex 
    XYZ V_src = Vertex_Map_Src[ x + y * width ].data;

    point_3f 	vsrc, 
                vdst = make_point_3f (qnan, qnan, qnan);

    vsrc.x = V_src.x; 

    if (!isnan (vsrc.x))
    {
        vsrc.y =  V_src.y;
        vsrc.z =  V_src.z;
        
        vdst = add_3f ( multiply ( &Rotation, &vsrc ), Translation );

	}

    Vertex_Map_Dst [ x + width * y ].data.x= vdst.x ;
    Vertex_Map_Dst [ x + width * y ].data.y = vdst.y ;
    Vertex_Map_Dst [ x + width * y ].data.z = vdst.z ;

    //normals
    XYZ N_src = Normal_Map_Src [ x + width * y ].data;
    point_3f 	nsrc, 
                ndst =  make_point_3f (qnan, qnan, qnan);

    nsrc.x = N_src.x;

    if (!isnan (nsrc.x))
    {
        nsrc.y = N_src.y;
        nsrc.z = N_src.z;

        ndst = multiply ( &Rotation, & nsrc );

    }
    Normal_Map_Dst [ x + width * y ].data.x= ndst.x ;
    Normal_Map_Dst [ x + width * y ].data.y = ndst.y ;
    Normal_Map_Dst [ x + width * y ].data.z = ndst.z ;
}
