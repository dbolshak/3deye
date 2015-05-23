#ifndef BASE_POINT_TYPES
#define BASE_POINT_TYPES
/**			WARNING
*	a lot of macros
*	just a miserable attempt  to emulate generic data-types in ansi c
*	which is used in OpenCL modules
*	sources of idea:
*	http://stackoverflow.com/questions/1489932/c-preprocessor-and-concatenation
*	http://stackoverflow.com/questions/10950828/simulation-of-templates-in-c
*	http://habrahabr.ru/post/154811/
*	http://habrahabr.ru/post/155439/
*	http://arnold.uthar.net/index.php?n=Work.TemplatesC
*/

#include <base_points.h>

#define CAT(X,Y) X##Y
#define CAT_(X,Y) X##_##Y
#define	TYPE_PREFIX(X,Y) CAT(X,Y)
#define PREFIX TYPE_PREFIX(DIMENSIONS,TYPE)
#define TYPE_NAME(X,Y) CAT_(X,Y)


/*
 *                              definition of Point data types which actually is used 
 *                              at higher level of module - Color, Tsdf, Depth etc
 *                              base types of such points are structs which declare at base_points.h
 *
 * */

#define BASE_TYPE   Point 
#ifndef GENERATE_OPENCL_CODE
    /* for random number generation */
    #include <stdlib.h>
    #include <base_point_template.h>
#endif

/* * * * * * * * * * * * * * * * * * * Point_Tsdf * * * * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE Tsdf
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_Tsdf_Color * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE Tsdf_Color
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_Depth * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE Depth
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_Vertex * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE Vertex
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_Normal * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE Normal
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_Intensity * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE Intensity 
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_Color * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE Color
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_XYZI * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE XYZI
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * Point_XYZI_Color * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE XYZI_Color
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_XYZ * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE XYZ
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_qXYZI * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE qXYZI
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_NXYZI * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE NXYZI
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_NXYZ * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE NXYZ
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

/* * * * * * * * * * * * * * * * * * * Point_Vertex_Normal * * * * * * * * * * * * * * * * * * */
#define INTERNAL_TYPE Vertex_Normal
    #ifdef GENERATE_OPENCL_CODE
        #include <base_point_template.h> 
    #else
        GENERATE_CPP_ALIAS(BASE_TYPE, INTERNAL_TYPE)
    #endif
#undef INTERNAL_TYPE

#undef BASE_TYPE

#endif // BASE_POINT_TYPES
