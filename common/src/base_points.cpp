#include <base_points.h>
#include <constants.h>
#include <string_utils.h>

#include <stdlib.h>
#include <sstream>

#include <math.h>

/*
 *              Depth Interface Implementation
 * */
Depth :: Depth() {
    value = 0 ;
}

Depth :: Depth ( unsigned short new_value ) {
    value = new_value;
}

Depth :: operator unsigned short ( ) {
    return value;
}

Depth &
Depth :: operator= ( unsigned short & new_value ) {
    value = new_value;
    return *this;
}

Depth & 
Depth :: operator= ( const Depth * obj ) {
    if (obj == this)
        return *this;
    value = obj->value;
          
    return * this;
}

istream & 
operator>> ( istream & in, Depth & this_object ) {
    in >> this_object.value;
    return in;
}

ostream &
operator<< ( ostream & out, const Depth & this_object) {
    out << this_object.value;
    return out;
}

void
Clear ( Depth & object_instance ) {
    object_instance.value = 0;
}

void
Generate_Random_Value( Depth & storage_for_random_value ) {
    storage_for_random_value.value = abs ( rand() % MAX_DEPTH_VALUE ) + 1;
}

float
Diff (const Depth & v1, const Depth & v2) {
    return v1.value - v2.value;
}

/*
 *                  specialization for Normal
 * */
Normal :: Normal () {
    value = 0.f;
}
 
Normal :: Normal ( float init_value ) {
    value = init_value;
} 

Normal::operator float() {
    return value;
}

istream & 
operator>> ( istream & in, Normal & this_object ) {
    in >> this_object.value;
    return in;
}

ostream &
operator<< ( ostream & out, const Normal & this_object) {
    out << this_object.value;
    return out;
}

void
Clear ( Normal & object_instance ) {
    object_instance.value = 0.f;
}

void
Generate_Random_Value( Normal & storage_for_random_value ) {
    storage_for_random_value.value = abs ( rand() / MAX_NORMAL_VALUE ) + abs ( rand() % MAX_NORMAL_VALUE + 1 ) ;
}

float
Diff (const Normal &v1, const Normal &v2)
{
    return v1.value - v2.value;
}

/*
 *                  specialization for Vertex 
 * */

Vertex :: Vertex () {
    Clear(*this);
}

Vertex :: Vertex ( float init_val ) {
    value = init_val;
}

Vertex :: operator float () {
    return value;
}

istream & 
operator>> ( istream & in, Vertex & this_object ) {
    in >> this_object.value;
    return in;
}

ostream &
operator<< ( ostream & out, const Vertex & this_object) {
    out << this_object.value;
    return out;
}

void
Clear ( Vertex & object_instance ) {
    object_instance.value = 0.f;
}

void
Generate_Random_Value( Vertex & storage_for_random_value ) {
    storage_for_random_value.value = abs ( rand() % MAX_VERTEX_VALUE ) + 0.001 * abs ( rand() % 1000 );
}

float
Diff (const Vertex &v1, const Vertex &v2) {
    return v1.value - v2.value;
}

/*
 *                  specialization for Intensity 
 * */
Intensity :: Intensity () {
    value = ( unsigned char ) 0;
}
 
Intensity :: Intensity ( unsigned char init_value ) {
    value = init_value;
} 

Intensity ::operator unsigned char () {
    return value;
}

istream & 
operator>> ( istream & in, Intensity & this_object ) {
    // as far as I remember there were some problems
    // related to stream in c++
    // that's why I have to use it in a such way here
    unsigned short tmp;
    in >> tmp;
    this_object.value = ( unsigned char ) tmp;
    return in;
}

ostream &
operator<< ( ostream & out, const Intensity & this_object) {
    out << ( unsigned ) this_object.value;
    return out;
}

void
Clear ( Intensity & object_instance ) {
    object_instance.value = 0;
}

void
Generate_Random_Value( Intensity & storage_for_random_value ) {
    storage_for_random_value.value = abs ( rand() / MAX_RGB_COLOR ) + abs ( rand() % MAX_RGB_COLOR + 1 ) ;
}

float
Diff (const Intensity &v1, const Intensity &v2) {
    return v1.value - v2.value;
}


/*
 *                  Specialization for Tsdf struct 
 *
 * */
istream & 
operator>> ( istream & in, Tsdf & this_object) {
    string tmp_str = "";
	in >> tmp_str;
	replaceAll(tmp_str, COMMON_DELIMITER, " ");
    
    /*  
     *  string format should be
     *  Tsdf_Value-Tsdf_Weight
     *  where 
     *  Tsdf_Value in range ()
     *  Tsdf_Weight in range ()
     *  and '-' - is TSDF_DELIMITER
    */

	istringstream in_tmp(tmp_str);
    in_tmp >> this_object.value >> this_object.weight;
    return in;
}

