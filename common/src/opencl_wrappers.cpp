#include <opencl_wrappers.h>
#include <constants.h>
// FIXME 
#include <mem_utils.h>
#include <math_utils.h>
#include <prefix_sum_scan.h>

#include <opencl_helper.h>

// for Estimation isometry using RGBD - subroutine for solving LSM
#include <opencv2/core.hpp>
#include <Eigen/Core>
#include <unsupported/Eigen/MatrixFunctions>
#include <Eigen/Dense>
/*
 *          FIXME
 *          you should load kernel from coded strings instead of fields!
 *
 * */

void
Integrate_Tsdf_Data    (    gpu_array_base * gDepth_Frame,
                            gpu_array_base * gDepth_Frame_Scaled,
                            gpu_array_base * gTsdf_Volume,
                            intrinsics & Intr,
                            matrix_3f & Rotation_Inverse,
                            vector_3f & Translation,
                            float Truncation_Distance,
                            point_3f & Cell_Size,
                            round_buffer * Round_Buffer,
                            kernel_manager * Gpu_Methods,
                            int Frame_Num
                        )
{
    LOG_TRACE;

    opencl_kernel* scale_depth = Gpu_Methods->Get_Kernel ("scale_depth.cl");
    scale_depth->add_kernel_arg ( gDepth_Frame );
    scale_depth->add_kernel_arg ( gDepth_Frame_Scaled );
    scale_depth->add_kernel_arg ( & Intr );

    size_t gl_wrk_sz[3] = { Intr.cols, Intr.rows, 0 };
    scale_depth->execute ( gl_wrk_sz, DEFAULT_LOCAL_WORK_SIZE );

    DEBUG_PRINT( "scale_depth !" );

    opencl_kernel* integrate_kernel = Gpu_Methods->Get_Kernel ("integrate_tsdf_volume.cl");
    integrate_kernel->add_kernel_arg( gDepth_Frame_Scaled );
    integrate_kernel->add_kernel_arg( gTsdf_Volume ) ;
    integrate_kernel->add_kernel_arg( & Truncation_Distance);
    integrate_kernel->add_kernel_arg( & Rotation_Inverse);
    integrate_kernel->add_kernel_arg( & Translation);
    integrate_kernel->add_kernel_arg( & Intr);
    integrate_kernel->add_kernel_arg( & Cell_Size);
    integrate_kernel->add_kernel_arg( Round_Buffer );
    
    gl_wrk_sz[0] = Round_Buffer->voxels_size.x;
    gl_wrk_sz[1] = Round_Buffer->voxels_size.y;
    gl_wrk_sz[2] = 0;

    integrate_kernel->execute( gl_wrk_sz, DEFAULT_LOCAL_WORK_SIZE );

    DEBUG_PRINT ( "integrate tsdf" );
}

void
Raycast (
                    gpu_array_base * gTsdf_Volume,
                    gpu_array_base * Vertex_Map,
                    gpu_array_base * Normal_Map,
                    round_buffer * Tsdf_Buffer,
                    vector_3f & Translation,
                    matrix_3f & Rotation,
                    float   Truncation_Distance,
                    intrinsics & Intr,
                    point_3f & Cell_Size,
                    opencl_kernel * raycast_kernel
        )
{
    LOG_TRACE;
    raycast_kernel->add_kernel_arg( gTsdf_Volume );
    raycast_kernel->add_kernel_arg( Vertex_Map );
    raycast_kernel->add_kernel_arg(	Normal_Map );
    raycast_kernel->add_kernel_arg(	& Translation );
    raycast_kernel->add_kernel_arg(	& Rotation );
    raycast_kernel->add_kernel_arg(	& Truncation_Distance );
    raycast_kernel->add_kernel_arg(	& Intr );
    raycast_kernel->add_kernel_arg(	Tsdf_Buffer );
    raycast_kernel->add_kernel_arg(	& Cell_Size );

    size_t global_work_size[3] = { Intr.cols, Intr.rows, 0 };

    raycast_kernel->execute(global_work_size, DEFAULT_LOCAL_WORK_SIZE );

}

void
Resize_Vertex_Map (
                    gpu_array_base * Vertex_Map_In,
                    gpu_array_base * Vertex_Map_Out,
                    intrinsics & Intr,
                    opencl_kernel * resize_vertex_kernel
                )
{
    LOG_TRACE;

    resize_vertex_kernel->add_kernel_arg ( Vertex_Map_In );
    resize_vertex_kernel->add_kernel_arg ( Vertex_Map_Out );

    int width  =   Intr.cols;
    int height  =   Intr.rows;  

    point_2us   image_size = { width, height };

    resize_vertex_kernel->add_kernel_arg ( & image_size );

    size_t gl_wrk_sz[3] = { DEFAULT_STRIDE * height, 0, 0 };
    size_t lc_wrk_sz[3] = { DEFAULT_STRIDE, 0, 0 };

    resize_vertex_kernel->execute ( gl_wrk_sz, lc_wrk_sz );

}

void
Resize_Normal_Map (
                        gpu_array_base * Normal_Map_In,
                        gpu_array_base * Normal_Map_Out,
                        intrinsics & Intr,
                        opencl_kernel * resize_normal_kernel
                    )
{
    LOG_TRACE;

    resize_normal_kernel->add_kernel_arg ( Normal_Map_In );
    resize_normal_kernel->add_kernel_arg ( Normal_Map_Out );
    
    int width = Intr.cols;
    int height =Intr.rows;  

    point_2us   image_size = { width, height };
    resize_normal_kernel->add_kernel_arg ( & image_size );
    size_t gl_wrk_sz[3] = { DEFAULT_STRIDE * height, 0, 0 };
    size_t lc_wrk_sz[3] = { DEFAULT_STRIDE, 0, 0 };
    
    resize_normal_kernel->execute ( gl_wrk_sz, lc_wrk_sz );
}

void
Transform_Maps (
                    gpu_array_base * Vertex_Map_Src,
                    gpu_array_base * Normal_Map_Src,
                    gpu_array_base * Vertex_Map_Dst,
                    gpu_array_base * Normal_Map_Dst,
                    matrix_3f & Rotation,
                    vector_3f & Translation,
                    intrinsics & Intr,
                    opencl_kernel * transform_map_kernel
                )
{
    LOG_TRACE;

    int width = Intr.cols ;
    int height = Intr.rows ;

    point_2us   img_size = { width, height };

    transform_map_kernel->add_kernel_arg( Vertex_Map_Src );	
    transform_map_kernel->add_kernel_arg( Normal_Map_Src );
    transform_map_kernel->add_kernel_arg( & Rotation );
    transform_map_kernel->add_kernel_arg( & Translation );
    transform_map_kernel->add_kernel_arg( & img_size );
    transform_map_kernel->add_kernel_arg( Vertex_Map_Dst );
    transform_map_kernel->add_kernel_arg( Normal_Map_Dst );	

    size_t gl_wrk_sz[3] = { width, height, 0};
 
    transform_map_kernel->execute( gl_wrk_sz, DEFAULT_LOCAL_WORK_SIZE );

}

