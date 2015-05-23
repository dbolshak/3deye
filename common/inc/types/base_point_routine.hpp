/*
 *                  Erase implementation by per class
 * */
template <class U>  /*FIXME  is it one more recursion ? */
void
Erase_By_Type ( U & object_for_erase) {
    
    object_for_erase.value = 0;   // FIXME .value = 0.f;
}

template<>
void
Erase_By_Type (Tsdf & object_for_erase) {
    object_for_erase.value = 0.f;
    object_for_erase.weight = 0.f;
}

template<>
void
Erase_By_Type (Color & object_for_erase) {
    object_for_erase.r = 0;
    object_for_erase.g = 0;
    object_for_erase.b = 0;
}

template<>
void
Erase_By_Type (Tsdf_Color & object_for_erase) {
    Erase_By_Type(object_for_erase.tsdf);
    Erase_By_Type(object_for_erase.color);
}

/*
 *                  Random value generator per class
 * */
template <class U>
void
Generate_Random_Value( U & storage_for_random_value )
{
    storage_for_random_value.value  = abs ( rand() ) ;
}

template <>
void
Generate_Random_Value( Depth & storage_for_random_value )
{
    storage_for_random_value.value = abs ( rand() % MAX_DEPTH_VALUE ) +1 ;
}

template <>
void
Generate_Random_Value( Color & storage_for_random_value )
{
	storage_for_random_value.r = abs ( rand() ) % MAX_RGB_COLOR + 1;
	storage_for_random_value.g = abs ( rand() ) % MAX_RGB_COLOR + 1;
	storage_for_random_value.b = abs ( rand() ) % MAX_RGB_COLOR + 1;
}
template <>
void
Generate_Random_Value( Tsdf & storage_for_random_value )
{
	storage_for_random_value.value = abs ( rand() ) / MAX_TSDF_VALUE;
	storage_for_random_value.weight = abs ( rand() ) / MAX_TSDF_WEIGHT;
}

template <>
void
Generate_Random_Value( Tsdf_Color & storage_for_random_value )
{
	Generate_Random_Value ( storage_for_random_value.tsdf );
	Generate_Random_Value ( storage_for_random_value.color );
}