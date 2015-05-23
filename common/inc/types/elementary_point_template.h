/* internal header
 * for using in conjuction with base_point_types.h
 * for generating point_* data structures and routine for manipulating them
 * */

#if DIMENSIONS == 4
	#define FIELDS TYPE x; TYPE y; TYPE z; TYPE w;
	#define ARGS	TYPE x, TYPE y, TYPE z, TYPE w
	#define FILL_WITH_ARGS(X) X.x = x; X.y = y; X.z = z; X.w = w;
#elif DIMENSIONS == 3
	#define FIELDS TYPE x; TYPE y; TYPE z;
	#define ARGS	TYPE x, TYPE y, TYPE z
	#define FILL_WITH_ARGS(X) X.x = x; X.y = y; X.z = z;
#elif DIMENSIONS == 2
	#define FIELDS TYPE x; TYPE y;
	#define ARGS	TYPE x, TYPE y
	#define FILL_WITH_ARGS(X) X.x = x; X.y = y;
#elif DIMENSIONS == 1
	#define FIELDS TYPE x;
	#define ARGS	TYPE x
	#define FILL_WITH_ARGS(X) X.x = x;
#else
#error Supported dimensions are 1-4!
#endif


/* * * * * * * * * * generic container definition * * * * * * * * * * */

typedef struct TYPE_NAME(point, PREFIX) {
	union {
		TYPE data[DIMENSIONS];
		struct {
			FIELDS;
		};
	};
} TYPE_NAME(point, PREFIX);

/* list of supported operations  */
#ifndef INCLUDED_IN_IMPLEMENTATION_FILE
/* if this file is included in any header - just redefine interface */
TYPE_NAME(point, PREFIX)
TYPE_NAME(make, TYPE_NAME(point, PREFIX) ) (ARGS);

TYPE_NAME(point, PREFIX)
TYPE_NAME(add, PREFIX) (TYPE_NAME(point, PREFIX) A, TYPE_NAME(point, PREFIX) B);

TYPE_NAME(point, PREFIX)
TYPE_NAME(subtract, PREFIX) (TYPE_NAME(point, PREFIX) A, TYPE_NAME(point, PREFIX) B);

TYPE_NAME(point, PREFIX)
TYPE_NAME(multiply, PREFIX) (TYPE_NAME(point, PREFIX) A, TYPE_NAME(point, PREFIX) B);

TYPE
TYPE_NAME(dot, PREFIX) (TYPE_NAME(point, PREFIX) A, TYPE_NAME(point, PREFIX) B);

TYPE
TYPE_NAME(norm, PREFIX) (TYPE_NAME(point, PREFIX) A);

TYPE_NAME(point, PREFIX)
TYPE_NAME(cross, PREFIX) (TYPE_NAME(point, PREFIX) A, TYPE_NAME(point, PREFIX) B);

TYPE_NAME(point, PREFIX)
TYPE_NAME(normalized, PREFIX) (TYPE_NAME(point, PREFIX) A);


#ifdef __cplusplus 

std::ostream &
operator << ( std::ostream & out, const TYPE_NAME(point, PREFIX) & p );

std::istream &
operator >> ( std::istream & in, TYPE_NAME(point, PREFIX) & p );

/*ANSI C doesn't have reference and at present this fucntion doesn't used at OpenCL kernel */
float
TYPE_NAME(squared_euclidean_distance, PREFIX) ( const TYPE_NAME(point, PREFIX) & p1, const TYPE_NAME(point, PREFIX) & p2 );

float
TYPE_NAME(euclidean_distance, PREFIX) ( const TYPE_NAME(point, PREFIX) & p1, const TYPE_NAME(point, PREFIX) & p2);

#endif

#else
/* it mean that file is included in *.cpp source which should be compile once
 * otherwise we get a lot of multiple definition error during linking, in case we
 * include this file in other separate sources 
 * */
TYPE_NAME(point, PREFIX)
TYPE_NAME(make, TYPE_NAME(point, PREFIX) ) (ARGS)
{
	TYPE_NAME(point, PREFIX) return_value;
	FILL_WITH_ARGS(return_value);

	return return_value;
}

/*	FIXME - I guess we can metaprogramm it even more using some kind of macro for operand 
 *	because body of this functions almost the same
 *	*/
TYPE_NAME(point, PREFIX)
TYPE_NAME(add, PREFIX) (TYPE_NAME(point, PREFIX) A, TYPE_NAME(point, PREFIX) B)
{
	TYPE_NAME(point, PREFIX) return_value;

	int i;
	for (i = 0; i < DIMENSIONS; i++)
		return_value.data[i] = A.data[i]+B.data[i];

	return return_value;
}