void
Integrate_Colors (
                                gpu_array_base * gColor_Volume,
                                gpu_array_base * gRGB_Frame,
                                /* gpu_array_base * gColor_Buffer,*/
                                gpu_array_base * gVertex_Map,
                                intrinsics  &   Intr,
                                matrix_3f   &   Rotation_Inverse,
                                vector_3f   &   Translation,
                                float Truncation_Distance,
                                int Max_Weight,
                                point_3f Cell_Size,
                                round_buffer * Round_Buffer,
                                opencl_kernel * integrate_colors
        )
{
    LOG_TRACE;

    integrate_colors->add_kernel_arg( gRGB_Frame );
    integrate_colors->add_kernel_arg( gVertex_Map );
    integrate_colors->add_kernel_arg( gColor_Volume );	
    integrate_colors->add_kernel_arg( & Truncation_Distance );
    integrate_colors->add_kernel_arg( & Max_Weight );
    integrate_colors->add_kernel_arg( & Rotation_Inverse );
    integrate_colors->add_kernel_arg( & Translation );
    integrate_colors->add_kernel_arg( & Intr );
    integrate_colors->add_kernel_arg( & Cell_Size );
    integrate_colors->add_kernel_arg( Round_Buffer );

    size_t gl_wrk_sz[3] = { Round_Buffer->voxels_size.x, Round_Buffer->voxels_size.y, 0 };
     
    integrate_colors->execute( gl_wrk_sz, DEFAULT_LOCAL_WORK_SIZE );

}

void
Apply_Billateral_Filtration (    gpu_array_base * gDepth, 
                                 const intrinsics & Intr,
                                 float Trunc_Dist,
                                 opencl_kernel * bilateral_filtration,
                                 gpu_array_base * gDepth_Filtered
                             )
{
    LOG_TRACE;

    bilateral_filtration->add_kernel_arg ( gDepth );
    bilateral_filtration->add_kernel_arg ( gDepth_Filtered );
    // FIXME because I want something like that add_kernel_arg ( Intr.Get_Size() )
    point_2us you_should_carefully_rethink_your_abstraction_layer = { Intr.cols, Intr.rows };
    bilateral_filtration->add_kernel_arg ( & you_should_carefully_rethink_your_abstraction_layer );
    bilateral_filtration->add_kernel_arg ( & Trunc_Dist );
    // FIXME choose optimal global/local work size ?
    bilateral_filtration->execute ( Intr.cols, Intr.rows, 0 );

}

/*              NOTE
 *                  we use kernel_manager for get kernel because
 *                  Get_Kernel method clean kernel_args for use
 *                  in case simple pointer we have to clean it by hands
 *                  before invokation r every iteration
 *  
 * */
void
Create_Pyramid (    vector <gpu_array_base*> & depth_pyramids , 
                    int Num_Of_Pyramid_Level, 
                    intrinsics Intr,
                    kernel_manager * Gpu_Methods
                )
{
    LOG_TRACE;

    for ( int i = 1; i < Num_Of_Pyramid_Level; ++i ) {

        opencl_kernel * create_pyramid = Gpu_Methods->Get_Kernel ("compute_depth_pyramid.cl");
        create_pyramid->add_kernel_arg ( depth_pyramids [ i-1 ] ); 
        create_pyramid->add_kernel_arg ( depth_pyramids [ i ] ); 

        // FIXME add method to intrinsics ?
        int pyr_cols = Intr.cols >> i;
        int pyr_rows = Intr.rows >> i;

        point_2us bad_long_unconvineint_name_for_rethink_arch = { pyr_cols , pyr_rows }; 
        create_pyramid->add_kernel_arg ( & bad_long_unconvineint_name_for_rethink_arch ); 

        // FIXME choose optimal global/local work size ?
        create_pyramid->execute ( pyr_cols, pyr_rows, 0 );

    }

}

/*              NOTE
 *                  we use kernel_manager for get kernel because
 *                  Get_Kernel method clean kernel_args for use
 *                  in case simple pointer we have to clean it by hands
 *                  before invokation r every iteration
 *  
 * */

void
Create_Vertex_Maps (    vector < gpu_array_base * > & depths,
                        vector < gpu_array_base * > & vertexes,
                        intrinsics Intr,
                        int Pyramid_Level_Num,
                        kernel_manager * Gpu_Methods
                     )
{
    LOG_TRACE;

    for ( int i = 0; i < Pyramid_Level_Num; i++ ) {
       opencl_kernel * create_vertex =  Gpu_Methods->Get_Kernel ("compute_vertex_map.cl");
        
        intrinsics cur_intr = Intr(i);

        create_vertex->add_kernel_arg ( depths[i]);
        create_vertex->add_kernel_arg ( vertexes[i] );
        create_vertex->add_kernel_arg ( & cur_intr );

        // FIXME choose optimal global/local work size ?
        // 3 - is a depth of Vertex and we actually have to fixed!
        size_t gl_wrk_sz[3] = {  cur_intr.cols, cur_intr.rows, 0 };
        create_vertex->execute ( gl_wrk_sz, DEFAULT_LOCAL_WORK_SIZE );
    }
}

/*              NOTE
 *                  we use kernel_manager for get kernel because
 *                  Get_Kernel method clean kernel_args for use
 *                  in case simple pointer we have to clean it by hands
 *                  before invokation r every iteration
 *  
 * */

void
Create_Normal_Maps (    vector < gpu_array_base * > & vertexes,
                        vector < gpu_array_base * > & normals,
                        intrinsics Intr,
                        int Pyramid_Level_Num,
                        kernel_manager * Gpu_Methods
                   )
{
    LOG_TRACE;

    for ( int i = 0; i < Pyramid_Level_Num; i++ ) {
       opencl_kernel * create_normals =  Gpu_Methods->Get_Kernel ("compute_normal_map_eigen.cl");

        create_normals->add_kernel_arg ( vertexes[i] );
        create_normals->add_kernel_arg ( normals[i] );

        // FIXME add method to intrinsics ?
        int pyr_cols = Intr.cols >> i;
        int pyr_rows = Intr.rows >> i;

        point_2us bad_long_unconvineint_name_for_rethink_arch = { pyr_cols , pyr_rows }; 
        create_normals->add_kernel_arg ( & bad_long_unconvineint_name_for_rethink_arch ); 

        // FIXME choose optimal global/local work size ?
        // 3 - is a depth of Vertex and we actually have to fixed!
        size_t gl_wrk_sz[3] = {  pyr_cols, pyr_rows, 0 };
        create_normals->execute ( gl_wrk_sz, DEFAULT_LOCAL_WORK_SIZE );
    }
}

void Prepare_Data_For_RGBD (    gpu_array_base * gDepth_Frame,
                                gpu_array_base * gRGB_Frame,
                                gpu_array_base * gIntensity_Image,
                                gpu_array_base * gMetric_Depth,
                                opencl_kernel  * prepare_initial_frames_rgbd )
{
    point_2i img_size = { gDepth_Frame->width(), gDepth_Frame->height() };
    prepare_initial_frames_rgbd->add_kernel_args (  gDepth_Frame,
                                                    gRGB_Frame,
                                                    gIntensity_Image,
                                                    gMetric_Depth,
                                                    &img_size
                                                    );

    size_t global_work_size[3] = { img_size.x, img_size.y, 0 };

    prepare_initial_frames_rgbd->execute ( global_work_size, DEFAULT_LOCAL_WORK_SIZE );
}

