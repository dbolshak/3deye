#ifndef OPENCL_WRAPPERS
#define OPENCL_WRAPPERS

#include <kernel_manager.h>
#include <base_point_types.h>
#include <round_buffer.h>
#include <matrix.h>
#include <light_source.h>

void Integrate_Tsdf_Data(   gpu_array_base * gDepth_Frame,
                            gpu_array_base * gDepth_Frame_Scaled,
                            gpu_array_base * gTsdf_Color_Volume,
                            intrinsics & Intr,
                            matrix_3f & Rotation_Inverse, 
                            vector_3f & Translation,
                            float Truncation_Distance,
                            point_3f & Cell_Size,
                            round_buffer * Tsdf_Buffer,
                            kernel_manager * Gpu_Methods,
                            int DBG_Frame_Num
                        );

void Raycast (  gpu_array_base * gTsdf_Color_Volume,
                gpu_array_base * Vertex_Map,
                gpu_array_base * Normal_Map,
                round_buffer * Round_Buffer,
                vector_3f & Translation,
                matrix_3f & Rotation,
                float   Truncation_Distance,
                intrinsics & Intr,
                point_3f & Cell_Size,
                opencl_kernel * raycast_kernel
             );


        
void Resize_Vertex_Map (    gpu_array_base * Vertex_Map_In,
                            gpu_array_base * Vertex_Map_Out,
                            intrinsics & Intr,
                            opencl_kernel * resize_vertex_kernel
                       );

void Resize_Normal_Map (    gpu_array_base * Normal_Map_In,
                            gpu_array_base * Normal_Map_Out,
                            intrinsics & Intr,
                            opencl_kernel * resize_normal_kernel
                       );

void Transform_Maps (   gpu_array_base * Vertex_Map_Src,
                        gpu_array_base * Normal_Map_Src,
                        gpu_array_base * Vertex_Map_Dst,
                        gpu_array_base * Normal_Map_Dst,
                        matrix_3f & Rotation,
                        vector_3f & Translation,
                        intrinsics & Intr,
                        opencl_kernel * transform_maps_kernel
                    );

