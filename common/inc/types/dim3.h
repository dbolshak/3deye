#ifndef DIM3
#define DIM3

#ifndef GENERATE_OPENCL_CODE
#include <iostream>
#endif

/*
 *  FIXME do not use elementary_points_* due to default values
	which are unavailable in Ansi C
 * */
struct dim3
{
    /* just alias for the same data for convinece and
     * ambiguilty
     * */
	union {
		size_t data [ 3 ];
		struct {
    			size_t x; 
    			size_t y;
    			size_t z;
		};
        struct {
    			size_t width; 
    			size_t height;
    			size_t depth;
		};
};
#ifndef GENERATE_OPENCL_CODE
// FIXME add default constructor to explicitly convert int\uint to dim3
    dim3( size_t nx = 1, size_t ny = 1, size_t nz = 1 ) : x(nx), y(ny), z(nz) {};
	friend inline ostream & operator << (ostream & out, const dim3 & obj_to_print) {
		out << "width - " << obj_to_print.x << " height - " << obj_to_print.y << " depth - " << obj_to_print.z << endl;
		return out;
	}
#endif
};

#endif