void
Estimate_Combined (	matrix_3f & Rotation_Current,
                    vector_3f & Translation_Current,
                    gpu_array_base * Vertex_Map_Current,
                    gpu_array_base * Normal_Map_Current,
                    matrix_3f & Rotation_Previous_Inverse,
                    vector_3f & Translation_Previous,
                    intrinsics & Intr,
                    gpu_array_base * Vertex_Map_Previous,
                    gpu_array_base * Normal_Map_Previous,
                    float Distance_Threshold,
                    float Angle_Threshold,
                    gpu_array_base * gbuf,
                    gpu_array_base * gMBuf,
                    /* FIXME actually I do not understand meaning of this args */
                    double * A,
                    double * b,	/* OMG 14 params >_< */
                    kernel_manager * Gpu_Methods
			)
{
    LOG_TRACE;

    opencl_kernel* combined_kernel = Gpu_Methods->Get_Kernel ("estimate_combined.cl");
    combined_kernel->add_kernel_arg(    Normal_Map_Current  );
    combined_kernel->add_kernel_arg(	Vertex_Map_Current  );
    combined_kernel->add_kernel_arg(	Normal_Map_Previous );
    combined_kernel->add_kernel_arg(	Vertex_Map_Previous );
    combined_kernel->add_kernel_arg(	& Rotation_Current   );
    combined_kernel->add_kernel_arg(	& Translation_Current );
    combined_kernel->add_kernel_arg(	& Rotation_Previous_Inverse  );
    combined_kernel->add_kernel_arg(	& Translation_Previous );
    combined_kernel->add_kernel_arg(	& Distance_Threshold );
    combined_kernel->add_kernel_arg(	& Angle_Threshold );
    combined_kernel->add_kernel_arg(	& Intr );
    combined_kernel->add_kernel_arg(	gbuf );

    size_t gl_wrk_sz[3] = { Intr.cols,
                            Intr.rows, 
                            0 };
    combined_kernel->execute( gl_wrk_sz, DEFAULT_LOCAL_WORK_SIZE );

    opencl_kernel * transform_estimation = Gpu_Methods->Get_Kernel ("transform_estimation.cl");
    transform_estimation->add_kernel_arg( gbuf );
    transform_estimation->add_kernel_arg( gMBuf );
    int buf_len = DIVUP( Intr.cols, 32) * DIVUP ( Intr.rows, 8);

    transform_estimation->add_kernel_arg( &buf_len );

    // FIXME we need to dynamicaly determine possible value for your kernel
    // and in case it do not correspond to your algo - tune this as a param in kernel!
    size_t loc_wrk_sz[3] =  { DEFAULT_STRIDE, 0, 0 };    // single work-group size would be 512
    
    gl_wrk_sz[0] = DEFAULT_STRIDE * 27;
    gl_wrk_sz[1] = 0; 
    gl_wrk_sz[2] = 0;
    
    transform_estimation->execute( gl_wrk_sz , loc_wrk_sz );

    float mbuf[27];
    memset(mbuf,27,0);
    gMBuf->memcpy_to_host ( mbuf );

    int shift = 0;
    for (int k = 0; k < 6; ++k)  //rows
    {
        for (int j = k; j < 7; ++j)    // cols + b
        {
            float value = mbuf[shift++];
            if (j == 6)       // vector b
                b [k] = value;
            else
                A [j * 6 + k] = A [k * 6 + j] = value;
        }
   }
}

void
Extract_Tsdf_Volume (
                        /*const*/ round_buffer * Round_Buffer,
                        const point_3f & Cell_Size,
                        gpu_array_base * gTsdf_Volume,
                        /* gpu_array_base * gRound_Buffer,*/
                        kernel_manager * Gpu_Methods,
                        matrix<Point_XYZI> & Resulting_Cloud_Parallel
                    )
{
    LOG_TRACE;

    unsigned int Num_Of_Points_In_Cloud_Parallel = 0;
    // compare voxels_size with available memory at the host and choose 

        unsigned int width  =   Round_Buffer->voxels_size.x;
        unsigned int height =   Round_Buffer->voxels_size.y;
        unsigned int depth  =   Round_Buffer->voxels_size.z;

        unsigned int step, num_of_steps;
        int step_divisor = 2;
        determine_extract_step( depth, step, num_of_steps, step_divisor );

        int Max_Size = step;
        int Min_Size = 0;

        unsigned int shift = 0;

        dim3 pred_sz ( 3 * width * height * step);
        gpu_array_base * gPredicats = new gpu_array<unsigned int> ( pred_sz );

        cout << width << " "<< height << " "<< step << " num_of_steps" << num_of_steps << endl;

        for (int i = 0; i < num_of_steps; i++ ) {

            cout << "Extract_Tsdf_Volume - iteration - "<< i << endl;

            opencl_kernel *scan_kernel = Gpu_Methods->Get_Kernel("scan_the_whole_array.cl");
    
            scan_kernel->add_kernel_arg(	gTsdf_Volume );
            scan_kernel->add_kernel_arg(	gPredicats ); 

            dim3 sz_f; 

            scan_kernel->add_kernel_arg(	&Num_Of_Points_In_Cloud_Parallel,
                                            sz_f,
                                            TO_GPU_AND_BACK );
            scan_kernel->add_kernel_arg(	& Min_Size ); 
            scan_kernel->add_kernel_arg(	& Max_Size ); 

            scan_kernel->add_kernel_arg(	Round_Buffer ); 
    
            size_t global_work_size[3] = { width,
                                           height,
                                           0
                                         };

            scan_kernel->execute(global_work_size, DEFAULT_LOCAL_WORK_SIZE );
            cout << "Extract_Tsdf_Volume - Num of point at current iteration - " << Num_Of_Points_In_Cloud_Parallel << endl;

/*END 1st phase */
            if ( Num_Of_Points_In_Cloud_Parallel > 0 ) {

                // call in place scanning
                scan_wrapper (gPredicats , pred_sz, Gpu_Methods);
            
/*END 2nd phase*/

                matrix<Point_XYZI>*  Resulting_Cloud_Parallel_Part = new matrix<Point_XYZI> ( Num_Of_Points_In_Cloud_Parallel );
            
                opencl_kernel * extract_kernel = Gpu_Methods->Get_Kernel("extract_tsdf_cloud.cl");

                extract_kernel->add_kernel_arg(	gTsdf_Volume );
                extract_kernel->add_kernel_arg(	gPredicats );
                extract_kernel->add_kernel_arg(	&Min_Size ); 
                extract_kernel->add_kernel_arg(	&Max_Size ); 
                // FIXME add methods to process appropriately const T *
                extract_kernel->add_kernel_arg(	(point_3f *) & Cell_Size ); 
                extract_kernel->add_kernel_arg(	Round_Buffer );
                extract_kernel->add_kernel_arg(	Resulting_Cloud_Parallel_Part->data, 
                                                Resulting_Cloud_Parallel_Part->Get_Size(),
                                                TO_GPU_AND_BACK/*TO_CPU*/ );

                // extract_kernel->show_args_info();

                extract_kernel->execute(global_work_size, DEFAULT_LOCAL_WORK_SIZE );
    
                // cout << "After kernel extract_tsdf_cloud execution num " << i<< endl;
                Resulting_Cloud_Parallel.extend_from ( *Resulting_Cloud_Parallel_Part );
                shift += Num_Of_Points_In_Cloud_Parallel;
                Num_Of_Points_In_Cloud_Parallel =0;

                delete Resulting_Cloud_Parallel_Part;
            }

            Min_Size += step;
            Max_Size += step;
        }
    Num_Of_Points_In_Cloud_Parallel = shift;
    gPredicats->dev_free();
    
    cout << "Extract_Tsdf_Volume - size of resulting cloud SHOULD be - " << Num_Of_Points_In_Cloud_Parallel << endl;
    cout << "Extract_Tsdf_Volume - but it is - " << Resulting_Cloud_Parallel.Get_Num_Of_Elem() << endl;
}

