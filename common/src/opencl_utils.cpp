#include <fstream>
#include <iostream>
#include "opencl_utils.h"
#include <algorithm>
#include <stdlib.h>

void print_opencl_error(int error_code)
{
	string Error_String  = "NONE";
	Error_String = Error_To_String(error_code);
	cout << Error_String<< endl;
}

string Error_To_String(int error_code)
{
	switch (error_code)
	{
		case CL_SUCCESS:                            return "Success!";
		case CL_DEVICE_NOT_FOUND:                   return "Device not found.";
		case CL_DEVICE_NOT_AVAILABLE:               return "Device not available";
		case CL_COMPILER_NOT_AVAILABLE:             return "Compiler not available";
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "Memory object allocation failure";
		case CL_OUT_OF_RESOURCES:                   return "Out of resources";
		case CL_OUT_OF_HOST_MEMORY:                 return "Out of host memory";
		case CL_PROFILING_INFO_NOT_AVAILABLE:       return "Profiling information not available";
		case CL_MEM_COPY_OVERLAP:                   return "Memory copy overlap";
		case CL_IMAGE_FORMAT_MISMATCH:              return "Image format mismatch";
		case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "Image format not supported";
		case CL_BUILD_PROGRAM_FAILURE:              return "Program build failure";
		case CL_MAP_FAILURE:                        return "Map failure";
		case CL_INVALID_VALUE:                      return "Invalid value";
		case CL_INVALID_DEVICE_TYPE:                return "Invalid device type";
		case CL_INVALID_PLATFORM:                   return "Invalid platform";
		case CL_INVALID_DEVICE:                     return "Invalid device";
		case CL_INVALID_CONTEXT:                    return "Invalid context";
		case CL_INVALID_QUEUE_PROPERTIES:           return "Invalid queue properties";
		case CL_INVALID_COMMAND_QUEUE:              return "Invalid command queue";
		case CL_INVALID_HOST_PTR:                   return "Invalid host pointer";
		case CL_INVALID_MEM_OBJECT:                 return "Invalid memory object";
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "Invalid image format descriptor";
		case CL_INVALID_IMAGE_SIZE:                 return "Invalid image size";
		case CL_INVALID_SAMPLER:                    return "Invalid sampler";
		case CL_INVALID_BINARY:                     return "Invalid binary";
		case CL_INVALID_BUILD_OPTIONS:              return "Invalid build options";
		case CL_INVALID_PROGRAM:                    return "Invalid program";
		case CL_INVALID_PROGRAM_EXECUTABLE:         return "Invalid program executable";
		case CL_INVALID_KERNEL_NAME:                return "Invalid kernel name";
		case CL_INVALID_KERNEL_DEFINITION:          return "Invalid kernel definition";
		case CL_INVALID_KERNEL:                     return "Invalid kernel";
		case CL_INVALID_ARG_INDEX:                  return "Invalid argument index";
		case CL_INVALID_ARG_VALUE:                  return "Invalid argument value";
		case CL_INVALID_ARG_SIZE:                   return "Invalid argument size";
		case CL_INVALID_KERNEL_ARGS:                return "Invalid kernel arguments";
		case CL_INVALID_WORK_DIMENSION:             return "Invalid work dimension";
		case CL_INVALID_WORK_GROUP_SIZE:            return "Invalid work group size";
		case CL_INVALID_WORK_ITEM_SIZE:             return "Invalid work item size";
		case CL_INVALID_GLOBAL_OFFSET:              return "Invalid global offset";
		case CL_INVALID_EVENT_WAIT_LIST:            return "Invalid event wait list";
		case CL_INVALID_EVENT:                      return "Invalid event";
		case CL_INVALID_OPERATION:                  return "Invalid operation";
		case CL_INVALID_GL_OBJECT:                  return "Invalid OpenGL object";
		case CL_INVALID_BUFFER_SIZE:                return "Invalid buffer size";
		case CL_INVALID_MIP_LEVEL:                  return "Invalid mip-map level";
		default:									return "Unknown";
	}
}