TYPE_NAME(point, PREFIX)
TYPE_NAME(subtract, PREFIX) (TYPE_NAME(point, PREFIX) A, TYPE_NAME(point, PREFIX) B)
{
	TYPE_NAME(point, PREFIX) return_value;

	int i;
	for (i = 0; i < DIMENSIONS; i++)
		return_value.data[i] = A.data[i]-B.data[i];
	return return_value;
}

TYPE_NAME(point, PREFIX)
TYPE_NAME(multiply, PREFIX) (TYPE_NAME(point, PREFIX) A, TYPE_NAME(point, PREFIX) B)
{
	TYPE_NAME(point, PREFIX) return_value;

	int i;
	for (i = 0; i < DIMENSIONS; i++)
		return_value.data[i] = A.data[i]*B.data[i];

	return return_value;
}

TYPE
TYPE_NAME(dot, PREFIX) (TYPE_NAME(point, PREFIX) A, TYPE_NAME(point, PREFIX) B)
{
	TYPE return_value = 0;
	int i;
	for (i = 0; i < DIMENSIONS; i++)
		return_value += A.data[i]*B.data[i];
	return return_value;
}

TYPE
TYPE_NAME(norm, PREFIX) (TYPE_NAME(point, PREFIX) A)
{
	return sqrt( ( float ) TYPE_NAME(dot, PREFIX) (A, A) ) ;
}

// source of formula for multi-dimensional cross-product 
// http://people.revoledu.com/kardi/tutorial/LinearAlgebra/VectorCrossProduct.html

TYPE_NAME(point, PREFIX)
TYPE_NAME(cross, PREFIX) (TYPE_NAME(point, PREFIX) A, TYPE_NAME(point, PREFIX) B)
{
	TYPE_NAME(point, PREFIX) return_value;

	int i;
	for (i = 0; i < DIMENSIONS; i++)
		return_value.data[i] = A.data [ ( i + 1 ) % DIMENSIONS  ] * B.data [ ( i + 2 ) % DIMENSIONS ] - A.data [ (i + DIMENSIONS - 1 ) % DIMENSIONS ] * B.data [ ( i + DIMENSIONS - 2 ) % DIMENSIONS ];

	return return_value;
}

TYPE_NAME(point, PREFIX)
TYPE_NAME(normalized, PREFIX) (TYPE_NAME(point, PREFIX) A)
{
	TYPE_NAME(point, PREFIX) return_value = A;
	float rsqrt = 1.f / sqrt ( ( float ) TYPE_NAME(dot, PREFIX) (A, A) );

	int i;
	for (i = 0; i < DIMENSIONS; i++)
		return_value.data[i] *= rsqrt;

	return return_value;
}

#ifdef __cplusplus 

std::ostream &
operator << ( std::ostream & out, const TYPE_NAME(point, PREFIX) & p )
{
    // FIXME can't use it from constants because of rounded dependency
    char    COMMON_DELIMITER[]          =   "x";
    for ( int i = 0; i < DIMENSIONS; i++ ) {
        out << p.data[i];
        if (i != DIMENSIONS-1)
            out << COMMON_DELIMITER;
    }
    out << " ";
    
    return out;
}


std::istream &
operator >> ( std::istream & in, TYPE_NAME(point, PREFIX) & p )
{
    // FIXME can't use it from constants because of rounded dependency
    char    COMMON_DELIMITER[]          =   "x";
    std::string tmp_str = "";
    in >> tmp_str;
    replaceAll ( tmp_str, COMMON_DELIMITER, " " );

    std::istringstream in_tmp ( tmp_str );

    for ( int i = 0 ; i < DIMENSIONS; i++ ) 
        in_tmp >> p.data[i];

    return in;
}

float
TYPE_NAME(squared_euclidean_distance, PREFIX) ( const TYPE_NAME(point, PREFIX) & p1, const TYPE_NAME(point, PREFIX) & p2 )
{
    float result = 0.f;
    for ( int i =0; i < DIMENSIONS; i++ )
        result += (p2.data[i] - p1.data[i]) * (p2.data[i] - p1.data[i]) ;

    return result;
}

float
TYPE_NAME(euclidean_distance, PREFIX) ( const TYPE_NAME(point, PREFIX) & p1, const TYPE_NAME(point, PREFIX) & p2)
{
    return sqrtf ( TYPE_NAME(squared_euclidean_distance,PREFIX) ( p1, p2) );
}

#endif

#endif  // INCLUDED_IN_IMPLEMENTATION_FILE

#undef FIELDS
#undef ARGS
#undef FILL_WITH_ARGS