void
Extract_Color_Volume (
                        gpu_array_base * gColor_Volume,
                        round_buffer * Round_Buffer,
                        matrix <Point_XYZI> & Vertex_Point_Cloud,
                        point_3f Cell_Size,
                        opencl_kernel * extract_color,
                        matrix <Point_Color> & Resulting_Color_Point_Cloud
                     )
{
    LOG_TRACE;

    unsigned int num_of_points = Vertex_Point_Cloud.Get_Num_Of_Elem();

    extract_color->add_kernel_arg(	gColor_Volume );
    extract_color->add_kernel_arg(	Vertex_Point_Cloud.data,
                                    Vertex_Point_Cloud.Get_Size()
                                 );
    extract_color->add_kernel_arg(	& Cell_Size );
    extract_color->add_kernel_arg(	Round_Buffer);
    extract_color->add_kernel_arg(	Resulting_Color_Point_Cloud.data,
                                    Resulting_Color_Point_Cloud.Get_Size(),
                                    TO_GPU_AND_BACK );
    
    extract_color->add_kernel_arg(	& num_of_points );

    size_t work_group_size = least_common_multiple ( num_of_points, DEFAULT_STRIDE );
    
    // extract_color->show_kernel_info();
        
    size_t gl_wrk_sz[3] = { work_group_size, 0, 0 };

    size_t lc_wrk_sz[3] = { DEFAULT_STRIDE, 0, 0 };
    
    extract_color->execute(gl_wrk_sz, lc_wrk_sz);
}

void
Extract_Tsdf_Slice (
                                    point_3i Min_Bounds,
                                    point_3i Max_Bounds,
                                    round_buffer * Round_Buffer,
                                    point_3f & Cell_Size,
                                    gpu_array_base * gTsdf_Volume,
                                    /* gpu_array_base * gRound_Buffer, */
                                    kernel_manager * Gpu_Methods,
                                    matrix<Point_XYZI> & Resulting_Cloud_Parallel
                                )
{
    LOG_TRACE;

    // FIXME any changes for shifted cube center ?

    unsigned int width = Round_Buffer->voxels_size.x;
    unsigned int height = Round_Buffer->voxels_size.y;
    unsigned int depth = Round_Buffer->voxels_size.z;

    unsigned int step, num_of_steps;
    int step_divisor = 4; 
    determine_extract_step( depth, step, num_of_steps, step_divisor );
    
    unsigned int Max_Size = step;
    unsigned int Min_Size = 0;

    unsigned int shift = 0;

    dim3 pred_sz ( 3 * width * height * step);
    gpu_array_base * gPredicats = new gpu_array<unsigned int> ( pred_sz );

	for (int i = 0; i < num_of_steps; i++ ) {

            unsigned int Num_Of_Points_In_Slice_Parallel = 0;
            dim3 sz_f;  // 1,1,1

            opencl_kernel *scan_kernel = Gpu_Methods->Get_Kernel("scan_slice_of_tsdf_volume.cl");

            scan_kernel->add_kernel_args (  gTsdf_Volume,
                                            & Min_Bounds,
                                            & Max_Bounds,
                                            & Min_Size,
                                            & Max_Size,
                                            Round_Buffer,
                                            gPredicats ); 

            scan_kernel->add_kernel_arg( &  Num_Of_Points_In_Slice_Parallel,
                                            sz_f,
                                            TO_GPU_AND_BACK );
    
            size_t global_work_size[3] = {  width, height, 0 };
    
            scan_kernel->execute(global_work_size, DEFAULT_LOCAL_WORK_SIZE );
            cout << "Extract_Tsdf_Slice  num of point at " << i << " iteration is - "<< Num_Of_Points_In_Slice_Parallel << endl;

/* END 1st phase */
            if ( Num_Of_Points_In_Slice_Parallel  > 0 ) {
                //                  II   STAGE
                // call in place scanning
                scan_wrapper (gPredicats , pred_sz, Gpu_Methods);
            
/* END 2nd phase */

                matrix<Point_XYZI>*  Resulting_Cloud_Parallel_Part = new matrix<Point_XYZI> ( Num_Of_Points_In_Slice_Parallel );
            
                opencl_kernel * extract_kernel = Gpu_Methods->Get_Kernel("extract_tsdf_slice_as_cloud.cl");

                extract_kernel->add_kernel_args (	gPredicats,
                                                    gTsdf_Volume,
                                                    &Min_Size,
                                                    &Max_Size,
                                                    & Cell_Size,
                                                    &Min_Bounds,
                                                    &Max_Bounds,
                                                    Round_Buffer
                                                 );

                extract_kernel->add_kernel_arg(	Resulting_Cloud_Parallel_Part->data, 
                                                Resulting_Cloud_Parallel_Part->Get_Size(),
                                                TO_GPU_AND_BACK );

                // extract_kernel->show_args_info();

                extract_kernel->execute(global_work_size, DEFAULT_LOCAL_WORK_SIZE );
    
                Resulting_Cloud_Parallel.extend_from ( *Resulting_Cloud_Parallel_Part );
                shift += Num_Of_Points_In_Slice_Parallel;

                Num_Of_Points_In_Slice_Parallel  = 0;

                delete Resulting_Cloud_Parallel_Part;
        }
        Min_Size += step;
        Max_Size += step;
    } // cycle

    gPredicats->dev_free();
    
    cout << "Extract_Tsdf_Slice - size of resulting cloud SHOULD be - " << shift<< endl;
    cout << "Extract_Tsdf_Slice - but it is - " << Resulting_Cloud_Parallel.Get_Num_Of_Elem() << endl;
}


