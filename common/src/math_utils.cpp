#include <math_utils.h>
#include <math.h>   // moddf


int
round_towards_nearest_even ( float value ) {
    float value_r = value + 0.49999997f;
    int result =	value_r;	

    float param, fractpart, intpart;

    fractpart = modff (value_r, &intpart);

    if ( fractpart > 0.5f )
        result++;
    else
    if ( (int) intpart % 2 != 0)
		result++;

    return result;
/*
	float value_r = sum1/sum2;	
	float ipart;
		modff( value_r, &ipart );
	float epsilon = 0.0000001;

	int ret = 0;

		// If 'value' is exactly halfway between two integers
	if ((value_r -(ipart +0.5)) < epsilon)
	{
		// If 'ipart' is even then return 'ipart'
		if (fmod( ipart, 2.0 ) < epsilon)
			ret = ipart;
		else
		{
			ret = ceil( fabs( ipart +0.5 ) );
		}
	}
	else
		ret = (unsigned short) value_r;

	dst.data [row_num*width+col_num] = ret;

	source of algo of frama-c news and ideas
	http://blog.frama-c.com/index.php?post/2013/05/02/nearbyintf1
*/
}

/* FIXME implement and move to elementary_matrix_*  or math/Eigen_wrappers*/
Matrix3frm 
matrix_3f_to_Eigen  ( const matrix_3f & A ) {
    Matrix3frm result;
    
    result (0,0) = A.data[0].x;
    result (0,1) = A.data[0].y;
    result (0,2) = A.data[0].z;

    result (1,0) = A.data[1].x;
    result (1,1) = A.data[1].y;
    result (1,2) = A.data[1].z;

    result (2,0) = A.data[2].x;
    result (2,1) = A.data[2].y;
    result (2,2) = A.data[2].z;

    return result;
}

matrix_3f 
Eigen_to_matrix_3f ( const Matrix3frm & A ) {
    matrix_3f result;
    result.data[0].x = A (0,0);
    result.data[0].y = A (0,1);
    result.data[0].z = A (0,2);
    result.data[1].x = A (1,0);
    result.data[1].y = A (1,1);
    result.data[1].z = A (1,2);
    result.data[2].x = A (2,0);
    result.data[2].y = A (2,1);
    result.data[2].z = A (2,2);
    
    return result;
}

Eigen::Vector3f 
vector_3f_to_Eigen ( const vector_3f & A ) {
    Eigen::Vector3f result;

    result(0) = A.x;         
    result(1) = A.y;
    result(2) = A.z;

    return result;
}

vector_3f 
Eigen_to_vector_3f ( const Eigen::Vector3f & A ) {
    vector_3f result;

    result.x = A(0);
    result.y = A(1);
    result.z = A(2);

    return result;
}

matrix_3f inverse ( const matrix_3f & A ) {
    Matrix3frm B;
    matrix_3f A_inverse;

    B = matrix_3f_to_Eigen ( A );

    A_inverse = Eigen_to_matrix_3f ( B.inverse() );

    return A_inverse;
}

matrix_3f
multiply ( matrix_3f & A, matrix_3f & B ) {
    matrix_3f result;

    Matrix3frm AE = matrix_3f_to_Eigen (A);
    Matrix3frm BE = matrix_3f_to_Eigen (B);
    Matrix3frm C = AE * BE;

    result = Eigen_to_matrix_3f (C);

    return result;
}

point_3f
Transform_Point (   const point_3f &point, 
                    const Eigen::Affine3f & transform
                ) {
    point_3f ret = point;

    ret.x = static_cast<float> (transform (0, 0) * point.x + transform (0, 1) * point.y + transform (0, 2) * point.z + transform (0, 3));
    ret.y = static_cast<float> (transform (1, 0) * point.x + transform (1, 1) * point.y + transform (1, 2) * point.z + transform (1, 3));
    ret.z = static_cast<float> (transform (2, 0) * point.x + transform (2, 1) * point.y + transform (2, 2) * point.z + transform (2, 3));

    return (ret);
}

