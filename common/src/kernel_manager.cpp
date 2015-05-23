#include <kernel_manager.h>

kernel_manager :: kernel_manager ( const string & Path, vector <string> & names ) {

    path_to_kernels = Path;
    kernel_names = names;
    Build_All_Kernels();
}

void
kernel_manager :: Build_All_Kernels () {
    for (int i = 0; i < kernel_names.size(); i++ ) {
        opencl_kernel* new_kernel = new opencl_kernel(path_to_kernels +"/"+kernel_names[i]);
        kernels[ kernel_names[i] ] = new_kernel;
#ifdef DEBUG
        cout << kernel_names[i] << " - is build"<< endl;
#endif        
    }
}

opencl_kernel*
kernel_manager :: Get_Kernel (string & kernel_name ) {
    try
    {
        kernels.at(kernel_name)->free_args();

        return kernels[kernel_name];
    }
    catch (...)
    {
        cout << "Kernel " << kernel_name << " Not found!" << endl;
        throw; 
    }

}

opencl_kernel*
kernel_manager :: Get_Kernel (const char * kernel_name ) {
    string cur_name (kernel_name);
    return Get_Kernel (cur_name);
}

string
kernel_manager :: Get_Path() {
    return path_to_kernels;
}
