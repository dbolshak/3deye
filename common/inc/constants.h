#ifndef CONSTANTS
#define CONSTANTS

#include <enums.h>
#include <common_defines.h>

/*
 *    FIXME 2 I guess we should invoke common include header above ifdefs
 * */

/* Side-specific constants     */
#ifdef GENERATE_OPENCL_CODE

#define OPENCL_MEM_PTR __global
#define OPENCL_READ_ONLY_MEM_PTR __read_only __global
#define OPENCL_CONST_PTR    __constant
#define CPP_READ_ONLY_MEM_PTR
#define ROUND_TOWARDS_ZERO(X) convert_int_rtz( (X) )
//#define ROUND_TOWARDS_NEAREST_EVEN(X)   convert_float_rte( (X) )
#define ROUND_TOWARDS_NEAREST_EVEN(X)   convert_int_sat_rte( (X) )
#define ROUND_TOWARDS_NEGATIVE_INFINITY(X) convert_int_sat_rtn ( (X)  ) 

// FIXME! - check where it is used and is it ok to provide such values to all related kernels?
//const sampler_t sampler =       CLK_NORMALIZED_COORDS_FALSE     |
//                                CLK_ADDRESS_CLAMP_TO_EDGE       |
//                                CLK_FILTER_NEAREST;
// FIXME at the integrate_color_volume, 
// compute_bilateral_filtering, - changed to buffer
// compute_depth_pyramid - changed to buffer
// const sampler_t	sampler =	CLK_NORMALIZED_COORDS_FALSE |
//				CLK_ADDRESS_NONE;
/**/
#else // NOT GENERATE_OPENCL_CODE

#include <opencl_utils.h>
#include <elementary_points.h>
#include <dim3.h>
#include <math.h>
//#include <math_utils.h> // for cpp version of round_rte

#define OPENCL_MEM_PTR 
#define OPENCL_READ_ONLY_MEM_PTR const
#define OPENCL_CONST_PTR const    
#define CPP_READ_ONLY_MEM_PTR const

#define ROUND_TOWARDS_ZERO(X) (int) floor(X)    /* FIXME icheck difference between usage of convert_int_rtz convert_int_rte*/
#define ROUND_TOWARDS_NEGATIVE_INFINITY(X) (int) floor(X) 


#define DEFAULT_ANGLE_THRESHOLD (sin(DEG2RADIAN(20.f)))
static  const   bool    COLOR_DISABLED  = false;
static  const   bool    COLOR_ENABLED   = true;

static  const   bool    ALLOCATE_NOW = true;
static  const   int RESIZE_VERTEX = 0;
static  const   int RESIZE_NORMAL = 1;

static  const   int FIRST_FRAME =   0;

static  const   int     DEFAULT_VOLUME_VOXEL_ORIGIN_X   =   0;
static  const   int     DEFAULT_VOLUME_VOXEL_ORIGIN_Y   =   0;
static  const   int     DEFAULT_VOLUME_VOXEL_ORIGIN_Z   =   0;
static  const   point_3i    DEFAULT_VOLUME_VOXEL_ORIGIN = { 
                                                            DEFAULT_VOLUME_VOXEL_ORIGIN_X,
                                                            DEFAULT_VOLUME_VOXEL_ORIGIN_Y,
                                                            DEFAULT_VOLUME_VOXEL_ORIGIN_Z
                                                            };

static  const   float    DEFAULT_VOLUME_METRIC_ORIGIN_X =   0.f; 
static  const   float    DEFAULT_VOLUME_METRIC_ORIGIN_Y =   0.f;
static  const   float    DEFAULT_VOLUME_METRIC_ORIGIN_Z =   0.f;
static  const   point_3f    DEFAULT_VOLUME_METRIC_ORIGIN    =   {
                                                                    DEFAULT_VOLUME_METRIC_ORIGIN_X,
                                                                    DEFAULT_VOLUME_METRIC_ORIGIN_Y,
                                                                    DEFAULT_VOLUME_METRIC_ORIGIN_Z
                                                                };
