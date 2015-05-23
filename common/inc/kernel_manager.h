#ifndef KERNEL_MANAGER
#define KERNEL_MANAGER

#include <opencl_kernel.h>
#include <map>

class kernel_manager {

        map < string, opencl_kernel* > kernels;
        string path_to_kernels;
        vector <string> kernel_names;
        void Build_All_Kernels();

    public:
        
        kernel_manager ( const string & Path_To_OpenCL_Kernels, vector <string> & names );
        opencl_kernel* Get_Kernel( string& kernel_name );
        opencl_kernel* Get_Kernel( const char* kernel_name );

        string Get_Path();
};

#endif
