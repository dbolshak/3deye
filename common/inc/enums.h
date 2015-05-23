#ifndef ENUMS
#define ENUMS
/*
 *     FIXME - we should move enums to separate header - global types.h or something like this
 *    NOTE - description of stream operator for enums - http://stackoverflow.com/questions/5633784/input-from-stream-to-enum-type
 *    http://stackoverflow.com/questions/4013703/cant-overload-for-enum-in-c
 *    */
enum Types_Of_Scan
{
	INCLUSIVE,
	EXCLUSIVE
}; 

enum processing_type { FROM_DIR, FROM_SENSOR };
enum odometry_method {
                    METHOD_ICP = 0,
                    METHOD_FOVIS = 1,
                    METHOD_MIXED = 2,
                    METHOD_DVO = 3,
                    METHOD_RGBD = 4,
                    METHOD_DBG = 111
                };
enum camera_type {
                    DEPTH_CAM = 0,
                    RGB_CAM
                };

enum matrix_creation_type {
                CREATE_AND_FILL_2D_RANDOM_MATRIX,       /* using random dimensions <= DEFAULT_VOLUME_VOXELS* but depth = 1 */
                CREATE_AND_FILL_3D_RANDOM_MATRIX,       /* using random dimensins <= DEFAULT_VOLUME_VOXELS* */
                CREATE_AND_FILL_DEFAULT_RANDOM_MATRIX,   /* using DEFAULT_MATRIX_* dimensions */
                CREATE_AND_FILL_DEFAULT_DEPTH_IMAGE,    /*  using DEFAULT_MATRIX_* but depth =1    */
                CREATE_AND_FILL_RANDOM_DEPTH_IMAGE,     /* width, height - even  depth =1 */
                CREATE_AND_FILL_DEFAULT_TSDF_VOLUME     /* using DEFAULT_VOLUME_* dimensions     */
        };                                
                                
enum size_dimensions {
				WIDTH,
				HEIGHT,
				DEPTH
			};

enum gpu_mem_type {
                LOCAL,
                GLOBAL,
                PRIVATE, /* Used for scalar kernel args */
                CONSTANT,
                IMAGE

};

enum gpu_transfer_type {
				TO_NONE,    /* not set - usually mean error  */ 
				TO_CPU,     /* alloc at gpu, download to cpu after kernel invokation  */
				TO_GPU,     /* alloc at gpu, transfer data and forget about it till free */
				TO_GPU_AND_BACK,    /* obvious  */
				IN_GPU_USE_ONLY	/* it is for cases when we do not invoke the whole copying routine - just process data which alreaady at the GPU, NOTE - it is YOU who are responsible for alloc/free - use with care */
			};

enum gpu_data_type {
				TYPE_NONE,
				BUFFER_1D,
				IMAGE_2D,
                CONSTANT_BUFFER
			};

enum profiling_level 	{	
				DISABLE_PROFILING, 
				ENABLE_PROFILING
			};

enum MATRIX_TYPE {
			DEPTH_MAP,	/* ushort  */
			VERTEX_MAP,	/* float */
			NORMAL_MAP,	/* float */
			RGB_MAP		/* triplets of uchar for every point */
		};
#endif
