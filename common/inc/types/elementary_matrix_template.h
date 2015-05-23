typedef struct TYPE_NAME(matrix, PREFIX) {
        TYPE_NAME(point, PREFIX) data[DIMENSIONS];
} TYPE_NAME(matrix, PREFIX);

/* list of supported operations  */
#ifndef INCLUDED_IN_IMPLEMENTATION_FILE

/* if this file is included in any header - just redefine interface */

TYPE_NAME(point, PREFIX)
multiply (  const TYPE_NAME(matrix, PREFIX) * matrix,
            TYPE_NAME(point, PREFIX) * vector );

TYPE_NAME(matrix, PREFIX)
multiply_matrix_3f (   const TYPE_NAME(matrix, PREFIX) * A,
                const TYPE_NAME(matrix, PREFIX) * B);
#ifdef __cplusplus 

std::ostream &
operator << ( std::ostream & out, const TYPE_NAME(matrix, PREFIX) & m );

std::istream &
operator >> ( std::istream & in, TYPE_NAME(matrix, PREFIX) & m );


#endif

#else

TYPE_NAME(point, PREFIX)
multiply (  const TYPE_NAME(matrix, PREFIX) * matrix, 
            TYPE_NAME(point, PREFIX) * vector )
{
    return TYPE_NAME(make, TYPE_NAME(point, PREFIX) ) ( 
        TYPE_NAME(dot, PREFIX) ( matrix->data[0], *vector ),
        TYPE_NAME(dot, PREFIX) ( matrix->data[1], *vector ),
        TYPE_NAME(dot, PREFIX) ( matrix->data[2], *vector ) 
    );
};

TYPE_NAME(matrix, PREFIX)
multiply_matrix_3f (   const TYPE_NAME(matrix, PREFIX) * A,
                const TYPE_NAME(matrix, PREFIX) * B)
{

    TYPE_NAME(matrix, PREFIX) result ={{
                                        {0.f,0.f,0.f},
                                        {0.f,0.f,0.f},
                                        {0.f,0.f,0.f}
                                        }};
    for ( int y = 0; y < DIMENSIONS; y++ )
        for ( int x = 0; x < DIMENSIONS; x++ )
        {
            for ( int k = 0 ; k < DIMENSIONS; k++)
                result.data[y].data[x] += A->data[y].data[k] * B->data[k].data[x]; 
        }
    return result; 
}

#ifdef __cplusplus

std::ostream &
operator << ( std::ostream & out, const TYPE_NAME(matrix, PREFIX) & m )
{
    // FIXME can't use it from constants because of rounded dependency
    //char    MATRIX_DELIMITER[]          =   "m";
    for ( int i = 0; i < DIMENSIONS; i++ ) {
        out << m.data[i];
        if (i != DIMENSIONS-1)
            out << std::endl; //MATRIX_DELIMITER;
    }
    out << " ";
    return out;    
}

std::istream &
operator >> ( std::istream & in, TYPE_NAME(matrix, PREFIX) & m )
{
    std::cerr << "Check elementary_matrix_template!"    << std::endl;
    throw;

    //  FIXME do not work because of whitespace from point_3f fields
    //  so delimiter is inserted after whitespaces
    // FIXME can't use it from constants because of rounded dependency
    char    MATRIX_DELIMITER[]          =   "x";
    std::string tmp_str = "";
    in >> tmp_str;
    replaceAll ( tmp_str, MATRIX_DELIMITER, " " );

    std::istringstream in_tmp ( tmp_str );

    for ( int i = 0 ; i < DIMENSIONS; i++ ) 
        in_tmp >> m.data[i];

    return in;
}

#endif

/*TYPE_NAME(matrix, PREFIX)
inverse ( OPENCL_READ_ONLY_MEM_PTR TYPE_NAME(matrix, PREFIX) * matrix )
{
    TYPE_NAME(matrix, PREFIX) result;
FIXME
    return result;
}
*/
/*
TYPE_NAME(matrix, PREFIX)
TYPE_NAME(add, TYPE_NAME(matrix, PREFIX)) ( TYPE_NAME(matrix, PREFIX) * matrix_A, TYPE_NAME(matrix, PREFIX) * matrix_B )
{
    TYPE_NAME(matrix, PREFIX) result;
   
    for (int i = 0; i < DIMENSIONS; i++)
        result.data[i] = TYPE_NAME(add,PREFIX) ( matrix_A->data[i], matrix_B->data[i] );
    
    return result;
}
*/
#endif  // INCLUDED_IN_IMPLEMENTATION_FILE
