#include <stdio.h>

#include <Capture.h>

#ifdef WIN32
#include <math.h>
using namespace std;
#endif
#define CHECK_STATUS(rc, msg) if((rc) != XN_STATUS_OK) { \
  fprintf(stderr, "%s: %s\n", (msg), xnGetStatusString(rc)); return false; }

Depth * 
capture	::	Get_Depth_Frame () {

    Depth * Depth_Frame = new Depth [ width_ * height_ ];
    memcpy ( Depth_Frame, cur_depth_frame, width_ * height_ * sizeof ( Depth ) );

	return Depth_Frame;
}

Color * 
capture	:: Get_RGB_Frame () {
    Color * Color_Frame = new Color [ width_ * height_ ];
    memcpy ( Color_Frame , cur_rgb_frame ,  width_ * height_ * sizeof (Color) );

	return Color_Frame;
}

const intrinsics & 
capture	:: Get_Intrinsics() const {
	return cam_params;
}

capture::capture ( const intrinsics & Intr ) {
	width_ = Intr.cols;
	height_ = Intr.rows;

	cam_params = Intr;

	cur_depth_frame = new Depth [ width_ * height_ ];
	cur_rgb_frame = new Color [ width_ * height_ ];
}

capture::~capture() {
	delete[] cur_depth_frame;
	delete[] cur_rgb_frame;
}

bool
capture::Init() {
	XnStatus rc = context.Init();
	CHECK_STATUS(rc, "Initializing device context");

	printf("Initializing image stream\n");
	image_gen.Create(context);
	rc = image_gen.Create(context);
	CHECK_STATUS(rc, "Initializing image stream");

	// set output format to RGB
	image_gen.SetPixelFormat(XN_PIXEL_FORMAT_RGB24);

	XnMapOutputMode image_mode;
	image_mode.nXRes = width_;
	image_mode.nYRes = height_;
	image_mode.nFPS = 30;
	image_gen.SetMapOutputMode(image_mode);
	CHECK_STATUS(rc, "Setting image output mode");


	printf("Initializing depth stream\n");
	rc = depth_gen.Create(context);
	CHECK_STATUS(rc, "Initializing depth stream");

	depth_gen.SetMapOutputMode(image_mode);
	CHECK_STATUS(rc, "Setting depth output mode");

	depth_gen.GetMetaData(depth_md);
	printf("Depth offset: %d %d\n", depth_md.XOffset(), depth_md.YOffset());
	// XXX do we need to do something with the depth offset?

	// set the depth image viewpoint
	depth_gen.GetAlternativeViewPointCap().SetViewPoint(image_gen);

	// read off the depth camera field of view.  This is the FOV corresponding to
	// the IR camera viewpoint, regardless of the alternative viewpoint settings.
	XnFieldOfView fov;
	rc = depth_gen.GetFieldOfView(fov);
	return true;
}

bool
capture::Start() {
	// start data capture
	printf("Starting data capture\n");
	XnStatus rc = context.StartGeneratingAll();
	CHECK_STATUS(rc, "Starting data capture");
	return true;
}

bool
capture::Stop() {
	context.StopGeneratingAll();
	context.Shutdown();
	return true;
}

bool
capture::Capture_Frame() {
	// Read a new frame
	XnStatus rc = context.WaitAndUpdateAll();
	CHECK_STATUS(rc, "Reading frame");

	// grab the image data
	image_gen.GetMetaData(image_md);
	const XnRGB24Pixel* rgb_data = image_md.RGB24Data();

	int num_rgb_pixels = width_ * height_;
	for(int i=0; i<num_rgb_pixels; i++) {
        cur_rgb_frame[i].r = rgb_data->nRed;
        cur_rgb_frame[i].g = rgb_data->nGreen;
        cur_rgb_frame[i].b = rgb_data->nBlue;
        cur_rgb_frame[i].w = 1;
        rgb_data++;
    }

	// grab the depth data
	depth_gen.GetMetaData(depth_md);
	// FIXME
	cur_depth_frame = ( Depth * ) depth_md.Data();

	return true;
}