string get_kernel_source_from_file(string &path_to_kernel_file)
{
        string kernel_source = "NONE";

        ifstream file_with_kernel (path_to_kernel_file.c_str(), ios::in|ios::binary|ios::ate);
        if (file_with_kernel.is_open())
        {
                kernel_source.assign( istreambuf_iterator<char>( file_with_kernel ), ( istreambuf_iterator<char>() ) );
                file_with_kernel.close();
        }
        else
        {
                // something wrong is happen
                // what we are going todo
                cout << "can't read file " << path_to_kernel_file << endl;
                abort();
        }

        return kernel_source;
}

string get_kernel_name_from_file(string &path_to_kernel_file)
{
	string kernel_file_name = "NONE";

	string intermediate_name = "";

        intermediate_name = string(find_if(	path_to_kernel_file.rbegin(), path_to_kernel_file.rend(),
                      			MatchPathSeparator() ).base(),
        				path_to_kernel_file.end() );

	kernel_file_name = intermediate_name.substr( 0, intermediate_name.find_last_of( '.' ) );
	
	return kernel_file_name;
}

// Helper function to get OpenCL image format string (channel order and type) from constant
// *********************************************************************
const char* oclImageFormatString(cl_uint uiImageFormat)
{
    // cl_channel_order 
    if (uiImageFormat == CL_R)return "CL_R";  
    if (uiImageFormat == CL_A)return "CL_A";  
    if (uiImageFormat == CL_RG)return "CL_RG";  
    if (uiImageFormat == CL_RA)return "CL_RA";  
    if (uiImageFormat == CL_RGB)return "CL_RGB";
    if (uiImageFormat == CL_RGBA)return "CL_RGBA";  
    if (uiImageFormat == CL_BGRA)return "CL_BGRA";  
    if (uiImageFormat == CL_ARGB)return "CL_ARGB";  
    if (uiImageFormat == CL_INTENSITY)return "CL_INTENSITY";  
    if (uiImageFormat == CL_LUMINANCE)return "CL_LUMINANCE";  

    if (uiImageFormat == CL_Rx) return "CL_Rx";
    if (uiImageFormat == CL_RGx) return "CL_RGx";
    if (uiImageFormat == CL_RGBx) return "CL_RGBx";
/*  
	// starting from OpenCL 1.2 p which is absent in CUDA 5.5 and below
	// ps God damn you, Nvidia!
  if (uiImageFormat == CL_DEPTH) return "CL_DEPTH";
    if (uiImageFormat == CL_DEPTH_STENCIL) return "CL_STENCIL";
*/
    // cl_channel_type 
    if (uiImageFormat == CL_SNORM_INT8)return "CL_SNORM_INT8";
    if (uiImageFormat == CL_SNORM_INT16)return "CL_SNORM_INT16";
    if (uiImageFormat == CL_UNORM_INT8)return "CL_UNORM_INT8";
    if (uiImageFormat == CL_UNORM_INT16)return "CL_UNORM_INT16";
    if (uiImageFormat == CL_UNORM_SHORT_565)return "CL_UNORM_SHORT_565";
    if (uiImageFormat == CL_UNORM_SHORT_555)return "CL_UNORM_SHORT_555";
    if (uiImageFormat == CL_UNORM_INT_101010)return "CL_UNORM_INT_101010";
    if (uiImageFormat == CL_SIGNED_INT8)return "CL_SIGNED_INT8";
    if (uiImageFormat == CL_SIGNED_INT16)return "CL_SIGNED_INT16";
    if (uiImageFormat == CL_SIGNED_INT32)return "CL_SIGNED_INT32";
    if (uiImageFormat == CL_UNSIGNED_INT8)return "CL_UNSIGNED_INT8";
    if (uiImageFormat == CL_UNSIGNED_INT16)return "CL_UNSIGNED_INT16";
    if (uiImageFormat == CL_UNSIGNED_INT32)return "CL_UNSIGNED_INT32";
    if (uiImageFormat == CL_HALF_FLOAT)return "CL_HALF_FLOAT";
    if (uiImageFormat == CL_FLOAT)return "CL_FLOAT";

    // unknown constant
    return "Unknown";
}