void
Extract_Color_Slice (   point_3i Min_Bounds,
                        point_3i Max_Bounds,
                        round_buffer * Round_Buffer,
                        gpu_array_base * gColor_Volume,
                        kernel_manager * Gpu_Methods,
                        matrix <Point_Color> & Resulting_Cloud_Parallel
                    )
{
    // FIXME, I guess it wouldn't be optimal for shifting usual slice
    // because of Min Max bounds restrictions
    //  --- we can use it to avoid unnecessary iterations

    // FIXME any changes for shifted cube center ?
    //
    unsigned int width = Round_Buffer->voxels_size.x;
    unsigned int height = Round_Buffer->voxels_size.y;
    unsigned int depth = Round_Buffer->voxels_size.z;

    unsigned int step, num_of_steps;
    int step_divisor = 2; 
    determine_extract_step( depth, step, num_of_steps, step_divisor );

    unsigned int Max_Size = step;
    unsigned int Min_Size = 0;

	dim3 dimensions ( width * height * step );
    
	gpu_array_base *gColor_Val = new gpu_array <Color> ( dimensions );
    gpu_array_base *gArray_Indexes = new gpu_array <unsigned int> ( dimensions ); 

    for (int i = 0; i < num_of_steps; i++ ) {
            //                  I   STAGE

            unsigned int Num_Of_Points_In_Slice_Parallel = 0;
            dim3 sz_f;  // 1,1,1

            opencl_kernel *scan_kernel = Gpu_Methods->Get_Kernel("scan_slice_of_color_volume.cl");
            scan_kernel->add_kernel_arg ( gColor_Volume );
    
            scan_kernel->add_kernel_arg ( & Min_Bounds );
            scan_kernel->add_kernel_arg ( & Max_Bounds );

            scan_kernel->add_kernel_arg ( & Min_Size );
            scan_kernel->add_kernel_arg ( & Max_Size );

            scan_kernel->add_kernel_arg ( Round_Buffer );
            scan_kernel->add_kernel_arg ( gArray_Indexes );
            scan_kernel->add_kernel_arg ( gColor_Val );

            scan_kernel->add_kernel_arg( &  Num_Of_Points_In_Slice_Parallel,
                                            sz_f,
                                            TO_GPU_AND_BACK );
    
            size_t global_work_size[3] = {  width, height, 0 };
    
            scan_kernel->execute(global_work_size, DEFAULT_LOCAL_WORK_SIZE );

            if ( Num_Of_Points_In_Slice_Parallel  > 0 ) {

//                  II   STAGE
                scan_wrapper (gArray_Indexes, width * height * step, Gpu_Methods );

                //                  III STAGE

                // extract_slise_as_cloud()
            
                matrix<Point_Color> *  Resulting_Slice_Parallel = new matrix <Point_Color> (  Num_Of_Points_In_Slice_Parallel );

                opencl_kernel * extract_kernel = Gpu_Methods->Get_Kernel ("extract_color_slice_as_cloud.cl");
    
                extract_kernel->add_kernel_arg( gColor_Val );
                extract_kernel->add_kernel_arg(	gArray_Indexes );
                extract_kernel->add_kernel_arg(	Round_Buffer );
                extract_kernel->add_kernel_arg(	& Min_Size );
                extract_kernel->add_kernel_arg(	Resulting_Slice_Parallel->data,
                                                Resulting_Slice_Parallel->Get_Size(),
                                                TO_GPU_AND_BACK );
    
                global_work_size[WIDTH] = width * height * step;
                global_work_size[HEIGHT] = 0; global_work_size[DEPTH] =0;

                size_t loc_wrk_sz[3] = { DEFAULT_STRIDE, 0, 0 };
    
                extract_kernel->execute ( global_work_size, loc_wrk_sz );

                Resulting_Cloud_Parallel.extend_from ( *Resulting_Slice_Parallel );

                delete Resulting_Slice_Parallel;
            }
            Min_Size += step;
            Max_Size += step;
    } // cycle

    gColor_Val->dev_free();
    gArray_Indexes->dev_free();
}

void
Extract_Slice (
                point_3i Min_Bounds,
                point_3i Max_Bounds,
                round_buffer * Round_Buffer,
                gpu_array_base * gTsdf_Volume,
                gpu_array_base * gColor_Volume,
                point_3f Cell_Size,
                kernel_manager * Gpu_Methods,
                matrix <Point_XYZI_Color> & Slice
        )
{
    matrix <Point_XYZI> Vertex_Cloud;
    matrix <Point_Color> Color_Cloud;

    Extract_Slice ( Min_Bounds,
                    Max_Bounds,
                    Round_Buffer,
                    gTsdf_Volume,
                    gColor_Volume,
                    Cell_Size,
                    Gpu_Methods,
                    Vertex_Cloud,
                    Color_Cloud );

    // FIXME - some how combine both cloud ?
    cerr << "Extract_Slice - add convertor between XYZI RGB to single matrix!"<< endl;
    throw;
}

void
Clean_Color_Slice (     point_3i Min_Bounds,
                        point_3i Max_Bounds,
                        round_buffer * Round_Buffer,
                        gpu_array_base * gColor_Volume,
                        opencl_kernel * clean_color_kernel )
{
    clean_color_kernel->add_kernel_arg ( gColor_Volume ); 
    clean_color_kernel->add_kernel_arg ( & Min_Bounds );
    clean_color_kernel->add_kernel_arg ( & Max_Bounds );
    clean_color_kernel->add_kernel_arg ( Round_Buffer );

    size_t gl_wrk_sz[3] = {
                                Round_Buffer->voxels_size.x,
                                Round_Buffer->voxels_size.y,
                                0
                        };

    clean_color_kernel->execute ( gl_wrk_sz , DEFAULT_LOCAL_WORK_SIZE );
}

void
Clean_Tsdf_Slice (     point_3i Min_Bounds,
                        point_3i Max_Bounds,
                        round_buffer * Round_Buffer,
                        gpu_array_base * gTsdf_Volume,
                        opencl_kernel * clean_tsdf_kernel )
{
    clean_tsdf_kernel->add_kernel_arg ( gTsdf_Volume ); 
    clean_tsdf_kernel->add_kernel_arg ( & Min_Bounds );
    clean_tsdf_kernel->add_kernel_arg ( & Max_Bounds );
    clean_tsdf_kernel->add_kernel_arg ( Round_Buffer ); 

    size_t gl_wrk_sz[3] = {
                                Round_Buffer->voxels_size.x,
                                Round_Buffer->voxels_size.y,
                                0
                        };
    
    clean_tsdf_kernel->execute ( gl_wrk_sz, DEFAULT_LOCAL_WORK_SIZE );
}

void
Extract_Slice (
                point_3i Min_Bounds,
                point_3i Max_Bounds,
                round_buffer * Round_Buffer,
                gpu_array_base * gTsdf_Volume,
                gpu_array_base * gColor_Volume,
                point_3f Cell_Size,
                kernel_manager * Gpu_Methods,
                matrix <Point_XYZI> & Vertex_Slice,
                matrix <Point_Color> & Color_Slice
        )
{
    unsigned int debug_num = 0;

    unsigned int width = Round_Buffer->voxels_size.x;
    unsigned int height = Round_Buffer->voxels_size.y;
    unsigned int depth = Round_Buffer->voxels_size.z;

    unsigned int step, num_of_steps;
    int step_divisor = 4; 
    determine_extract_step( depth, step, num_of_steps, step_divisor );

    unsigned int Max_Size = step;
    unsigned int Min_Size = 0;

	dim3 dimensions ( 4 * width * height * step );
    gpu_array_base *gPredicats		= new gpu_array < unsigned int > ( dimensions ); 

    for (int i = 0; i < num_of_steps; i++ ) {
            //                  I   STAGE
            unsigned int Num_Of_Points_In_Slice_Parallel = 0;
            dim3 sz_f;  // 1,1,1

            opencl_kernel *scan_kernel = Gpu_Methods->Get_Kernel("scan_slice_of_both_volumes.cl");
            scan_kernel->add_kernel_args (  gTsdf_Volume,
                                            gColor_Volume,
                                            & Min_Bounds,
                                            & Max_Bounds,
                                            & Min_Size,
                                            & Max_Size,
                                            Round_Buffer,
                                            gPredicats );
            scan_kernel->add_kernel_arg( &  Num_Of_Points_In_Slice_Parallel,
                                            sz_f,
                                            TO_GPU_AND_BACK );
    
            size_t global_work_size[3] = {  width, height, 0 };
            scan_kernel->execute(global_work_size, DEFAULT_LOCAL_WORK_SIZE );
            if ( Num_Of_Points_In_Slice_Parallel  > 0 ) {
                //                  II   STAGE
                scan_wrapper (gPredicats, dimensions, Gpu_Methods );
                //                  III STAGE
                matrix<Point_XYZI> *  Vertex_Slice_Part = new matrix <Point_XYZI> (  Num_Of_Points_In_Slice_Parallel );
                matrix<Point_Color> *  Color_Slice_Part = new matrix <Point_Color> (  Num_Of_Points_In_Slice_Parallel );

                opencl_kernel * extract_kernel = Gpu_Methods->Get_Kernel ("extract_slice_as_cloud.cl" );

                extract_kernel->add_kernel_args (   
                                                    gPredicats,
                                                    gTsdf_Volume,
                                                    gColor_Volume,
                                                    & Min_Size,
                                                    & Max_Size,
                                                    & Cell_Size,
                                                    & Min_Bounds,
                                                    & Max_Bounds,
                                                    Round_Buffer
                                                );
                extract_kernel->add_kernel_arg(	Vertex_Slice_Part->data,
                                                Vertex_Slice_Part->Get_Size(),
                                                TO_GPU_AND_BACK );
                extract_kernel->add_kernel_arg(	Color_Slice_Part->data,
                                                Color_Slice_Part->Get_Size(),
                                                TO_GPU_AND_BACK );

                extract_kernel->execute ( global_work_size, DEFAULT_LOCAL_WORK_SIZE );
#ifdef DEBUG
                cout << "Extract_Slice OK?"<< endl;
                cout << "Vertex size - " << Vertex_Slice_Part->Get_Num_Of_Elem() << endl;
                cout << "Color size - " << Color_Slice_Part->Get_Num_Of_Elem() << endl;
#endif

                Vertex_Slice.extend_from ( * Vertex_Slice_Part );
                Color_Slice.extend_from ( * Color_Slice_Part );

#ifdef DEBUG
                cout << "After extending ?"<< endl;
                cout << "Vertex size - " << Vertex_Slice.Get_Num_Of_Elem() << endl;
                cout << "Color size - " << Color_Slice.Get_Num_Of_Elem() << endl;
#endif
                delete Vertex_Slice_Part;
                delete Color_Slice_Part;
            }

            Min_Size += step;
            Max_Size += step;

            debug_num += Num_Of_Points_In_Slice_Parallel;
    } // cycle

    cout << " Extract_Slice return; Total sizes are :" << endl;

    cout << Vertex_Slice.Get_Num_Of_Elem() << endl;
    cout << Color_Slice.Get_Num_Of_Elem() << endl;

    gPredicats->dev_free();

}


