#include <typeinfo> 
#include <sstream>
#include <fstream>

// TODO: re-write all using C++11

#include <math.h>   // fabs etc 
#include <string.h> // memcpy
#include <stdlib.h> // rand

//---------------------------------------------------------------------------------

template < class T >
matrix<T>& 
matrix<T> :: operator= ( const matrix * rhs)
{
    if ( rhs == this )
        return *this;
    Set_Dimensions( rhs->Get_Dimensions() );
	memcpy(data, rhs->data,sizeof(T) * sizes.width * sizes.height * sizes.depth);

    return this;
}

//---------------------------------------------------------------------------------

template < class T >
T &
matrix<T>::operator[] (const size_t index)
{
    if ( index >= sizes.width * sizes.height * sizes.depth ) {
        cerr << "matrix operator[] mismatch indexes and actual size of data:" << endl;
        cout << "index - " << index << "dimenions - " << sizes.width << " x " << sizes.height << " x " << sizes.depth  << endl;
        throw;
    }
    return data[index];
}
//---------------------------------------------------------------------------------

template < class T >
T &
matrix<T>::operator[] (const size_t index) const
{
    if ( index >= sizes.width * sizes.height * sizes.depth ) {
        cerr << "matrix operator[] mismatch indexes and actual size of data:" << endl;
        cout << "index - " << index << "dimenions - " << sizes.width << " x " << sizes.height << " x " << sizes.depth  << endl;
        throw;
    }
    return data[index];
}

//---------------------------------------------------------------------------------

template <class T>
matrix<T> :: matrix (const size_t * initial_dimensions) : data(NULL)
{
    if (!initial_dimensions) {
        cerr << "matrix constructor - invalid pointer on initial sizes" << endl;
        throw;
    }
	Set_Dimensions( initial_dimensions[WIDTH], initial_dimensions[HEIGHT], initial_dimensions[DEPTH]);
}

//---------------------------------------------------------------------------------

template <class T>
matrix<T> :: matrix ( dim3 initial_dimensions) : data(NULL)
{
    if ( initial_dimensions.width < 0 ||
         initial_dimensions.height< 0 ||
         initial_dimensions.depth < 0 )
    {        
        cerr << "Check matrixes dimensions - I can't not handle negative sizes :("
            << endl;
        throw;
    }

	Set_Dimensions( initial_dimensions.width, initial_dimensions.height, initial_dimensions.depth );
}

//---------------------------------------------------------------------------------

template <class T>
matrix <T> :: matrix(size_t new_width, size_t new_height, size_t new_depth) : data(NULL)
{
	Set_Dimensions(new_width, new_height, new_depth);
}

//---------------------------------------------------------------------------------

template <class T>
matrix <T> :: matrix(matrix_creation_type fill_with_random_data, size_t dimension ) : data ( NULL )
{
    size_t new_width = dimension; 
    size_t new_height = dimension;
    size_t new_depth  = dimension;
    
    if (dimension == 0)
    {
        switch ( fill_with_random_data )
        {
            case CREATE_AND_FILL_2D_RANDOM_MATRIX:
                 {
                    new_width     =     abs ( (rand() % DEFAULT_VOLUME_VOXELS_WIDTH ) + 1 ); 
                    new_height    =     abs ( (rand() % DEFAULT_VOLUME_VOXELS_HEIGHT ) + 1 ); 
                    new_depth     =     1; 
                 }
                 break;
            case CREATE_AND_FILL_3D_RANDOM_MATRIX:
                 {
                    new_width     =   abs ( (rand() % DEFAULT_VOLUME_VOXELS_WIDTH ) + 1 ); 
                    new_height    =   abs ( (rand() % DEFAULT_VOLUME_VOXELS_HEIGHT ) + 1 ); 
                    new_depth     =   abs ( (rand() % DEFAULT_VOLUME_VOXELS_DEPTH ) + 1 );
                 }
                 break;
            case CREATE_AND_FILL_DEFAULT_RANDOM_MATRIX:
                 {
                    new_width   =   DEFAULT_MATRIX_WIDTH;
                    new_height  =   DEFAULT_MATRIX_HEIGHT;
                    new_depth   =   DEFAULT_MATRIX_DEPTH;
                 }
                 break;
            case CREATE_AND_FILL_DEFAULT_DEPTH_IMAGE:
                 {
                    new_width   = DEFAULT_MATRIX_WIDTH;
                    new_height  = DEFAULT_MATRIX_HEIGHT;
                    new_depth   = 1;
                 }
                 break;
            case CREATE_AND_FILL_RANDOM_DEPTH_IMAGE:
                 {
                    new_width = abs ( ( rand() % DEFAULT_MATRIX_WIDTH ) + 1 ) ; 
                    if ( (new_width % 2) != 0 )		// FIXME
                        new_width++;

                    new_height = abs ( ( rand() % DEFAULT_MATRIX_HEIGHT ) + 1 );
                    if ( (new_height % 2) != 0 )
                        new_height++;

                    new_depth = 1;
                 }
                 break;
            case CREATE_AND_FILL_DEFAULT_TSDF_VOLUME:
                 {
                    new_width   = DEFAULT_VOLUME_VOXELS_WIDTH ;
                    new_height  = DEFAULT_VOLUME_VOXELS_HEIGHT;
                    new_depth   = DEFAULT_VOLUME_VOXELS_DEPTH ;
                 }
                 break;
        };        
    }

	Set_Dimensions(new_width, new_height, new_depth);


    // FIXME - change on predicate function
    if (    ( fill_with_random_data == CREATE_AND_FILL_DEFAULT_RANDOM_MATRIX    ) || 
            ( fill_with_random_data == CREATE_AND_FILL_2D_RANDOM_MATRIX         ) || 
            ( fill_with_random_data == CREATE_AND_FILL_3D_RANDOM_MATRIX         ) ||
            ( fill_with_random_data == CREATE_AND_FILL_DEFAULT_TSDF_VOLUME     ) || 
            ( fill_with_random_data == CREATE_AND_FILL_DEFAULT_DEPTH_IMAGE )
       )
        Fill_With_Random_Data();
}