static  const   int     DEFAULT_VOLUME_VOXELS_WIDTH    =   512;
static  const   int     DEFAULT_VOLUME_VOXELS_HEIGHT   =   512;
static  const   int     DEFAULT_VOLUME_VOXELS_DEPTH    =   512;
static  const   point_3i   DEFAULT_VOLUME_VOXELS_SIZE   =   { 
                                                                DEFAULT_VOLUME_VOXELS_WIDTH,
                                                                DEFAULT_VOLUME_VOXELS_HEIGHT,
                                                                DEFAULT_VOLUME_VOXELS_DEPTH
                                                            };

static  const   float   DEFAULT_VOLUME_METRIC_WIDTH     =   3.f;
static  const   float   DEFAULT_VOLUME_METRIC_HEIGHT    =   3.f;
static  const   float   DEFAULT_VOLUME_METRIC_DEPTH     =   3.f;
static  const   point_3f    DEFAULT_VOLUME_METRIC_SIZE  =   {
                                                                DEFAULT_VOLUME_METRIC_WIDTH,
                                                                DEFAULT_VOLUME_METRIC_HEIGHT,
                                                                DEFAULT_VOLUME_METRIC_DEPTH
                                                            };
static  const   float   DEFAULT_CELL_SIZE_X = DEFAULT_VOLUME_METRIC_WIDTH / DEFAULT_VOLUME_VOXELS_WIDTH;
static  const   float   DEFAULT_CELL_SIZE_Y = DEFAULT_VOLUME_METRIC_HEIGHT / DEFAULT_VOLUME_VOXELS_HEIGHT;   
static  const   float   DEFAULT_CELL_SIZE_Z = DEFAULT_VOLUME_METRIC_DEPTH / DEFAULT_VOLUME_VOXELS_DEPTH;
static  const   point_3f    DEFAULT_CELL_SIZE = {
                                                    DEFAULT_CELL_SIZE_X,
                                                    DEFAULT_CELL_SIZE_Y,
                                                    DEFAULT_CELL_SIZE_Z 
                                                };

static	const	char	COLOR_DELIMITER[]           =	".";
static  const   char    TSDF_DELIMITER[]            =   "-";
static  const   char    COMMON_DELIMITER[]          =   "x";
static  const   char    ROUND_BUFFER_DELIMITER[]    =   "r";
static  const   char    VERTEX_NORMAL_DELIMITER[]   =   "x";
static	const	int	    MAX_RGB_COLOR		        =	255;
/* FIXME following focal_length - for kinect only! */
static  const   float   DEFAULT_FOCAL_LENGTH        =   575.816f;

/*  default tsdf value is equal to 2.1 * volume_metric_size = 3.0f / volume_voxel_size = 512 
 *  but in original kinfu is used 0.03 with default volume characteristics above
 *  */
static  const   float   MAX_TSDF_VALUE              =   0.03f;

static	const	int DEFAULT_MATRIX_WIDTH   =	640;
static	const	int DEFAULT_MATRIX_HEIGHT  =	480;
static  const   int DEFAULT_MATRIX_DEPTH   =   3;

#include <intrinsics.h>
static const intrinsics DEFAULT_INTRINSICS = {  DEFAULT_FOCAL_LENGTH,   /* fx */
                                                DEFAULT_FOCAL_LENGTH,   /* fy */
                                                ( (float) DEFAULT_MATRIX_WIDTH / 2.f - 0.5f ),  /* cx */
                                                ( (float) DEFAULT_MATRIX_HEIGHT / 2.f - 0.5f ),  /* cy */
                                                DEFAULT_MATRIX_WIDTH,
                                                DEFAULT_MATRIX_HEIGHT
                                             };

static const char* MATRIX_TYPE_NAME[] = 
						{
							"DEPTH_MAP",
							"VERTEX_MAP",
							"NORMAL_MAP",
							"RGB_MAP"
						};

static const MATRIX_TYPE DEFAULT_MATRIX_TYPE	=	DEPTH_MAP;