void
Generate_Image (    gpu_array_base * Vertex_Map,
                    gpu_array_base * Normal_Map,
                    intrinsics & Intr,
                    light_source & light,
                    gpu_array_base * Image_Out,
                    opencl_kernel * generate_image_kernel
                )
{
    LOG_TRACE;

    int width  =   Intr.cols;
    int height  =   Intr.rows;  

    point_2us   img_size = { width, height };

    generate_image_kernel->add_kernel_args (    Vertex_Map,
                                                Normal_Map,
                                                & img_size,
                                                & light,
                                                Image_Out );

    size_t global_work_size[3] = {  width, height, 0 };

    generate_image_kernel->execute ( global_work_size, DEFAULT_LOCAL_WORK_SIZE );
}

void
Clean_Buff (    gpu_array_base * gPtr,
                opencl_kernel * clean_buff_kernel )
{
    int width = gPtr->height(); // yeah, ugly, I know

    clean_buff_kernel->add_kernel_args ( gPtr,
                                        & width );

    size_t global_work_size[3] = { width, 0, 0 };

    clean_buff_kernel->execute ( global_work_size, DEFAULT_STRIDE );
}

void
Generate_Intensity_And_Depth_Pyramid    (   
                                    vector < gpu_array_base * > & intensity_pyramid,
                                    vector < gpu_array_base * > & metric_depth_pyramid,
                                    intrinsics Intr,
                                    int Pyramid_Level_Num_RGBD,
                                    kernel_manager * Gpu_Methods
                                )
{

    for ( int i = 1; i < Pyramid_Level_Num_RGBD; i++ )
    {
        opencl_kernel * generate_intensity_and_depth_pyramid = Gpu_Methods->Get_Kernel ("generate_intensity_and_depth_pyramid.cl");

        // FIXME add method to intrinsics ?
        int dst_width   = Intr.cols >> i;
        int dst_height  = Intr.rows >> i;
        point_2i dst_sizes = { dst_width, dst_height }; 

        generate_intensity_and_depth_pyramid->add_kernel_args ( intensity_pyramid [ i - 1 ],
                                                                metric_depth_pyramid [ i - 1 ],
                                                                intensity_pyramid [ i ],
                                                                metric_depth_pyramid [ i ],
                                                                &dst_sizes );
                                                                
        size_t global_work_size[3] = { dst_width, dst_height, 0 };
        generate_intensity_and_depth_pyramid->execute ( global_work_size,  DEFAULT_LOCAL_WORK_SIZE );
    }

}

void
Create_Intensity_Derivatives (
                                vector < gpu_array_base * > & intensity_pyramid,
                                vector < gpu_array_base * > & DI_dx,
                                vector < gpu_array_base * > & DI_dy,
                                intrinsics Intr,
                                int Pyramid_Level_Num_RGBD,
                                kernel_manager * Gpu_Methods
)
{

        for ( int i = 0; i < Pyramid_Level_Num_RGBD; i++ )
        {
            opencl_kernel * create_intensity_derivatives = Gpu_Methods->Get_Kernel ("create_intensity_derivatives.cl");

            // FIXME add method to intrinsics ?
            int dst_width   = Intr.cols >> i;
            int dst_height  = Intr.rows >> i;
            point_2i dst_sizes = { dst_width, dst_height }; 

            create_intensity_derivatives->add_kernel_args ( intensity_pyramid [ i ],
                                                            DI_dx [ i ],
                                                            DI_dy [ i ],
                                                            &dst_sizes );
                                                                
            size_t global_work_size[3] = { dst_width, dst_height, 0 };
            create_intensity_derivatives->execute ( global_work_size,  DEFAULT_LOCAL_WORK_SIZE );
       
        }
    
}

void
Accumulate_Points_Of_Interest_For_RGBD(
                                        gpu_array_base * DI_dx,
                                        gpu_array_base * DI_dy,
                                        float   Depth_Scale,
                                        gpu_array_base * Points_Of_Interest,
                                        gpu_array_base * Num_Of_Points,
                                        int            &    num_of_points,
                                        kernel_manager * Gpu_Methods
                                      )
{
    point_2i img_size = { DI_dx->width(), DI_dx->height() };
    dim3    dimensions ( img_size.x, img_size.y );

    gpu_array_base * Indexes    = new gpu_array <int>   ( dimensions );
    gpu_array_base * Flags      = new gpu_array <bool>  ( dimensions );

    //          STAGE I - fill supplementary arrays with desired point indexes
    opencl_kernel * scan_for_interest_point = Gpu_Methods->Get_Kernel ( "scan_for_interest_point.cl" );
    scan_for_interest_point->add_kernel_args ( DI_dx, DI_dy, & Depth_Scale, Indexes, Flags, Num_Of_Points, & img_size );

    size_t global_work_size[3] = { img_size.x, img_size.y, 0 };
    scan_for_interest_point->execute ( global_work_size, DEFAULT_LOCAL_WORK_SIZE );

    num_of_points = -1;
    Num_Of_Points->memcpy_to_host ( &num_of_points );
    unsigned int pred_sz = num_of_points;

    if ( pred_sz > img_size.x * img_size.y )
    {
        cerr << "opencl_wrappers.cpp Accumulate_Points_Of_Interest_For_RGBD: " << endl;
        cerr << " pred_sz too big - I guess you have to check kernel " << pred_sz << endl;
        throw;
    }

    //          STAGE II - scan Indexes to get appropriate indexes instead of 1\0 values
    scan_wrapper ( Indexes , pred_sz, Gpu_Methods);
    
    Points_Of_Interest = new gpu_array <point_2i> ( num_of_points );
    
    //          STAGE III - fill resulting array in GPU
    opencl_kernel * extract_interest_points = Gpu_Methods->Get_Kernel ("extract_interest_points.cl");

    extract_interest_points->add_kernel_args ( Flags, Indexes, Points_Of_Interest, &img_size );
    extract_interest_points->execute ( global_work_size, DEFAULT_LOCAL_WORK_SIZE );

    Indexes->dev_free();
    Flags->dev_free();
}