unsigned int
greatest_common_divisor(unsigned int a, unsigned int b) 
{
    assert( a > 0 && b > 0 );
    unsigned int mask = 1;
    while( 0 == ( (a|b) & mask ) ) {
        assert( mask != 0 );
        mask *= 2;
    }
    while( 0 == ( b & mask ) ) {
        b /= 2;
        assert( b != 0 );
    }
    
    do {
        assert( a != 0 && ( b & mask ) != 0 );
        while( 0 == ( a & mask ) ) {
            a /= 2;
            assert( a != 0 );
        }
        if( a < b ) {
            a = b - a;    //    std::swap(a, b); 
            b -= a;        //    a -= b;
        } else {
            a -= b;
        }
    } while( a > 0 );
    return b;
}

unsigned int
least_common_multiple(unsigned int a, unsigned int b) 
{
	return (unsigned int) a * b / greatest_common_divisor(a,b);
}

/*
 *      You can use GPU for that
 * */
void
Transform_Cloud (    matrix <Point_XYZI> & In_Cloud,
                     matrix <Point_XYZI> & Out_Cloud,
                     Eigen::Affine3f & Transform )
{
    if ( & In_Cloud != & Out_Cloud )
        Out_Cloud.Set_Dimensions ( In_Cloud.Get_Dimensions() );

    // Dataset might contain NaNs and Infs, so check for them first,
    // otherwise we get errors during the multiplication (?)
    for (size_t i = 0; i < In_Cloud.Get_Num_Of_Elem (); ++i) {

        XYZI cur_point = In_Cloud.data[i];

        if (    ! isfinite (cur_point.x) || 
                ! isfinite (cur_point.y) || 
                ! isfinite (cur_point.z) )
            continue;

        // FIXME - rewrite - 
        // too complex 
        // use SSE ???
        Eigen::Matrix<float, 3, 1> pt ( cur_point.x, cur_point.y, cur_point.z );
        XYZI result;
        result.x = static_cast<float> (Transform (0, 0) * pt.coeffRef (0) + Transform (0, 1) * pt.coeffRef (1) + Transform (0, 2) * pt.coeffRef (2) + Transform (0, 3));
        result.y = static_cast<float> (Transform (1, 0) * pt.coeffRef (0) + Transform (1, 1) * pt.coeffRef (1) + Transform (1, 2) * pt.coeffRef (2) + Transform (1, 3));
        result.z = static_cast<float> (Transform (2, 0) * pt.coeffRef (0) + Transform (2, 1) * pt.coeffRef (1) + Transform (2, 2) * pt.coeffRef (2) + Transform (2, 3));
        result.w = cur_point.w;
        Out_Cloud.data[i] = result;
    }
}

