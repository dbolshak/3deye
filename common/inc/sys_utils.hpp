#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
using namespace std;

/*
 *      possible improvements and enchantments:
 *      http://stackoverflow.com/questions/865668/parse-command-line-arguments
 *
 *      FIXME 
 *
 *      using this overload form: lead to ambiguous errors
 *      find_and_set_arg ( int argc, char* argv[], string & arg_name, T& target_arg ) {
 *
 * */

template <class T>
bool
find_and_set_arg ( int argc, char* argv[], const char * arg_name, T& target_arg ) {
    // we use find below so we have to convert arg_name to string
    string a_name (arg_name);
    
    bool result = false;
    
    typedef vector<string>::iterator ptr;
    
    vector <string> args ( argv, argv + argc );
    ptr Iter = std::find (args.begin(), args.end(), a_name);

    if ( Iter++ != args.end()  && Iter != args.end() ) {

        istringstream in_stream ( *Iter );
        in_stream >> target_arg;

        result = true;
    }
    else
        result = false;

    return result;
}
