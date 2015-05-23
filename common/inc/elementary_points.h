#ifndef ELEMENTARY_POINTS
#define ELEMENTARY_POINTS
/*
 *                          This file contains actual list of data structures used in module.
 *                          
 *                          Every entry of include *_template.h generate new data_struct 
 *                          according with provided define values and internal macros.
 *                          Simultaneusly this file is included in elementary_point.cpp
 *                          where using it implementation of appropriate functions is generated.
 *
 *                          NOTE: you can't use this file directly, because it is based on additional
 *                          macros, which included at higher level (FIXME - add file name with macros)
 *
 *                          simple n-dimensions vector-like data typies
 *                          generally data[num_of_dimensions] with alias x,y,z,w
 *                          at present supported not more then 4 dimensions
 *
 * */

#ifdef __cplusplus
#include <stdlib.h> // for size_t declaration
#include <iostream>
#endif

/* FIXME - the same macros is used in base_point_types.h     */
#define CAT(X,Y) X##Y
#define CAT_(X,Y) X##_##Y
#define	TYPE_PREFIX(X,Y) CAT(X,Y)
#define PREFIX TYPE_PREFIX(DIMENSIONS,TYPE)
#define TYPE_NAME(X,Y) CAT_(X,Y)
 
typedef int i;
typedef float f;
typedef unsigned int us;
/* * * * * * * * * * * * * * * * * * * *  point_3f * * * * * * * * * * * * * * * * * * * */
#define TYPE f
#define DIMENSIONS 3
	#include <elementary_point_template.h>
#undef TYPE
#undef DIMENSIONS
typedef point_3f vector_3f;

/* * * * * * * * * * * * * * * * * * * *  point_3i * * * * * * * * * * * * * * * * * * * */
#define TYPE i
#define DIMENSIONS 3
	#include <elementary_point_template.h>
#undef TYPE
#undef DIMENSIONS

/* * * * * * * * * * * * * * * * * * * *  point_3us * * * * * * * * * * * * * * * * * * */
#define TYPE us
#define DIMENSIONS 3
	#include <elementary_point_template.h>
#undef TYPE
#undef DIMENSIONS

/* * * * * * * * * * * * * * * * * * * *  point_2i * * * * * * * * * * * * * * * * * * * */
#define TYPE i
#define DIMENSIONS 2
	#include <elementary_point_template.h>
#undef TYPE
#undef DIMENSIONS

/* * * * * * * * * * * * * * * * * * * *  point_2us * * * * * * * * * * * * * * * * * * * */
#define TYPE us
#define DIMENSIONS 2
	#include <elementary_point_template.h>
#undef TYPE
#undef DIMENSIONS

/* * * * * * * * * * * * * * * * * * * *  short2 * * * * * * * * * * * * * * * * * * * */
#define TYPE short 
#define DIMENSIONS 2
	#include <elementary_point_template.h>
#undef TYPE
#undef DIMENSIONS
#ifndef GENERATE_OPENCL_CODE
typedef point_2short short2;
#endif

/* * * * * * * * * * * * * * * * * * * *  matrix_3f * * * * * * * * * * * * * * * * * * * */
#define TYPE f
#define DIMENSIONS 3
	#include <elementary_matrix_template.h>
#undef TYPE
#undef DIMENSIONS

#endif // ELEMENTARY_POINTS
