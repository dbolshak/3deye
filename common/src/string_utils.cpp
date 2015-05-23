#include <string_utils.h>
#include <constants.h>
#include <fstream>
#include <math_utils.h>

void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if(from.empty())
		return;
	
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

void
add_opencl_extension ( string & kernel_name )
{
    kernel_name.append ( OPENCL_EXTENSIONS );
}


void
write_isometry_to_file ( const char * file_name, std::vector <Eigen::Affine3d> & data )
{
    if ( ! file_name ) {
        cerr << " string_utils.cpp :: write_isometry_to_file - bad file name ! "<< endl;
        throw;
    }

    ofstream out_file;

    out_file.open ( file_name );
    if ( out_file.is_open() )  {
        for ( int i = 0; i < data.size(); i++ )
        {
            matrix_3f R = Eigen_to_matrix_3f ( data[i].rotation().cast<float>() );
            vector_3f t	= Eigen_to_vector_3f ( data[i].translation().cast<float>() );
            out_file << i << endl;
            out_file << R << endl;
            out_file << t << endl;
        }
    }
    out_file.close();

}

void
write_isometry_to_file ( const char * file_name, std::vector <Eigen::Isometry3d> & data )
{
    if ( ! file_name ) {
        cerr << " string_utils.cpp :: write_isometry_to_file - bad file name ! "<< endl;
        throw;
    }

    ofstream out_file;

    out_file.open ( file_name );
    if ( out_file.is_open() )  {
        for ( int i = 0; i < data.size(); i++ )
        {
            matrix_3f R = Eigen_to_matrix_3f ( data[i].rotation().cast<float>() );
            vector_3f t	= Eigen_to_vector_3f ( data[i].translation().cast<float>() );
            out_file << i << endl;
            out_file << R << endl;
            out_file << t << endl;
        }
    }
   out_file.close();

}
