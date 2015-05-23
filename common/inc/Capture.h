#ifndef CAPTURE
#define CAPTURE

#include <XnOS.h>
#include <XnCppWrapper.h>

#include <base_points.h>
#include <intrinsics.h>

class capture
{
	public:
		capture( const intrinsics & Intr );
		~capture();

		bool Capture_Frame();

		bool Init();
		bool Start();
		bool Stop();
		
        Depth       * Get_Depth_Frame();    
        Color       * Get_RGB_Frame();
		const	intrinsics & Get_Intrinsics() const;
	
	private:
		xn::Context				context;
		xn::EnumerationErrors	errors;
		xn::DepthGenerator		depth_gen;
		xn::DepthMetaData		depth_md;
		xn::ImageGenerator		image_gen;
		xn::ImageMetaData		image_md;

		int width_;
		int height_;

		intrinsics cam_params;

        Depth	*	cur_depth_frame;
        Color   *	cur_rgb_frame;
};

#endif
