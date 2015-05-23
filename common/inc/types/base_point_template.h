/*  internal header
 *  for using in conjuction with base_point_types.h
 *  for generating template class for c++-side code and c-types
 *  with aliases template-name_template-data-type
 *
 *  for example:
 *  vector <int > b 
 *  become 
 *  vector_int b;
 *
 *  NOTE: only user-specific types are supported at present
 *
 * */


#ifndef BASE_TYPE
    #error You should define BASE_TYPE before including this header
#endif


#define COMMON_DATA_FIELDS(SUB_TYPE)  SUB_TYPE data;    /* FIXME - add variatins for different data types via do-while? */

#ifdef GENERATE_OPENCL_CODE
    #define DECLARE_TYPE(TYPE, SUB_TYPE) typedef struct TYPE_NAME(TYPE, SUB_TYPE) {
    #define END_TYPE(TYPE, SUB_TYPE)   } TYPE_NAME(TYPE, SUB_TYPE);
    /*  empty interface for OpenCL structures  */
    #define COMMON_INTERFACE(TYPE, SUB_TYPE)
//     #define GENERATE_CPP_ALIAS(TYPE, SUB_TYPE) 
#else
    #define DECLARE_TYPE(TYPE, SUB_TYPE) template <class SUB_TYPE> class TYPE {
    #define END_TYPE(TYPE, SUB_TYPE)    };
    #define COMMON_INTERFACE(TYPE, SUB_TYPE)   \
    public: \
        TYPE  (); \
        TYPE ( SUB_TYPE * init_value ); \
        void    Set_Value ( const SUB_TYPE & init_value ); \
        TYPE & operator=( const TYPE * rhs ); \
        operator SUB_TYPE (); \
        TYPE & operator=( const SUB_TYPE & init_value ); \
        SUB_TYPE Get_Value() const; \
        template <class U> friend istream & operator>>( istream & in, TYPE < U> & this_object ); \
        template <class U> friend ostream & operator<<( ostream & out, const TYPE <U> & object_to_fill ); \
        void Erase(); \
        static TYPE <SUB_TYPE> Get_Random_Value(); \
        static void Init_Randomizer();
    #define GENERATE_CPP_ALIAS(TYPE, SUB_TYPE) typedef TYPE<SUB_TYPE> TYPE_NAME(TYPE, SUB_TYPE);
#endif

/*      
 *      FIXME - was part of interface
 *      template <class U> U friend  operator-( const U & left_operand, const U & right_operand ); \
        template <class U> float friend operator-(const U & left_operand, const U & right_operand ); \*/
DECLARE_TYPE(BASE_TYPE, INTERNAL_TYPE)
    COMMON_DATA_FIELDS(INTERNAL_TYPE)
    COMMON_INTERFACE(BASE_TYPE, INTERNAL_TYPE)
END_TYPE(BASE_TYPE, INTERNAL_TYPE)

#ifndef GENERATE_OPENCL_CODE

 /*             Common implementation - for all types   
  *             NOTE: we can't use alias here as this is c++ parts
  *             and it include only ones
  *             so it almost typical implementation of templates methods
  *             where BASE_TYPE - is a name of template
  *             and INTERNAL_TYPE - is a template parameter
  * 
  * */

/*  Implementation */
#include <base_point_interface.hpp>

#endif  // not OPENCL