void
Fill_List_Of_Valid_Correspondence_RGBD  (
                                            gpu_array_base * I_Prev,
                                            gpu_array_base * D_Prev,
                                            gpu_array_base * I_Curr,
                                            gpu_array_base * D_Curr,
                                            gpu_array_base * Points_Of_Interest,
                                            float Depth_Threshold,
                                            int Num_Of_Points_Of_Interest,
                                            matrix_3f R,
                                            vector_3f t,
                                            gpu_array_base * Correspondent_Points_At_Prev_Frame,
                                            gpu_array_base * Correspondent_Points_At_Curr_Frame,
                                            gpu_array_base * Correspondence_Diff_Per_Pair,
                                            gpu_array_base * Num_Of_Correspondence,
                                            int & hNum_Of_Correspondence,
                                            float & sigma_squared,
                                            opencl_kernel   * scan_list_of_valid_correspondence
                                        )
{
    sigma_squared = 0.f;
    point_2i img_size = { I_Prev->width(), I_Prev->height() };
    size_t global_work_size[3] = { 
        least_common_multiple ( Num_Of_Points_Of_Interest, DEFAULT_STRIDE ), /* number of local work group with size DEFAULT_STRIDE */

        0,0 };    
    gpu_array_base * Total_Diff = new gpu_array < float > ( global_work_size[0] );

    scan_list_of_valid_correspondence->add_kernel_args  (   /* in params */ 
                                                            I_Prev, D_Prev,
                                                            I_Curr, D_Curr,
                                                            Points_Of_Interest,
                                                            & Num_Of_Points_Of_Interest,
                                                            & R, & t,
                                                            & img_size,
                                                            & Depth_Threshold,
                                                            /* out params  */
                                                            Num_Of_Correspondence,
                                                            Total_Diff,
                                                            Correspondent_Points_At_Prev_Frame,
                                                            Correspondent_Points_At_Curr_Frame,
                                                            Correspondence_Diff_Per_Pair
                                                        );
    

    scan_list_of_valid_correspondence->execute ( global_work_size, DEFAULT_STRIDE );
    
    float * hTotal_Diff = new float [ global_work_size[0] ];

    Num_Of_Correspondence->memcpy_to_host ( & hNum_Of_Correspondence );
    
    for ( int j = 0; j < hNum_Of_Correspondence; j++ ) // max 1200 iterations
        sigma_squared += hTotal_Diff [j] * hTotal_Diff [j];
}

void Calculate_Supplementary_Isometry_Using_Camera_Matrix (
                    matrix_3f & Projected_Rotation,
                    vector_3f & Projected_Translation,
                    intrinsics Intr,    /* for constructing camera matrix  */
                    matrix_3f Current_Rotation,
                    vector_3f Current_Translation
)
{

    matrix_3f Camera_Matrix =   {{
                                    {   Intr.fx,    0,          Intr.cx },
                                    {   0,          Intr.fy,    Intr.cy },
                                    {   0,          0,          1       }
                                }};

    matrix_3f Camera_Matrix_Inverse = inverse ( Camera_Matrix );

    Projected_Rotation = multiply_matrix_3f ( &Camera_Matrix, &Current_Rotation );
    Projected_Rotation = multiply_matrix_3f ( &Projected_Rotation, & Camera_Matrix_Inverse );

    Projected_Translation = multiply ( &Camera_Matrix, &Current_Translation );

}

void
Compute_Least_Squares_Equation  (   /*   In params     */
                                    gpu_array_base * I_Prev,
                                    gpu_array_base * D_Prev,    /* for calculating point cloud */
                                    gpu_array_base * I_Curr,
                                    gpu_array_base * DI_dx,
                                    gpu_array_base * DI_dy,
                                    matrix_3f   R,
                                    vector_3f   t,
                                    gpu_array_base * Correspondence_Prev,
                                    gpu_array_base * Correspondence_Curr,
                                    gpu_array_base * Correspondence_Diff,
                                    int Num_Of_Correspondence,
                                    intrinsics Intr,
                                    float sigma,
                                    /*  Out params  */
                                    float   * A,    /* 6x6 matrix   */
                                    float   * b,    /* 6x1 vector   */
                                    opencl_kernel   *   compute_least_squares_equation
        )
{
    // NOTE in kernel used float sobelScale = 1./8.; // by opencv implementation
    
    int sz = 6 * Num_Of_Correspondence;
   
    // FIXME allocate\deallocate at every iteration. bad. prealloc maximum?
    gpu_array_base * Partial_Indexes        = new gpu_array <float > ( dim3 ( sz ) );
    gpu_array_base * Partial_Indexes_Vector = new gpu_array <float > ( dim3 ( sz ) );

    size_t global_work_size[3] = { 
            least_common_multiple ( Num_Of_Correspondence, DEFAULT_STRIDE ), /* number of local work group with size DEFAULT_STRIDE */
            0,
            0 
    };

    compute_least_squares_equation->add_kernel_args (
                                                        I_Prev, D_Prev,
                                                        I_Curr,
                                                        DI_dx,  DI_dy,
                                                        Correspondence_Prev,
                                                        Correspondence_Curr,
                                                        Correspondence_Diff,                                                        
                                                        & sigma,
                                                        & R, & t,
                                                        & Intr,
                                                        & Num_Of_Correspondence,
                                                        Partial_Indexes,
                                                        Partial_Indexes_Vector 
    );

    compute_least_squares_equation->execute ( global_work_size, DEFAULT_STRIDE );

    // FIXME allocate\deallocate at every iteration. bad.
    float *h_partial_indexes            =   new float [ sz ];
    float *h_partial_indexes_vector     =   new float [ sz ];
    Partial_Indexes->memcpy_to_host ( h_partial_indexes );
    Partial_Indexes_Vector->memcpy_to_host ( h_partial_indexes_vector );

    memset ( A, 0, TRANSFORM_DIM_RGB );
    memset ( b, 0, TRANSFORM_DIM_RGB * TRANSFORM_DIM_RGB );

    for ( int i = 0; i < Num_Of_Correspondence; i++)
    {
        for(int y = 0; y < TRANSFORM_DIM_RGB; y++)
        {
            for(int x = y; x < TRANSFORM_DIM_RGB; x++)
                A [ x ] += h_partial_indexes [ 6 * i + x ] * h_partial_indexes [ 6 * i + y ];
            b [ y ] += h_partial_indexes_vector [ 6 * i + y ];
        }
    }

    for(int y = 0; y < TRANSFORM_DIM_RGB; y++)
        for(int x = y+1; x < TRANSFORM_DIM_RGB; x++)
            A [ y * TRANSFORM_DIM_RGB + x ] = A [ x * TRANSFORM_DIM_RGB + y ];

    Partial_Indexes->dev_free();
    Partial_Indexes_Vector->dev_free();

    delete[] h_partial_indexes;
    delete[] h_partial_indexes_vector;
}