ostream& 
operator<< (ostream &out, const Tsdf & this_object) {
    out << this_object.value << COMMON_DELIMITER << this_object.weight;
    return out;
}   

void
Clear ( Tsdf & object_instance ) {
    object_instance.value = 0.f;
    object_instance.weight  = 0.f;
}

void
Generate_Random_Value( Tsdf & storage_for_random_value ) {
 	storage_for_random_value.value = ( rand() % MAX_DEPTH_VALUE + 1 );
	storage_for_random_value.weight = abs ( rand() ) % MAX_TSDF_WEIGHT;
}

float
Diff (const Tsdf &v1, const Tsdf &v2) {
    return (v1.value - v2.value) + (v1.weight - v2.weight);
}
/*
 *                  Specialization for Color struct 
 *
 * */
istream & 
operator>> ( istream & in, Color & this_object) {
    string tmp_str = "";
	in >> tmp_str;
	replaceAll(tmp_str, COLOR_DELIMITER, " ");

	/*  
     *  string format should be
     *  R.G.B.W
     *  where R, G, B ~ [0,255]
     *  W - weight
     *  and '.' - is COLOR_DELIMITER
    */

    // I Love Cpp!
	istringstream in_tmp(tmp_str);
    unsigned short r , g , b, w;
    in_tmp >> r >> g >> b >> w;
    this_object.r = (unsigned char) r;
    this_object.g = (unsigned char) g;
    this_object.b = (unsigned char) b;
    this_object.w = (unsigned char) w;
 	in_tmp >> this_object.r >> this_object.g >> this_object.b >> this_object.w;
    
    return in;
}

ostream& 
operator<< (ostream &out, const Color & this_object) {
	out << (unsigned) this_object.r << COLOR_DELIMITER << (unsigned) this_object.g << COLOR_DELIMITER  << (unsigned) this_object.b << COLOR_DELIMITER << (unsigned) this_object.w << " ";
    return out;
}

void
Clear ( Color & object_instance ) {
    object_instance.r = 0;
    object_instance.g = 0;
    object_instance.b = 0;
    object_instance.w = 0;
}

void
Generate_Random_Value( Color & storage_for_random_value ) {
	storage_for_random_value.r = abs ( rand() ) % MAX_RGB_COLOR + 1;
	storage_for_random_value.g = abs ( rand() ) % MAX_RGB_COLOR + 1;
	storage_for_random_value.b = abs ( rand() ) % MAX_RGB_COLOR + 1;
	storage_for_random_value.w = abs ( rand() ) % MAX_RGB_COLOR + 1;
}

float
Diff (const Color &v1, const Color &v2) {
    return (v1.r - v2.r) + (v1.g - v2.g) + (v1.b - v2.b) + (v1.w - v2.w);
}

/*
 *                  Specialization for Tsdf_Color struct 
 *
 * */
istream & 
operator>> ( istream & in, Tsdf_Color & this_object) {
    in >> this_object.tsdf;
    in >> this_object.color;
    return in;
}

ostream& 
operator<< (ostream &out, const Tsdf_Color & this_object) {
    out << this_object.tsdf << " " << this_object.color;
    return out;
}

void
Clear ( Tsdf_Color & object_instance ) {
    Clear(object_instance.color);
    Clear(object_instance.tsdf);
}
void
Generate_Random_Value( Tsdf_Color & storage_for_random_value ) {
	Generate_Random_Value ( storage_for_random_value.tsdf );
	Generate_Random_Value ( storage_for_random_value.color );
}

float
Diff (const Tsdf_Color &v1, const Tsdf_Color &v2) {
    return Diff (v1.tsdf, v2.tsdf) + Diff (v1.color, v2.color);
}

/*
 *                  Specialization for XYZI_Color struct 
 *
 * */
istream & 
operator>> ( istream & in, XYZI_Color & this_object) {
    in  >> this_object.vertex >> this_object.color;
    return in;
}

ostream& 
operator<< (ostream &out, const XYZI_Color & this_object) {
    out << this_object.vertex << this_object.color;
    return out;
}

void
Clear ( XYZI_Color & object_instance ) {
    Clear(object_instance.color);
    Clear(object_instance.vertex);
}
void
Generate_Random_Value( XYZI_Color & storage_for_random_value ) {
	Generate_Random_Value ( storage_for_random_value.vertex );
	Generate_Random_Value ( storage_for_random_value.color );
}

float
Diff (const XYZI_Color &v1, const XYZI_Color &v2) {
    return Diff ( v1.vertex, v2.vertex ) + Diff ( v1.color, v2.color );
}

