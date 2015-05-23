#include <intrinsics.h>

#include <string_utils.h>

#include <sstream>

intrinsics
intrinsics :: operator () (int level_index) const {
    int div = 1 << level_index; 
    intrinsics ret_val = {  fx / div, 
                            fy / div,
                            cx / div, 
                            cy / div,
                            cols >> level_index,
                            rows >> level_index
                        };
    return ret_val;
}

std::ostream &
operator << ( std::ostream & out, const intrinsics & intr ) {
    char    COMMON_DELIMITER[]          =   "x";

    out <<  intr.fx << COMMON_DELIMITER <<
            intr.fy << COMMON_DELIMITER <<
            intr.cx << COMMON_DELIMITER <<
            intr.cy << COMMON_DELIMITER <<
            intr.cols << COMMON_DELIMITER <<
            intr.rows << COMMON_DELIMITER <<
            " ";

    return out;    
}

std::istream &
operator >> ( std::istream & in, intrinsics & intr ) {
    // FIXME can't use it from constants because of rounded dependency
    char    COMMON_DELIMITER[]          =   "x";
    std::string tmp_str = "";
    in >> tmp_str;
    replaceAll ( tmp_str, COMMON_DELIMITER, " " );

    std::istringstream in_tmp ( tmp_str );

    in_tmp >>   intr.fx     >> 
                intr.fy     >> 
                intr.cx     >> 
                intr.cy     >> 
                intr.cols   >> 
                intr.rows; 
    return in;    
}