void
convert_to_point_cloud (    matrix <Point_XYZI_Color> & Point_Cloud,
                            pcl::PointCloud < pcl::PointXYZRGB> & pcl_cloud
                         )
{
    for (unsigned int z = 0; z < Point_Cloud .Get_Num_Of_Elem(); z++)
    {
        pcl::PointXYZRGB one_more;

        XYZI_Color cur_point = Point_Cloud.data[z];

       one_more.x = cur_point.vertex.x;
       one_more.y = cur_point.vertex.y;
       one_more.y = cur_point.vertex.z;

        uint8_t r = cur_point.color.b,
                g = cur_point.color.g,
                b = cur_point.color.r;
        uint32_t rgb = ((uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b);
        one_more.rgb = *reinterpret_cast<float*>(&rgb);
    
        pcl_cloud.push_back (one_more);
    }
}

void
convert_to_point_cloud (
                            matrix <Point_XYZI_Color> & Point_Cloud,
                            pcl::PointCloud < pcl::PointXYZ> & pcl_cloud
                        )
{
    for (unsigned int z = 0; z < Point_Cloud.Get_Num_Of_Elem(); z++)
    {
        pcl::PointXYZ one_more;

        XYZI_Color cur_point = Point_Cloud.data[z];

       one_more.x = cur_point.vertex.x;
       one_more.y = cur_point.vertex.y;
       one_more.y = cur_point.vertex.z;

        pcl_cloud.push_back (one_more);
    }
}

void
convert_to_point_cloud (    matrix <Point_XYZI> & Vertex_Point_Cloud,
                            matrix <Point_Color> & Color_Point_Cloud,
                            pcl::PointCloud < pcl::PointXYZRGB > & pcl_cloud )
{
    for (unsigned int z = 0; z < Color_Point_Cloud .Get_Num_Of_Elem(); z++)
    {
        pcl::PointXYZRGB one_more;

        XYZI cur_vertex = Vertex_Point_Cloud [z];
        Color cur_color = Color_Point_Cloud [z];
    
        one_more.x = cur_vertex.x; 
        one_more.y = cur_vertex.y; 
        one_more.z = cur_vertex.z;
    
        uint8_t r = cur_color.b, g = cur_color.g, b = cur_color.r;
        uint32_t rgb = ((uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b);
        one_more.rgb = *reinterpret_cast<float*>(&rgb);
    
        pcl_cloud.push_back (one_more);
    }
}

void
convert_to_point_cloud (
                            matrix <Point_XYZI> & Vertex_Point_Cloud, 
                            pcl::PointCloud <pcl::PointXYZI> & pcl_cloud   
                       )
{
    for (unsigned int z = 0; z < Vertex_Point_Cloud.Get_Num_Of_Elem(); z++)
    {
        pcl::PointXYZI one_more;

        XYZI cur = Vertex_Point_Cloud[z];
    
        one_more.x = cur.x; 
        one_more.y = cur.y; 
        one_more.z = cur.z;
        one_more.intensity = cur.w; 

        pcl_cloud.push_back (one_more);
    }

}

void
convert_to_point_cloud (
                            matrix <Point_XYZI> & Vertex_Point_Cloud, 
                            pcl::PointCloud <pcl::PointXYZ> & pcl_cloud  /*NOTE:in Point_XYZI
                                                                                out PointXYZ */ 
                       )
{
    pcl::PointXYZ one_more;
    for (unsigned int z = 0; z < Vertex_Point_Cloud.Get_Num_Of_Elem(); z++)
    {

        XYZI cur = Vertex_Point_Cloud[z];
    
        one_more.x = cur.x; 
        one_more.y = cur.y; 
        one_more.z = cur.z;
//        one_more.intensity = cur.w; 

        pcl_cloud.push_back (one_more);
    }

}

// from math_utils.cpp
Eigen::Vector3f
rodrigues2(const Eigen::Matrix3f & matrix) {
    Eigen::JacobiSVD<Eigen::Matrix3f> svd(matrix, Eigen::ComputeFullV | Eigen::ComputeFullU);
    Eigen::Matrix3f R = svd.matrixU() * svd.matrixV().transpose();

    double rx = R(2, 1) - R(1, 2);
    double ry = R(0, 2) - R(2, 0);
    double rz = R(1, 0) - R(0, 1);

    double s = sqrt((rx*rx + ry*ry + rz*rz)*0.25);
    double c = (R.trace() - 1) * 0.5;
    c = c > 1. ? 1. : c < -1. ? -1. : c;

    double theta = acos(c);

    if( s < 1e-5 )
    {
        double t;

        if( c > 0 )
            rx = ry = rz = 0;
        else
        {
            t = (R(0, 0) + 1)*0.5;
            rx = sqrt( std::max(t, 0.0) );
            t = (R(1, 1) + 1)*0.5;
            ry = sqrt( std::max(t, 0.0) ) * (R(0, 1) < 0 ? -1.0 : 1.0);
            t = (R(2, 2) + 1)*0.5;
            rz = sqrt( std::max(t, 0.0) ) * (R(0, 2) < 0 ? -1.0 : 1.0);

            if( fabs(rx) < fabs(ry) && fabs(rx) < fabs(rz) && (R(1, 2) > 0) != (ry*rz > 0) )
                rz = -rz;
            theta /= sqrt(rx*rx + ry*ry + rz*rz);
            rx *= theta;
            ry *= theta;
            rz *= theta;
        }
    }
    else
    {
        double vth = 1/(2*s);
        vth *= theta;
        rx *= vth; ry *= vth; rz *= vth;
    }
    return Eigen::Vector3d(rx, ry, rz).cast<float>();
}