//---------------------------------------------------------------------------------

template <class T>
matrix<T>::matrix() : data(NULL)
{
	Set_Dimensions(0,0,0);
}

//---------------------------------------------------------------------------------
template <class T>
void
matrix<T> :: add_from (const matrix<T> & matr, size_t shift, size_t how_many )
{
    size_t curr_size = Get_Num_Of_Elem() ;
//    cout << "cur_size = " << Get_Num_Of_Elem() << endl;

    size_t exx_size = how_many == 0 ? matr.Get_Num_Of_Elem() : how_many;
//    cout << " new_size" << exx_size << " shift " << shift <<endl;
    if ( curr_size < (exx_size + shift) ) {
        cerr << "You are trying to add "<< exx_size  <<" elements to matrix with size " 
            << curr_size << " but use shift - "<< shift << endl;
        throw;
    }
    for (int i = 0; i< exx_size;i++)
        data[i+shift] = matr[i];
}

//---------------------------------------------------------------------------------
template <class T>
void
matrix<T> :: compact ( size_t new_size )
{
    size_t old_size = Get_Num_Of_Elem();

    Set_Dimensions ( new_size );

#ifdef DEBUG
    cout << "Should compact from "<< old_size*sizeof(T) << " to " << new_size*sizeof(T) << endl;
#endif  

    T * p = ( T * ) realloc ( data, new_size * sizeof(T) );
    if ( !p ) {
        cerr << "Can't compact matrix from " << old_size * sizeof ( T ) << " to " << new_size * sizeof ( T ) << endl;
        throw;
    }
    data = p ;
}
//---------------------------------------------------------------------------------
/*
 *      NOTE that in case realloc failed you will loose all your data
 *      it is a bit pity, so you have to use mapping to file
 *      but it is look like difficult because you have to work with bytes and not strings :(
 *      probably matrix can be written in binary format?
 * */