bool 
cv_Compute_Cholesky_Factorization ( float * A, float * b, cv::Mat & ksi )
{
    // hugely rely on OpenCV
    cv::Mat AtA ( TRANSFORM_DIM_RGB, TRANSFORM_DIM_RGB, CV_64FC1, A ),
            AtB ( TRANSFORM_DIM_RGB, 1, CV_64FC1, b );
    const double detThreshold = 1e-6;
    
    double det = cv::determinant(AtA);
    bool solutionExist  =   fabs (det) < detThreshold || cvIsNaN(det) || cvIsInf(det);
    if(!solutionExist)
        return false;

    cv::solve ( AtA, AtB, ksi, cv::DECOMP_CHOLESKY );
    return true;
}

void cv_computeProjectiveMatrix ( const cv::Mat& ksi, matrix_3f & R, vector_3f & t )
{
    CV_Assert ( ksi.size() == cv::Size(1,6)  && ksi.type() == CV_64FC1 );
    const double* ksi_ptr = ksi.ptr<const double>();
    Eigen::Matrix<double,4,4> twist, g;
    twist   <<  0.,             -ksi_ptr[2],    ksi_ptr[1],     ksi_ptr[3],
                ksi_ptr[2],     0.,             -ksi_ptr[0],    ksi_ptr[4],
                -ksi_ptr[1],    ksi_ptr[0],     0,              ksi_ptr[5],
                0.,             0.,             0.,             0.;
    g = twist.exp();

    //eigen2cv(g, Rt);
    // FIXME I'm not sure that this is a correct transform
    R.data[0].x = g (0,0);
    R.data[0].y = g (0,1);
    R.data[0].z = g (0,2);
    R.data[1].x = g (1,0);
    R.data[1].y = g (1,1);
    R.data[1].z = g (1,2);
    R.data[2].x = g (2,0);
    R.data[2].y = g (2,1);
    R.data[2].z = g (2,2);

    t.x =   g ( 0,3);
    t.y =   g ( 1,3);
    t.z =   g ( 2,3);

}

bool
Estimate_Isometry_By_RGBD (
                                vector < gpu_array_base * > & I_Prev,
                                vector < gpu_array_base * > & D_Prev,
                                vector < gpu_array_base * > & I_Curr,
                                vector < gpu_array_base * > & D_Curr,
                                vector < gpu_array_base * > & DI_dx,
                                vector < gpu_array_base * > & DI_dy,
                                intrinsics Intr,
                                float RGBD_Depth_Threshold,
                                int Pyramid_Level_Num_RGBD,
                                matrix_3f   Init_R,
                                vector_3f   Init_t,
                                matrix_3f   &   Estimated_Rotation,
                                vector_3f   &   Estimated_Translation,
                                kernel_manager * Gpu_Methods
)
{
    // create temporary resources - >
    gpu_array_base * Num_Of_Interest_Points = new gpu_array <int> ( SINGLE_ELEMENT );
    gpu_array_base * Num_Of_Correspondence = new gpu_array <int> ( SINGLE_ELEMENT );
    gpu_array_base * Points_Of_Interest;    // NOTE allocated in Accumulate_Point_Of_Interest_For_RGBD
    int hNum_Of_Interest_Points = -1;
    int hNum_Of_Correspondence = - 1;
    float sigma_squared = 0.f;
    float sigma = 0.f;
    // iterative odometry estimation
    matrix_3f R_;
    vector_3f t_;

    //  matrix_3f Init_R =  {{
    //                                      {1,0,0},
    //                                      {0,1,0},
    //                                      {0,0,1}
    //                      }};
    //  vector_3f Init_t = { 0,0,0 };

    float A [ TRANSFORM_DIM_RGB * TRANSFORM_DIM_RGB ];
    float b [ TRANSFORM_DIM_RGB ];

    bool    isOk = false;
    for ( int i = 0; i < Pyramid_Level_Num_RGBD; i++ ) // start from max ?
    {
        intrinsics curr_Intr = Intr(i);

        Accumulate_Points_Of_Interest_For_RGBD ( 
                
                DI_dx[i], DI_dy[i], 
                RGBD_Depth_Threshold,
                Points_Of_Interest, Num_Of_Interest_Points, 
                hNum_Of_Interest_Points, 
                Gpu_Methods
        );

        if ( hNum_Of_Interest_Points <= 0 )
            continue;

        // FIXME here we are introducing an elegant memory leak!
        gpu_array_base * Correspondent_Points_At_Prev_Frame = new gpu_array<point_2i> (hNum_Of_Interest_Points ) ;
        gpu_array_base * Correspondent_Points_At_Curr_Frame = new gpu_array<point_2i> (hNum_Of_Interest_Points ) ;
        gpu_array_base * Correspondence_Diff_Per_Pair = new gpu_array<float> (hNum_Of_Interest_Points ) ;

        Calculate_Supplementary_Isometry_Using_Camera_Matrix ( 
                
                                            R_, t_, curr_Intr,
                                            Init_R, Init_t
        );

        Fill_List_Of_Valid_Correspondence_RGBD (
                
                I_Prev[i], D_Prev[i],
                I_Curr[i], D_Curr[i],
                Points_Of_Interest,
                RGBD_Depth_Threshold,
                hNum_Of_Interest_Points,
                R_, t_,
                Correspondent_Points_At_Prev_Frame,
                Correspondent_Points_At_Curr_Frame,
                Correspondence_Diff_Per_Pair,
                Num_Of_Correspondence,
                hNum_Of_Correspondence,
                sigma_squared,
                Gpu_Methods->Get_Kernel ("scan_list_of_valid_correspondence.cl")
        );
        
        sigma = std::sqrt ( sigma_squared );
        
        Compute_Least_Squares_Equation  (   /*   In params     */
                                    
                I_Prev[i], D_Prev[i],
                I_Curr[i],
                DI_dx[i],  DI_dy[i],
                R_, t_, /*I'm not sure that we should pass those value for opencv version ? */
                Correspondent_Points_At_Prev_Frame,
                Correspondent_Points_At_Curr_Frame,
                Correspondence_Diff_Per_Pair,
                hNum_Of_Correspondence,
                curr_Intr,
                sigma,
                /*  Out params  */
                A,    /* 6x6 matrix   */
                b,    /* 6x1 vector   */
                Gpu_Methods->Get_Kernel("compute_least_squares_equation.cl")
        );

        cv::Mat ksi;
        bool    solutionExist = cv_Compute_Cholesky_Factorization ( A, b, ksi );
        
        if(!solutionExist)
            break;

        matrix_3f   Curr_R;
        vector_3f   Curr_t;

        cv_computeProjectiveMatrix ( ksi, Curr_R, Curr_t );

		//compose - FIXME - I really not sure that we should update them in such way
        vector_3f tmp = multiply ( &Curr_R, &Init_t );
        t_  = add_3f ( tmp , Curr_t );
		R_  = multiply ( Curr_R , Init_R );
        
        isOk = true;

        // some condition of solution existence?
        Points_Of_Interest->dev_free();
        Correspondent_Points_At_Prev_Frame->dev_free();
        Correspondent_Points_At_Curr_Frame->dev_free();
        Correspondence_Diff_Per_Pair->dev_free();
    }

    // free temporary resources ->
    Num_Of_Interest_Points->dev_free();
    Num_Of_Correspondence->dev_free();

    Estimated_Rotation      = R_;
    Estimated_Translation   = t_;

    return isOk;
}
