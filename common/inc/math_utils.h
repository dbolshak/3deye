#ifndef MATH_UTILS
#define MATH_UTILS

#include <matrix.h>
#include <base_point_types.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <pcl/point_cloud.h>    // PointCloud
#include <pcl/point_types.h>    // PointXYZRGBA

int
round_towards_nearest_even ( float value);

typedef Eigen::Matrix < float, 3, 3, Eigen::RowMajor > Matrix3frm;
Matrix3frm 
matrix_3f_to_Eigen  ( const matrix_3f & A );

matrix_3f 
Eigen_to_matrix_3f ( const Matrix3frm & A );

Eigen::Vector3f 
vector_3f_to_Eigen ( const vector_3f & A );

vector_3f 
Eigen_to_vector_3f ( const Eigen::Vector3f & A );

matrix_3f 
inverse ( const matrix_3f & A );

matrix_3f
multiply ( matrix_3f & A, matrix_3f & B );

point_3f
Transform_Point (   const point_3f &point, 
                    const Eigen::Affine3f & transform
                );

unsigned int
greatest_common_divisor(unsigned int a, unsigned int b);

unsigned int
least_common_multiple(unsigned int a, unsigned int b);

void
Transform_Cloud (    matrix <Point_XYZI> & In_Cloud,
                     matrix <Point_XYZI> & Out_Cloud,
                     Eigen::Affine3f & Transform );

void
convert_to_point_cloud (    matrix <Point_XYZI_Color> & Point_Cloud,
                            pcl::PointCloud < pcl::PointXYZRGB> & pcl_cloud
                       );

void
convert_to_point_cloud (
                            matrix <Point_XYZI_Color> & Point_Cloud,
                            pcl::PointCloud < pcl::PointXYZ> & pcl_cloud
                        );

void
convert_to_point_cloud (    matrix <Point_XYZI> & Vertex_Point_Cloud,
                            matrix <Point_Color> & Color_Point_Cloud,
                            pcl::PointCloud < pcl::PointXYZRGB > & pcl_cloud );

void
convert_to_point_cloud (
                            matrix <Point_XYZI> & Vertex_Point_Cloud, 
                            pcl::PointCloud <pcl::PointXYZI> & pcl_cloud   
                       );

void
convert_to_point_cloud (
                            matrix <Point_XYZI> & Vertex_Point_Cloud, 
                            pcl::PointCloud <pcl::PointXYZ> & pcl_cloud  /*NOTE:in Point_XYZI
                                                                                out PointXYZ */ 
                       );

#endif