template <class T>
void
matrix<T>::extend_from (const matrix<T> & matrix_with_additional_data )
{
    // FIXME: NEVER do it in your homework like this!
    // better to use vector
    size_t cur_size = Get_Num_Of_Elem();
    size_t exx_size = matrix_with_additional_data.Get_Num_Of_Elem();
    size_t new_size = cur_size + exx_size;

#ifdef DEBUG    
    cout << "old_size - " << cur_size << " additional_size -  " << exx_size << " new_size - " << new_size << endl;
#endif    

    if ( exx_size == 0 )
        return;        

    if ( cur_size == 0) {
        Set_Dimensions( matrix_with_additional_data.Get_Dimensions() );
    } else {
        if ( Get_Height() == matrix_with_additional_data.Get_Height() && 
             Get_Depth() == matrix_with_additional_data.Get_Depth() && 
             Get_Height() == Get_Depth() == 1 
        )
        {
            sizes.width = new_size;
        }
        else    {
            cerr << "Invalid usage of extend_from function! - height and depth of both matreixes should be equal to 1!"<< endl;
            cerr << "Current matrix - "<< sizes.width << "x" << sizes.height << "x"<< sizes.depth << endl;
            cerr << "Additional matrix - "<< matrix_with_additional_data.Get_Width() << "x" << matrix_with_additional_data.Get_Height() << "x"<< matrix_with_additional_data.Get_Depth() << endl;
            throw;
        }
   }

#ifdef DEBUG    
    cout << "check htop"  << endl;
    getchar();
#endif

    // it should preserve existent data
    T* p  = (T*) realloc ( data, new_size * sizeof(T) );
    if ( p == NULL)
    {
        cout << "realloc failed for  "<< new_size * sizeof(T) << endl;
        throw;
    }
    data = p;
    memcpy ( data + cur_size, matrix_with_additional_data.data, exx_size * sizeof(T) ); // save current data

}

//---------------------------------------------------------------------------------

template <class T>
void
matrix<T>::allocate_buffer()
{
#ifdef DEBUG    
    cout << "try to alloc - "<< sizes.width * sizes.depth * sizes.height * sizeof(T) << endl;
#endif
    // NOTE - no constructor!
    size_t total_size = sizes.depth * sizes.height * sizes.width * sizeof(T);
    data = (T*) malloc ( total_size );
    if (data==NULL)
    {
        cerr << "matrix - can't alloc :( for " << total_size << endl;
        throw;
    }
}
//---------------------------------------------------------------------------------

template <class T>
void
matrix<T>::free_buffer()
{
    free (data);
    data = NULL;
}

//---------------------------------------------------------------------------------

template <class T>
matrix <T> ::~matrix()
{
    free_buffer();
}

//---------------------------------------------------------------------------------


template <class T>
matrix <T> ::matrix( const matrix<T> & object_for_copy) : data( NULL )
{
    // FIXME yeah yeah it is not c++ style - you should use copy constructor for every object and so on
    // but I think that this should be quicker ( and buggy ) 
    Set_Dimensions ( object_for_copy.Get_Width(), object_for_copy.Get_Height(), object_for_copy.Get_Depth() );
	memcpy(data, object_for_copy.data, sizeof(T) * Get_Num_Of_Elem() );
}

//---------------------------------------------------------------------------------
template <class T>
void
matrix <T> ::Set_Dimensions( const dim3 & new_dimensions )
{
    if ( new_dimensions.width < 0 || new_dimensions.height < 0 || new_dimensions.depth < 0 ) {
        cerr << "Try to set dimensions for matrix with invalid size_t object :( "<< endl;
        throw;
    }

    Set_Dimensions( new_dimensions.width, new_dimensions.height, new_dimensions.depth );
}

//---------------------------------------------------------------------------------

template <class T>
void
matrix <T> ::Set_Dimensions(size_t new_width, size_t new_height, size_t new_depth)
{
	if ( ( sizes.width < 0 ) || ( sizes.height < 0 ) || ( sizes.depth < 0 ) ) {
		cout << "new matrix size are below zero, I will die hard :(" << endl;
		cout << "width = " << sizes.width << " height = " << sizes.height << " depth = " << sizes.depth << endl;
		throw;
	}

	if ( data == NULL) {
		sizes.width = new_width;
        sizes.height = new_height;
		sizes.depth = new_depth;
#ifdef DEBUG 
		cout << "matrix::Set_Dimensions = "<< "width = " << sizes.width << " height = " << sizes.height << " depth = " << sizes.depth << endl;
#endif        
		if ( (sizes.width > 0) && (sizes.height > 0 ) && (sizes.depth > 0)) 
			allocate_buffer();
	} else
	if ( ( new_width == sizes.width ) && ( new_height == sizes.height ) && ( new_depth == sizes.depth ) ) {
		// as we already allocated buffer width appropriate dimensions
		// we should do nothing
		cout << "matrix::Set_Dimensions WTF ?!" << endl;
	} else {
		cout << "old and new matrix size are mismatched! I will die hard :(" << endl;
		cout << "width = " << sizes.width << " height = " << sizes.height << " depth = " << sizes.depth << endl;
		cout << "new_width = " << new_width << " new_height " << new_height << "  new_depth " << new_depth << endl;

		throw ;		/* TODO we should re- or de- allocate existing bufferÂ  */
	}
}

//---------------------------------------------------------------------------------

