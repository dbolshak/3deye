#ifndef STRING_UTILS 
#define STRING_UTILS

#include <string>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <enums.h>

void replaceAll ( std::string& str, const std::string& from, const std::string& to);

void
add_opencl_extension ( std::string & kernel_name );

void
write_isometry_to_file ( const char * file_name, std::vector <Eigen::Affine3d> & data );

void
write_isometry_to_file ( const char * file_name, std::vector <Eigen::Isometry3d> & data );

inline std::ostream& operator<< ( std::ostream & os, odometry_method  method )
{
    switch (method) {
        case METHOD_ICP     :   os << "Method ICP"  ; break;
        case METHOD_FOVIS   :   os << "Method FOVIS"; break;   
        case METHOD_MIXED   :   os << "Method MIXED"; break;
        case METHOD_DVO     :   os << "Method DVO"  ; break;
        case METHOD_RGBD    :   os << "Method RGBD" ; break;
        case METHOD_DBG     :   os << "Method DBG"  ; break;
        default             :   os << "Method undefined!!!"; break;
    };
    return os;
};

#endif
