#ifndef MATRIX
#define MATRIX

#include <constants.h>
#include <dim3.h>


/** \brief class for represent all matrix'like data
 *
 *  NOTE 1 - a lot of repetitive calls to depth() * width() * height()
 *
 * */
template <typename T>
class matrix
{
    dim3 sizes;

	size_t  size_for_gpu[3]; // re-calculated on every call to Get_Size ( GPU-data representation )
	void allocate_buffer();

public:

	/**	\brief simple constructor with allocation
		note - data are not set!
	*/
	matrix(size_t width, size_t height = 1, size_t depth = 1);
	matrix (const size_t * initial_dimensions);
	matrix (const dim3 initial_dimensions);
	matrix();
	~matrix();

	matrix(matrix_creation_type fill_with_random_data, size_t dimension = 0 );

	matrix( const matrix<T> & object_for_copy);

	dim3 Get_Dimensions () const;
    bool    Is_Empty() const;
	size_t Get_Width() const;
	size_t Get_Height() const;
	size_t Get_Depth() const;

	const size_t * Get_Size_out( gpu_data_type data_representation = BUFFER_1D );
	dim3 Get_Size ( gpu_data_type data_representation = BUFFER_1D );
	size_t Get_Num_Of_Elem () const;

	void Set_Dimensions(size_t new_width, size_t new_height = 1, size_t new_depth = 1);
	void Set_Dimensions ( const dim3 & new_dimensions );
    void Fill_With_Data(size_t width, size_t height, size_t depth, T * new_data);
	void Fill_With_Random_Data();

	string Get_Matrix_Type() const;
	bool Is_Valid_Data_Type(const string& matrix_type_name_from_file) const;   

	void Save_To_File(const string & file_name="") const;
	int Fill_From_File(const string & file_name);

	bool operator == (matrix &rhs);
    matrix & operator= (const matrix * rhs);
    T & operator[](const size_t index); 
    T & operator[](const size_t index) const; 
	float Estimate_Difference (matrix<T> &matrix_for_compare, float threshold = 0.000001);
    template <class U>
	float Estimate_Absolute_Difference (matrix<U> &matrix_for_compare, float threshold = 0.000001);

	// BTW - I hate template because of this syntax
	template <class U>
	friend istream& operator>> (istream &in, matrix<U>  &this_object);
	template <class U>
	friend ostream& operator<< (ostream &out, const matrix<U> &obj_for_output);
	
	void free_buffer();
    
    void extend_from (const matrix<T> & matrix_with_additional_data );
    void add_from (const matrix<T> & matrix_with_additional_data, size_t shift, size_t amount = 0 );
    void compact ( size_t new_data_size );

	T * data;
    // vector <T> data;
};

#include <matrix.hpp>

#endif	// MATRIX
