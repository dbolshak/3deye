#include <sys_utils.h>
#include <iostream>
#include <string>
#include <sstream>

bool
find_arg ( int argc, char* argv[], const char * arg_name ) {
    // we use find below so we have to convert arg_name to string
    string a_name (arg_name);
    
    bool result = false;
    
    typedef vector<string>::iterator ptr;
    
    vector <string> args ( argv, argv + argc );
    ptr Iter = std::find (args.begin(), args.end(), a_name);

    if ( Iter != args.end() )
        result = true;
    else
        result = false;

    return result;
}
