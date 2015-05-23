#ifndef MEM_UTILS
#define MEM_UTILS

//#include <stdlib.h>

unsigned int
determine_extract_step ( unsigned int tsdf_buffer_width );

unsigned int
determine_extract_step ( unsigned int width, unsigned int height, unsigned int depth );

void
determine_extract_step ( unsigned int width, unsigned int & step, unsigned int & num_of_steps, int divisor = 1 );

#endif
