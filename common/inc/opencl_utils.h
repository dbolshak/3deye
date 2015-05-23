#ifndef  OPENCL_UTIL
#define OPENCL_UTIL

#ifdef __APPLE__   
#include <OpenCL/opencl.h>   
#else  
#include <CL/cl.h>
#endif 

#include <iostream>
#include <string>
using namespace std;

#ifdef DEBUG
#define DBG( os,var) \
(os) << "DBG: " << __FILE__ << "(" << __LINE__ << ") " \
<< #var << " = [" << (var) << "]" << endl;
#else
#define DBG( os, var) 
#endif

#define CL_CHECK(_expr)                                                      	\
   do										\
   {                                                                         	\
   	cl_int _err = _expr;                                            \
   	if (_err == CL_SUCCESS)                                         \
   		break;                                                  \
    	cout << "OpenCL Error: " << #_expr <<" returned " << _err << endl;	\
    	cout << "File " << __FILE__ << " line " << __LINE__ <<endl;		\
     	print_opencl_error(_err);						\
     	abort();                                                                \
   }										\
   while (0)

// NOTE: decltype is c++11 operator which takes an expression and return a type
// pre-C++11 - typeof

#define CL_CHECK_ERR(_expr)                                                     \
     ({										                                    \
            cl_int _err = CL_INVALID_VALUE;                                     \
            decltype(_expr) _ret = _expr;                                         \
	_err = opencl_routine_return_value;                                         \
     	if (_err != CL_SUCCESS) {                                               \
		cout << "OpenCL Error: " << #_expr << " returned " << (int)_err << endl;\
		cout << "File " << __FILE__ << " line " << __LINE__ <<endl;             \
       print_opencl_error(_err);						                        \
       abort();                                                                 \
     }                                                                          \
     _ret;                                                                      \
     })


void print_opencl_error(int error_code);
string Error_To_String(int error_code);
string get_kernel_source_from_file(string &path_to_kernel_file);
string get_kernel_name_from_file(string &path_to_kernel_file);
const char* oclImageFormatString(cl_uint uiImageFormat);

struct MatchPathSeparator
{
	inline
	bool operator()( char ch ) const
	{
		return ( ch == '/' );
	}
};

#endif	// OPENCL_UTIL
