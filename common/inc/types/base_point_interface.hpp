/*
 *                      Constructors
 * */
template <class INTERNAL_TYPE>
BASE_TYPE<INTERNAL_TYPE> ::  BASE_TYPE()
{
    Erase();
}

template <class INTERNAL_TYPE>
BASE_TYPE<INTERNAL_TYPE> :: BASE_TYPE ( INTERNAL_TYPE * init_value )
{
    Set_Value( init_value->Get_value() );
}

/*
 *                      Operators
 * */

template <class INTERNAL_TYPE>
BASE_TYPE <INTERNAL_TYPE> & 
BASE_TYPE <INTERNAL_TYPE> :: operator= ( const INTERNAL_TYPE & new_value )
{
    Set_Value(new_value);

    return *this;
}

template <class INTERNAL_TYPE>
BASE_TYPE <INTERNAL_TYPE> & 
BASE_TYPE <INTERNAL_TYPE> :: operator= ( const BASE_TYPE * new_value )
{
    if ( new_value == this )
        return *this;

    Set_Value(new_value->Get_Value());
    return *this;
}

template <class INTERNAL_TYPE>
BASE_TYPE <INTERNAL_TYPE> :: operator INTERNAL_TYPE ( )
{
    return Get_Value();
}

template <class INTERNAL_TYPE>
istream & 
operator>> ( istream & in, BASE_TYPE <INTERNAL_TYPE> & this_object)
{
    INTERNAL_TYPE new_value;
    in >> new_value;
    this_object.Set_Value(new_value);
    return in;
}

template <class INTERNAL_TYPE>
ostream& 
operator<< (ostream &out, const BASE_TYPE <INTERNAL_TYPE> & this_object)
{
    out << this_object.Get_Value();
    return out;
}

template <class U>
float
operator-(const BASE_TYPE<U> & left_operand, const BASE_TYPE<U> & right_operand )
{
    // FIXME - actually would be better in case every class have some method like evailuate difference, which provide as output float value
    return Diff (left_operand.Get_Value(), right_operand.Get_Value () );
}

/*
 *                  Setters/Getters
 * */

template <class INTERNAL_TYPE>
INTERNAL_TYPE 
BASE_TYPE <INTERNAL_TYPE> :: Get_Value() const
{
    return data;
}

template <class INTERNAL_TYPE>
void
BASE_TYPE <INTERNAL_TYPE> :: Set_Value(const INTERNAL_TYPE & init_value)
{
    data = init_value;
}

/*
 *                  Erase
 * */

template <class INTERNAL_TYPE>
void
BASE_TYPE <INTERNAL_TYPE> :: Erase()
{
    Clear (data);   // call functions outside of classes scope to deal with particular type
}

/*
 *                  Randomizer
 * */
template <class INTERNAL_TYPE>
void
BASE_TYPE <INTERNAL_TYPE> :: Init_Randomizer()
{
    srand( time(NULL) );    // FIXME - i am afraid - it is os specific
}
   
template <class INTERNAL_TYPE>
BASE_TYPE <INTERNAL_TYPE>
BASE_TYPE <INTERNAL_TYPE> :: Get_Random_Value()
{
    INTERNAL_TYPE random_value;
    BASE_TYPE<INTERNAL_TYPE> return_value;

    Generate_Random_Value (random_value);

    return_value.Set_Value(random_value);

    return return_value;
}

