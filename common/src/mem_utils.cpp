#include <mem_utils.h>
//#include <stdio.h>
#include <iostream>
using namespace std;

/*
 *      FIXME - it should depend on
 *      OS-bitness
 *      amount of RAM
 *      OS ? Win/Mac/Lin
 *      amount of swap ?
 * */
unsigned int
determine_extract_step ( unsigned int depth ) {
    
    unsigned int step = 0;

    switch ( depth ) {
        
        case 512: step = 256; break;
        case 256: step = 128; break;
        case 128: step = 64; break;
        default:
                  {
                      cout << "determine_extract_step can't gues step size for buffer with depth - " << depth << endl;
                      throw;
                  }                  
                    break;
    }

    if ( depth % step ) {
        cerr    << "determine_extract_step: " 
        << "depth is not a multiplier of step!" 
        <<  " step - " << step
        <<  " width  " << depth
        <<  endl;
                
        throw;
    }
    return step;
}

void
determine_extract_step ( unsigned int depth, unsigned int & step, unsigned int & num_of_steps, int divisor ) {

    if ( divisor == 0 ) {
        cout << "determine_extract_step divisor is equal to zero. What do you mean by that?"<< endl;        
        throw;
    }
    
    step = 0;
    num_of_steps = 0;

    switch ( depth ) {
        
        case 512: step = 256; break;
        case 256: step = 128; break;
        case 128: step = 64; break;
        default:
                  {
                      cout << "determine_extract_step can't gues step size for buffer with width - " << depth << endl;
                      throw;
                  }                  
                    break;
    }

    step /= divisor;

    if ( depth % step ) {
        cerr    << "determine_extract_step: " 
        << "depth is not a multiplier of step!" 
        <<  " step - " << step
        <<  " width  " << depth
        <<  endl;
                
        throw;
    }

    num_of_steps = depth / step;
    if ( num_of_steps == 0 ) {
        num_of_steps = 1;
        step = depth;
    }

}