void Integrate_Colors ( gpu_array_base * gTsdf_Color_Volume,
                        gpu_array_base * gRGB_Frame,
                        gpu_array_base * Vertex_Map,
                        intrinsics  &   Intr,
                        matrix_3f   &   Rotation_Inverse,
                        vector_3f   &   Translation,
                        float Truncation_Distance,
                        int Max_Weight,
                        point_3f Cell_Size,
                        round_buffer * Round_Buffer,
                        opencl_kernel * integrate_colors_kernel
                      );

        void Apply_Billateral_Filtration (  gpu_array_base * gDepth, 
                                            const intrinsics & Intr,
                                            float Trunc_Dist,
                                            opencl_kernel * billateral_filtering_kernel,
                                            gpu_array_base * gDepth_Filtered
                                         );

        void Create_Pyramid (   vector <gpu_array_base*> & depth_pyramids, 
                                int Num_Of_Pyramid_Level,
                                intrinsics Intr,
                                kernel_manager * Gpu_Methods
                             );

        void Create_Vertex_Maps (   vector < gpu_array_base * > & depths,
                                    vector < gpu_array_base * > & vertexes,
                                    intrinsics Intr,
                                    int Pyramid_Level_Num,
                                    kernel_manager * Gpu_Methods
                                 );

        void Create_Normal_Maps (   vector < gpu_array_base * > & vertexes,
                                    vector < gpu_array_base * > & normals,
                                    intrinsics Intr,
                                    int Pyramid_Level_Num,
                                    kernel_manager * Gpu_Methods
                                 );

        void Prepare_Data_For_RGBD (    gpu_array_base * gDepth_Frame,
                                        gpu_array_base * gRGB_Frame,
                                        gpu_array_base * gIntensity_Image,
                                        gpu_array_base * gMetric_Depth,
                                        opencl_kernel  * prepare_initial_frames_rgbd );

        void Estimate_Combined (	matrix_3f & Rotation_Current,
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
                               );

        void    Extract_Tsdf_Volume     (   /*const*/ round_buffer * Round_Buffer,
                                            const point_3f & Cell_Size,
                                            gpu_array_base * gTsdf_Volume,
                                            kernel_manager * Gpu_Methods,
                                            matrix <Point_XYZI> & Resulting_Cloud_Parallel
                                        );

        void    Extract_Color_Volume    (   
                                            gpu_array_base * gColor_Volume,
                                            round_buffer * Round_Buffer,
                                            matrix <Point_XYZI> & Point_Cloud,
                                            point_3f Cell_Size,
                                            opencl_kernel * extract_colors_kernel,
                                            matrix <Point_Color> & Resulting_Color_Point_Cloud
                                        );
                                   
        void Extract_Tsdf_Slice (   point_3i Min_Bounds,
                                    point_3i Max_Bounds,
                                    round_buffer * Round_Buffer,
                                    point_3f & Cell_Size,
                                    gpu_array_base * gTsdf_Volume,
                                    kernel_manager * Gpu_Methods,
                                    matrix <Point_XYZI > & Resulting_Tsdf_Cloud
                                 );

        void Extract_Color_Slice (  point_3i Min_Bounds,
                                    point_3i Max_Bounds,
                                    round_buffer * Round_Buffer,
                                    gpu_array_base * gColor_Volume,
                                    kernel_manager * Gpu_Methods,
                                    matrix <Point_Color > & Resulting_Color_Cloud
                                 );


        void Extract_Slice_As_Cloud (
                                        gpu_array_base * gTsdf_Volume,
                                        gpu_array_base * gColor_Volume,
                                        round_buffer * Round_Buffer,
                                        point_3i & Offset,
                                        kernel_manager * Gpu_Methods,
                                        matrix <Point_XYZI>   & Resulting_Points_Cloud, 
                                        matrix <Point_Color>  & Resulting_Colors_Cloud
                                    );
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
        );


        void
        Clean_Tsdf_Slice (     point_3i Min_Bounds,
                        point_3i Max_Bounds,
                        round_buffer * Round_Buffer,
                        gpu_array_base * gTsdf_Volume,
                        opencl_kernel * clean_tsdf_kernel );
        void
        Clean_Color_Slice (     point_3i Min_Bounds,
                        point_3i Max_Bounds,
                        round_buffer * Round_Buffer,
                        gpu_array_base * gColor_Volume,
                        opencl_kernel * clean_color_kernel );
        void
        Extract_Color_Slice (   point_3i Min_Bounds,
                        point_3i Max_Bounds,
                        round_buffer * Round_Buffer,
                        gpu_array_base * gColor_Volume,
                        kernel_manager * Gpu_Methods,
                        matrix <Point_Color> & Resulting_Cloud_Parallel
                    );
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
        );
        
        void
        Generate_Image (    gpu_array_base * Vertex_Map,
                    gpu_array_base * Normal_Map,
                    intrinsics & Intr,
                    light_source & light,
                    gpu_array_base * Image_Out,
                    opencl_kernel * generate_image_kernel
        );

        void
        Clean_Buff ( gpu_array_base * gPtr,
                     opencl_kernel * clean_buff_kernel );

        void
        Generate_Intensity_And_Depth_Pyramid (   
                                        vector < gpu_array_base * > & intensity_pyramid,
                                        vector < gpu_array_base * > & metric_depth_pyramid,
                                        intrinsics Intr,
                                        int Pyramid_Level_Num_RGBD,
                                        kernel_manager * Gpu_Methods
                                    );

        void
        Create_Intensity_Derivatives (
                                vector < gpu_array_base * > & intensity_pyramid,
                                vector < gpu_array_base * > & DI_dx,
                                vector < gpu_array_base * > & DI_dy,
                                intrinsics Intr,
                                int Pyramid_Level_Num_RGBD,
                                kernel_manager * Gpu_Methods);


        void
        Accumulate_Points_Of_Interest_For_RGBD(
                                        gpu_array_base * DI_dx,
                                        gpu_array_base * DI_dy,
                                        float   Depth_Scale,
                                        gpu_array_base * Points_Of_Interest,
                                        gpu_array_base * Num_Of_Points,
                                        int            &    num_of_points,
                                        kernel_manager * Gpu_Methods
        );

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
                                            float sigma_squared,
                                            opencl_kernel   * scan_list_of_valid_correspondence
        );

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
        );

        /* MOVE THIS TO OpenCL WRAPPERs ! */


#endif