template <class T>
void
matrix <T> ::Fill_With_Data(size_t new_width, size_t new_height, size_t new_depth, T* new_data)
{
	// FIXME add more warning's watchers
	if ( new_width != sizes.width || new_height != sizes.height )
		throw;
	if (!data)
		throw;

    Set_Dimensions ( new_width, new_height, new_depth );
    
    memcpy (data, new_data, Get_Num_Of_Elem() * sizeof(T));
}

//---------------------------------------------------------------------------------

template <class T>
void
matrix <T> :: Fill_With_Random_Data()
{
	int actual_height = Get_Num_Of_Elem();
	
	for (int i=0; i < actual_height; i++) {
		data[i] = T::Get_Random_Value() ;
    }
}

//---------------------------------------------------------------------------------

template <class T>
istream&
operator>> (istream& in, matrix<T> &this_object)
{
	// FIXME
	cout << "matrix operator << isn't supported :(" << endl;
	return in;
}

//---------------------------------------------------------------------------------

template <class T> 
ostream&
operator<< (ostream &out, const matrix<T> &this_object)
{
	int height = this_object.Get_Height();
	int width = this_object.Get_Width();
	int depth = this_object.Get_Depth();

	if ( ( 0 > width ) || ( 0 > height ) || ( 0 > depth ) )
	{
		out << "incorrect size of matrix:" << endl;
		out << " width  - " << width << endl;
		out << " height - " << height << endl;
		out << " depth - " << depth << endl;
		return out; 
	}

	out << width << " " << height << " " << depth << " " << this_object.Get_Matrix_Type() << endl;

	int actual_height = depth * height;

	for (int row_count = 0; row_count < actual_height; row_count++)
	{
		for (int column_count =0; column_count < width; column_count++)
		{
			out << this_object.data[row_count*width + column_count] << " ";
		}
		out << endl;
	}

	return out;
}

//---------------------------------------------------------------------------------

template <class T>
string
matrix <T> :: Get_Matrix_Type() const
{
    return typeid(data).name();
}

//---------------------------------------------------------------------------------

template <class T>
void
matrix <T> :: Save_To_File(const string & custom_file_name) const
{
	string file_name = "";

	if (custom_file_name.empty() )
	{
		stringstream generated_file_name;
		generated_file_name << sizes.width << "x" << sizes.height << "x" << sizes.depth << "-"<< Get_Matrix_Type()  <<".tst";
		file_name = generated_file_name.str();
	}
	else
		file_name.assign(custom_file_name);

	ofstream file_with_matrix;
	file_with_matrix.open ( file_name.c_str() );
	if (file_with_matrix.is_open())
	{
		file_with_matrix << *this;
	}
}

//---------------------------------------------------------------------------------

template <class T>
bool
matrix <T> :: Is_Valid_Data_Type( const string& matrix_type_name_from_file) const
{
	// FIXME
	return true;
}


//---------------------------------------------------------------------------------


template <class T>
int
matrix <T> :: Fill_From_File(const string & file_name)
{
	if (file_name.empty() )
	{
		cout << "Error: Empty file name" << endl;
		return -1;
	}
	
	ifstream file_with_data ( file_name.c_str() );
	if (file_with_data.is_open())
	{
		string head_line;
		int matrix_width, matrix_height,matrix_depth;
		string matrix_type;

		getline( file_with_data, head_line);

		istringstream in(head_line);
		
		in >> matrix_width >> matrix_height >> matrix_depth >> matrix_type;

		if ( !Is_Valid_Data_Type(matrix_type) )
		{
			
			cout << "Incorrect matrix type!" << endl << matrix_type << " - should be" << endl;
			return -1;
		}
		
		Set_Dimensions(matrix_width, matrix_height, matrix_depth);

		int num_of_current_row = 0;
		int num_of_current_col = 0;
		string	current_line;

		while ( getline (file_with_data, current_line) ) {
			istringstream in(current_line);

			num_of_current_col = 0;
			T	Cur_Value;
			while ( in >> Cur_Value ) {
				data[ num_of_current_row * sizes.width + num_of_current_col++ ] = Cur_Value;
            }

			if (num_of_current_col != sizes.width ) {
				cout << "mismatch : "  << num_of_current_col << " " << sizes.width << endl;
				return -1;
			}
			num_of_current_row ++;
		}
		file_with_data.close();
	}
	else {
		cout << "unable to open file - " << file_name << endl;
		return -1;
	}
	return 0;
}

//---------------------------------------------------------------------------------

