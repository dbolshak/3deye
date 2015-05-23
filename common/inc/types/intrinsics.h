#ifndef INTRINSICS
#define INTRINSICS

#ifdef __cplusplus
#include <iostream>
#endif

typedef struct intrinsics
{
	float	fx, 
            fy,
            cx,
            cy;
    int     cols,
            rows;
#ifdef __cplusplus    
    intrinsics operator () (int level_index) const;

    friend std::ostream &
    operator << ( std::ostream & out, const intrinsics & intr );

    friend std::istream &
    operator >> ( std::istream & in, intrinsics & intr );

#endif        
} intrinsics;

#endif  // INTRISICS