// FIXME list of supported images
// http://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clGetSupportedImageFormats.html
// should be usefull
// CL_RGBA | CL_FLOAT
// CL_BGRA | CL_UNORM_INT8
// static const cl_image_format 
//static const cl_image_format DEFAULT_IMG_FORMAT	= { CL_INTENSITY, CL_FLOAT };
static const cl_image_format DEFAULT_IMG_FORMAT	= { CL_RGBA, CL_UNSIGNED_INT32 };

const   size_t DEFAULT_LOCAL_WORK_SIZE [3] = { 32 , 8, 0 };
const   dim3    SINGLE_ELEMENT ( 1,1,1);

const char OPENCL_EXTENSIONS[] = ".cl";
const int NUM_OF_FUSION_KERNELS = 40; 
const string fusion_kernel_list [ NUM_OF_FUSION_KERNELS ] = {
    "clean_tsdf_slice",
    "clean_color_slice",
    "scan_slice_of_both_volumes",
    "extract_slice_as_cloud",
    "compute_bilateral_filtering",
    "compute_depth_pyramid",
    "compute_normal_map_eigen",
    "compute_vertex_map",
    "estimate_combined",
    "extract_colors",                           /*10*/
    "extract_tsdf_slice_as_cloud",
    "extract_color_slice_as_cloud",
    "extract_tsdf_cloud",
    "init_color",
    "init_tsdf",
    "integrate_color_volume",
    "integrate_tsdf_volume",
    "merge_point_normal",
    "push_cloud_slice_gpu",
    "raycast",                                  /*20*/
    "resize_vertex_map",
    "resize_normal_map",
    "scale_depth",
    "scan_slice_of_tsdf_volume",
    "scan_slice_of_color_volume",
    "scan_subarrays",
    "scan_pow2",
    "scan_subarrays",
    "scan_the_whole_array",
    "transform_estimation",                     /* 30 */
    "transform_map",
    "generate_image",
    "clean_buff",
    "create_intensity_image_and_metric_depth",
    "generate_intensity_and_depth_pyramid",
    "create_intensity_derivatives",
    "scan_for_interest_point",
    "extract_interest_points",
    "scan_list_of_valid_correspondence",
    "compute_least_squares_equation"            /* 40 */
};

static const string DEFAULT_PATH_TO_KERNELS         = "~/3deye/GPU/opencl";
static const string DEFAULT_DBG_FILE_WITH_ISOMETRY  = "";
// FIXME added fast math flag
//const char	DEFAULT_GPU_COMPILE_FLAGS[]	=	"-cl-mad-enable -cl-strict-aliasing -cl-unsafe-math-optimizations -cl-finite-math-only -I /home/dmitry/3deye/common/inc/opencl/ -I /home/dmitry/3deye/common/inc/ -I /home/dmitry/3deye/common/inc/types";
const char	DEFAULT_GPU_COMPILE_FLAGS[]	=	"-cl-mad-enable -cl-strict-aliasing -cl-unsafe-math-optimizations -cl-finite-math-only -I /home/dima/3deye/common/inc/opencl/ -I /home/dima/3deye/common/inc/ -I /home/dima/3deye/common/inc/types";

#include <Callstack_Logger.h>

/*      FIXME
 *      it is a bit annoying to add in every fuction "LOG_TRACE;" string 
 *      so would be much better to instationed all functors from logger class
 *      and call it automatically as parent's constructor/ destructor
 *          
 *      Usefull links:
 *      http://c.learncodethehardway.org/book/ex20.html
 *      http://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing
	by the way, would be very convinient to use some identifiers of debug's severity:
	0 - func call
	...
	3 - file save
 *  
 * */
#ifdef DEBUG
#define LOG_TRACE       Callstack_Logger logger(__FILE__, __FUNCTION__, __LINE__);
#define DEBUG_PRINT(x)  do { std::cout << x << std::endl; } while (0);
#define DBG2(x)         do { std::cerr << #x << ": " << x << std::endl; } while (0);
#else
#define LOG_TRACE
#define DEBUG_PRINT(x)
#define DBG2(x)
#endif

#endif // GENERATE_OPENCL_CODE

#endif		// CONSTANTS