template <class T>
bool
matrix<T> :: operator == (matrix<T> &rhs)
{
	if (( rhs.Get_Width()  != Get_Width()  ) || 
		( rhs.Get_Height() != Get_Height() ) ||
		( rhs.Get_Depth()  != Get_Depth()  ) 
	   )
		return false;
    size_t sz = Get_Num_Of_Elem();
	for (size_t x = 0; x < sz; x++) {
		if ( data[x].Get_Value() !=rhs.data[x].Get_Value()  )
		return false;
	}
	return true;
}


//---------------------------------------------------------------------------------

template <class T>
float
matrix<T> :: Estimate_Difference (matrix<T> &matrix_for_compare, float threshold)
{
	if ((matrix_for_compare.Get_Width() != Get_Width() ) || 
		(matrix_for_compare.Get_Height()!= Get_Height() ) ||
		(matrix_for_compare.Get_Depth() != Get_Depth() ) )
	{
		cout << "Shame on me, but I can't compare mismatched matrixes"<< endl;
		throw;	
	}

	float global_difference = 0.f;
    size_t sz =  Get_Num_Of_Elem(); 
	for (size_t x = 0; x < sz; x++)
	{
		// FIXME - case when they are equal but have different signs ?
        // we should fix it for every child class and do not break 
        // the overwhole functionality
		float cur_dif = fabs (matrix_for_compare.data[x] - data[x]);
			
		if ( cur_dif > threshold)
		{
			cout << "x - "<< x << " "<< matrix_for_compare.data[x] << " - " << data[x] << " = " << cur_dif << endl;
			global_difference += cur_dif;
		}
	}
	return global_difference;
}

//---------------------------------------------------------------------------------

template <class T>
template <class U>
float
matrix<T> :: Estimate_Absolute_Difference (matrix<U> &matrix_for_compare, float threshold)
{
	float global_difference = 0.f;

    float val_1 = 0;
    size_t sz = Get_Num_Of_Elem(); 
    for ( size_t x = 0; x < sz; x++ )
        val_1 = val_1 + data[x];

    float val_2 = 0;
    for ( int x = 0; x < matrix_for_compare.Get_Num_Of_Elem(); x++ ) 
        val_2 = val_2 + matrix_for_compare[x];

    global_difference = val_1 - val_2;

    if (global_difference > threshold ) {
    	cout << "TOTAL "<< global_difference << endl;
    }
    else
        global_difference = 0.f;

	return global_difference;
}

//---------------------------------------------------------------------------------


template <class T >
dim3
matrix<T> :: Get_Dimensions() const
{
    return sizes;
}

//---------------------------------------------------------------------------------

template <class T>
size_t
matrix<T> :: Get_Num_Of_Elem () const
{
    return sizes.width * sizes.height * sizes.depth;
}

//---------------------------------------------------------------------------------

template <class T>
size_t
matrix<T> :: Get_Width() const
{
    return sizes.width;
}

//---------------------------------------------------------------------------------

template <class T>
size_t
matrix<T> :: Get_Height () const
{
    return sizes.height;
}

//---------------------------------------------------------------------------------

template <class T>
size_t
matrix<T> :: Get_Depth () const
{
    return sizes.depth;
}

//---------------------------------------------------------------------------------

template <class T>
bool
matrix<T> :: Is_Empty () const 
{
    return ( Get_Num_Of_Elem() == 0 );
}

//---------------------------------------------------------------------------------

template <class T >
const size_t *
matrix<T> :: Get_Size_out( gpu_data_type data_representation)
{
    switch ( data_representation ) {
        case BUFFER_1D:  
            {
                size_for_gpu[0] = Get_Num_Of_Elem();
                size_for_gpu[1] = 1;
                size_for_gpu[2] = 1;
            
            }
                break;
        case IMAGE_2D:
            {
                size_for_gpu[0] = Get_Width();
                size_for_gpu[1] = Get_Height() * Get_Depth();
                size_for_gpu[2] = 1;
            }
            break;
        default:
            cerr << "Unsupported data-representaion type"<< endl;
                throw;
    }
    
    return size_for_gpu;
}

//---------------------------------------------------------------------------------
template <class T >
dim3
matrix<T> :: Get_Size( gpu_data_type data_representation)
{
	Get_Size_out (data_representation);
	
	dim3 ret_val ( size_for_gpu [0], size_for_gpu[1], size_for_gpu[2] );

	return ret_val;
}
//---------------------------------------------------------------------------------