/*
 *
 *                  Specializatioin for qXYZI struct
 *
 * */

istream &
operator>> (istream & in, qXYZI & this_object) {
    string tmp_str = "";
	in >> tmp_str;
	replaceAll(tmp_str, COMMON_DELIMITER, " ");
    
    /*  
     *  string format should be
     *  X.Y.Z.I
     *  where 
     *  X, Y, Z in range (unsigned short)
     *  I in range (0 - 1)
     *  and '.' - is COMMON_DELIMITER
    */

	istringstream in_tmp(tmp_str);
    in_tmp >> this_object.x >> this_object.y >> this_object.z >> this_object.w;

    return in;
}

ostream& 
operator<< (ostream &out, const qXYZI & this_object) {
	out << this_object.x << COMMON_DELIMITER << this_object.y << COMMON_DELIMITER  << this_object.z << COMMON_DELIMITER << this_object.w << " ";
    return out;
}

void
Clear ( qXYZI & object_instance ) {
    object_instance.x = 0;
    object_instance.y = 0; 
    object_instance.z = 0; 
    object_instance.w = 0.f; 
}

void
Generate_Random_Value( qXYZI & storage_for_random_value ) {
    storage_for_random_value.x = abs ( rand() ) % DEFAULT_VOLUME_VOXELS_WIDTH;
    storage_for_random_value.y = abs ( rand() ) % DEFAULT_VOLUME_VOXELS_HEIGHT; 
    storage_for_random_value.z = abs ( rand() ) % DEFAULT_VOLUME_VOXELS_DEPTH; 
    storage_for_random_value.w = ( rand() + 1 ) % MAX_INTENSITY_DIVISOR; // should lay in ( -1, 0.98), not 0.0
}

float
Diff (const qXYZI &v1, const qXYZI &v2) {
    return  ( v1.x - v2.x ) +
            ( v1.y - v2.y ) +
            ( v1.z - v2.z ) +
            ( v1.w - v2.w );
}

float operator+ (float init_val,  qXYZI obj ) {
    return init_val + obj.x + obj.y + obj.z + obj.w;
}

bool
operator== ( const qXYZI &v1, const qXYZI &v2 ) {

    return  ( v1.x == v2.x ) &
            ( v1.y == v2.y ) &
            ( v1.z == v2.z ) &
            ( v1.w == v2.w );
}

/*
 *
 *                  Specializatioin for XYZ struct
 *
 * */

istream &
operator>> (istream & in, XYZ & this_object) {
    string tmp_str = "";
	in >> tmp_str;
	replaceAll(tmp_str, COMMON_DELIMITER, " ");
    
    /*  
     *  string format should be
     *  X.Y.Z
     *  where 
     *  X, Y, Z in range (float)
     *  and '.' - is COMMON_DELIMITER
    */

    std::size_t found = tmp_str.find("nan");
    if ( found == std::string::npos) {
	    istringstream in_tmp(tmp_str);
        in_tmp >> this_object.x >> this_object.y >> this_object.z;
    }
    else {
        /*
         *      FIXME
         *      Well, I have only one excuse for this - I was in WILD hurry
         * */

	    istringstream in_tmp(tmp_str);
        string field_1, field_2, field_3;
        in_tmp >> field_1 >> field_2 >> field_3;

        found = field_1.find("nan");
        if ( found != std::string::npos)
            this_object.x = NAN;
        else {            
	        istringstream in_tmp_1(field_1);
            in_tmp_1 >> this_object.x;
        }

        found = field_2.find("nan");
        if ( found != std::string::npos)
            this_object.y = NAN;
        else {            
	        istringstream in_tmp_1(field_2);
            in_tmp_1 >> this_object.y;
        }

        found = field_3.find("nan");
        if ( found != std::string::npos)
            this_object.z = NAN;
        else {            
	        istringstream in_tmp_1(field_3);
            in_tmp_1 >> this_object.z;
        }
    }

    return in;
}

ostream& 
operator<< (ostream &out, const XYZ & this_object) {
	out << this_object.x << COMMON_DELIMITER << this_object.y << COMMON_DELIMITER  << this_object.z;// << " ";
    return out;
}

void
Clear ( XYZ & object_instance ) {
    object_instance.x = 0.f;
    object_instance.y = 0.f; 
    object_instance.z = 0.f; 
}

