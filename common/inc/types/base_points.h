#ifndef BASE_POINTS
#define BASE_POINTS

#ifdef __cplusplus
    #include <iostream>
    using namespace std;

    //namespace fusion {
    /*  TODO:
     *  1. make common interface for all structs
     *  2. using macross - define it in one line,
     *   replacing type name to appropriate one
     *
     * */
#endif

typedef struct Depth {
    unsigned short value;
#ifdef __cplusplus
    Depth ( );
    Depth ( unsigned short init_value );
    operator unsigned short();
    Depth & operator=(unsigned short & init_value);
    Depth & operator=(const Depth * obj);
    friend istream & operator>> ( istream & in, Depth & object_instance );
    friend ostream & operator<< ( ostream & out, const Depth & object_instance );
    friend void Clear ( Depth & object_instance );
    friend void Generate_Random_Value( Depth & storage_for_random_value );
    friend float Diff (const Depth & v1, const Depth & v2);
#endif
} Depth;

typedef struct Vertex {
    float value;
#ifdef  __cplusplus
    Vertex();
    Vertex( float init_value);
    operator float();
    friend istream & operator>> ( istream & in, Vertex & object_instance );
    friend ostream & operator<< ( ostream & out, const Vertex & object_instance );
    friend void Clear ( Vertex & object_instance );
    friend void Generate_Random_Value( Vertex & storage_for_random_value );
    friend float Diff (const Vertex & v1, const Vertex & v2);
#endif
} Vertex;

typedef struct Normal {
    float value;
#ifdef  __cplusplus
    Normal();
    Normal ( float init_value );
    operator float();
    friend istream & operator>> ( istream & in, Normal & object_instance );
    friend ostream & operator<< ( ostream & out, const Normal & object_instance );
    friend void Clear ( Normal & object_instance );
    friend void Generate_Random_Value( Normal & storage_for_random_value );
    friend float Diff (const Normal & v1, const Normal & v2);
#endif
} Normal;

typedef struct Intensity {
    unsigned char value;
#ifdef  __cplusplus
    Intensity();
    Intensity( unsigned char init_value );
    operator unsigned char ();
    friend istream & operator>> ( istream & in, Intensity & object_instance );
    friend ostream & operator<< ( ostream & out, const Intensity & object_instance );
    friend void Clear ( Intensity & object_instance );
    friend void Generate_Random_Value( Intensity & storage_for_random_value );
    friend float Diff (const Intensity & v1, const Intensity & v2);
#endif
} Intensity;

typedef struct Tsdf {
    short value;
    short weight;
#ifdef __cplusplus 
    friend istream & operator>> ( istream & in, Tsdf & object_instance );
    friend ostream & operator<< ( ostream & out, const Tsdf & object_instance );
    friend void Clear ( Tsdf & object_instance );
    friend void Generate_Random_Value( Tsdf & storage_for_random_value );
    friend float Diff (const Tsdf & v1, const Tsdf & v2);
#endif
} Tsdf;

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char w;
#ifdef  __cplusplus
    friend istream & operator>> ( istream & in, Color & object_instance );
    friend ostream & operator<< ( ostream & out, const Color & object_instance );
    friend void Clear ( Color & object_instance );
    friend void Generate_Random_Value( Color & storage_for_random_value );
    friend float Diff (const Color & v1, const Color & v2);
#endif
} Color;

typedef struct Tsdf_Color {
    Tsdf tsdf;
    Color color;
#ifdef  __cplusplus
    friend istream & operator>> ( istream & in, Tsdf_Color & object_instance );
    friend ostream & operator<< ( ostream & out, const Tsdf_Color & object_instance );
    friend void Clear ( Tsdf_Color & object_instance );
    friend void Generate_Random_Value( Tsdf_Color & storage_for_random_value );
    friend float Diff (const Tsdf_Color & v1, const Tsdf_Color & v2 );
#endif
} Tsdf_Color;



/*
 *      quick and dirty data-type for operate on voxels in TSFD volume
 * */
typedef struct qXYZI {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    float w;
#ifdef  __cplusplus
    friend istream & operator>> ( istream & in, qXYZI & object_instance );
    friend ostream & operator<< ( ostream & out, const qXYZI & object_instance );
    friend float operator+ (float init_val, qXYZI object_instance );
    friend void Clear ( qXYZI & object_instance );
    friend void Generate_Random_Value( qXYZI & storage_for_random_value );
    friend float Diff (const qXYZI & v1, const qXYZI & v2 );
    friend bool operator==( const qXYZI & v1, const qXYZI & v2 );
#endif
} qXYZI;

#ifdef __cplusplus
typedef struct XYZI {
    union {
		float value[4];
		struct {
            float x;
            float y;
            float z;
            float w;
        };
    };
/*    XYZI();
    XYZI(float new_x, float new_y, float new_z, float new_w );*/
    friend float operator+ (float init_val, XYZI object_instance );
    friend istream & operator>> ( istream & in, XYZI & object_instance );
    friend ostream & operator<< ( ostream & out, const XYZI & object_instance );
    friend void Clear ( XYZI & object_instance );
    friend void Generate_Random_Value( XYZI & storage_for_random_value );
    friend float Diff (const XYZI & v1, const XYZI & v2 );
    friend bool operator==( const XYZI & v1, const XYZI & v2 );
} XYZI;
#else
typedef float4 XYZI;
#endif
typedef XYZI NXYZI;

typedef struct XYZI_Color {
    XYZI vertex;
    Color color;
#ifdef  __cplusplus
    friend istream & operator>> ( istream & in, XYZI_Color & object_instance );
    friend ostream & operator<< ( ostream & out, const XYZI_Color & object_instance );
    friend void Clear ( XYZI_Color & object_instance );
    friend void Generate_Random_Value( XYZI_Color & storage_for_random_value );
    friend float Diff (const XYZI_Color & v1, const XYZI_Color & v2 );
#endif
} XYZI_Color;

typedef struct XYZ {
    float x;
    float y;
    float z;
#ifdef  __cplusplus
    friend istream & operator>> ( istream & in, XYZ & object_instance );
    friend ostream & operator<< ( ostream & out, const XYZ & object_instance );
    friend float operator+ (float init_val, XYZ object_instance );
    friend void Clear ( XYZ & object_instance );
    friend void Generate_Random_Value( XYZ & storage_for_random_value );
    friend float Diff (const XYZ & v1, const XYZ & v2 );
    friend bool operator==( const XYZ & v1, const XYZ & v2 );
#endif
} XYZ;
typedef XYZ NXYZ;

typedef struct Vertex_Normal {
    union {
		XYZI    vertex;
        struct {
            float x;
            float y;
            float z;
            float w;
        };
    };
    union {
		NXYZI    normal;
        struct {
            float nx;
            float ny;
            float nz;
            float nw;
        };
    };
#ifdef  __cplusplus
    friend istream & operator>> ( istream & in, Vertex_Normal & object_instance );
    friend ostream & operator<< ( ostream & out, const Vertex_Normal & object_instance );
    friend void Clear ( Vertex_Normal & object_instance );
    friend void Generate_Random_Value( Vertex_Normal & storage_for_random_value );
    friend float Diff (const Vertex_Normal & v1, const Vertex_Normal & v2 );
#endif
} Vertex_Normal;


#ifdef __cplusplus
//    };   // end namespace fusion
#endif

#endif // BASE_POINTS
