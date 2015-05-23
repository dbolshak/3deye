/*
 *      here we generate actual implementations for generic-like functions which were declared at elementary_point_template.h
 * */

#include <math.h>
#define INCLUDED_IN_IMPLEMENTATION_FILE


#define CAT(X,Y) X##Y
#define CAT_(X,Y) X##_##Y
#define	TYPE_PREFIX(X,Y) CAT(X,Y)
#define PREFIX TYPE_PREFIX(DIMENSIONS,TYPE)
#define TYPE_NAME(X,Y) CAT_(X,Y)

/*
 *                  This is NOT definition,
 *                  BUT implementation of methods for generated types
 *
 *                  therefore you SHOULD synchronise 
 *
 * */
#include <string_utils.h>
#include <sstream>
#include <elementary_points.h>