void
Generate_Random_Value( XYZ & storage_for_random_value ) {
    // FIXME
     storage_for_random_value.x = abs ( rand() / MAX_NORMAL_VALUE ) + abs ( rand() % MAX_NORMAL_VALUE + 1 ) ;
     storage_for_random_value.y = abs ( rand() / MAX_NORMAL_VALUE ) + abs ( rand() % MAX_NORMAL_VALUE + 1 ) ; 
     storage_for_random_value.z = abs ( rand() / MAX_NORMAL_VALUE ) + abs ( rand() % MAX_NORMAL_VALUE + 1 ) ; 
}

float
Diff (const XYZ &v1, const XYZ &v2) {
    return  ( v1.x - v2.x ) +
            ( v1.y - v2.y ) +
            ( v1.z - v2.z ); 
}

float operator+ (float init_val,  XYZ obj ) {
    return init_val + obj.x + obj.y + obj.z;
}

bool
operator== ( const XYZ &v1, const XYZ &v2 ) {

    return  ( v1.x == v2.x ) &
            ( v1.y == v2.y ) &
            ( v1.z == v2.z );
}
/*
 *                  Specialization for XYZI struct 
 *
 * */
float operator+ (float init_val,  XYZI obj ) {
    return init_val + obj.x + obj.y + obj.z + obj.w;
}

istream & 
operator>> ( istream & in, XYZI & this_object) {
    string tmp_str = "";
	in >> tmp_str;
	replaceAll(tmp_str, COMMON_DELIMITER, " ");
    /*  
     *  string format should be
     *  X.Y.Z.I
     *  where 
     *  X, Y, Z in range ()
     *  I in range ()
     *  and 'x' - is COMMON_DELIMITER
    */

	istringstream in_tmp(tmp_str);
    in_tmp >> this_object.x >> this_object.y >> this_object.z >> this_object.w;

    return in;
}

ostream& 
operator<< (ostream &out, const XYZI & this_object) {
	out << this_object.x << COMMON_DELIMITER << this_object.y << COMMON_DELIMITER  << this_object.z << COMMON_DELIMITER << this_object.w;//FIXME << " ";
    return out;
}

void
Clear ( XYZI & object_instance ) {
    object_instance.x = 0.f;
    object_instance.y = 0.f; 
    object_instance.z = 0.f; 
    object_instance.w = 0.f; 
}
void
Generate_Random_Value( XYZI & storage_for_random_value ) {
    storage_for_random_value.x = abs ( rand() ) % DEFAULT_VOLUME_VOXELS_WIDTH;
    storage_for_random_value.y = abs ( rand() ) % DEFAULT_VOLUME_VOXELS_HEIGHT; 
    storage_for_random_value.z = abs ( rand() ) % DEFAULT_VOLUME_VOXELS_DEPTH; 
    storage_for_random_value.w = ( rand() + 1 ) % MAX_INTENSITY_DIVISOR; // should lay in ( -1, 0.98), not 0.0
}

float
Diff (const XYZI &v1, const XYZI &v2) {
    return  ( v1.x - v2.x ) +
            ( v1.y - v2.y ) +
            ( v1.z - v2.z ) +
            ( v1.w - v2.w );
}

bool
operator== ( const XYZI &v1, const XYZI &v2 ) {

    return  ( v1.x == v2.x ) &
            ( v1.y == v2.y ) &
            ( v1.z == v2.z ) &
            ( v1.w == v2.w );
}

/*
 *                  Specialization for Vertex_Normal struct 
 *
 * */

istream & 
operator>> ( istream & in, Vertex_Normal & this_object) {
    string tmp_str = "";
	in >> tmp_str;
	replaceAll(tmp_str, VERTEX_NORMAL_DELIMITER, " ");
    
    /*  
     *  string format should be
     *  X.Y.Z.IxX.Y.Z.I
     *  where 
     *  X, Y, Z in range ()
     *  I in range ()
     *  1st part - vertex
     *  2nd part - normal
     *  and 'x' - is VERTEX_NONRMAL_DELIMITER
    */

	istringstream in_tmp(tmp_str);
    in_tmp >> this_object.vertex >> this_object.normal;

    return in;
}

ostream& 
operator<< (ostream &out, const Vertex_Normal & this_object) {
	out << this_object.vertex << VERTEX_NORMAL_DELIMITER << this_object.normal << " ";
    return out;
}

void
Clear ( Vertex_Normal & object_instance ) {
    Clear ( object_instance.vertex );
    Clear ( object_instance.normal );
};

void
Generate_Random_Value( Vertex_Normal & storage_for_random_value ) {
    Generate_Random_Value(storage_for_random_value.vertex);
    Generate_Random_Value(storage_for_random_value.normal);
}

float 
Diff (const Vertex_Normal & v1, const Vertex_Normal & v2 ) {
    return Diff (v1.vertex, v2.vertex) + Diff (v1.normal, v2.normal);
}
