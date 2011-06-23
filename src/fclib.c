/* 
 * fclib.c
 * -----------------------------------------
 * frictional contact library interface ? :)
 */

#include "fclib.h"

struct fclib_root
{
  fclib_global * globalproblem;
  fclib_local  * localproblem;
  fclib_solution  * solution;
  int number_of_guesses;
  fclib_solution  * guess; /* pointer to the first guess */
}

int fclib_root_write(fclib_root *, hid_t * file ){
  
}

int fclib_root_read(fclib_write *, hid_t * file ){
  
}

int fclib_matrix_write(fclib_matrix *, hid_t * file ){
}

int fclib_vector_write(double * v,  int *n, hid_t * file ){
}

fclib_matrix * fclib_matrix_read( hid_t * file ){
}

double * fclib_vector_read( int *n, hid_t * file ){
}

int fclib_info_write(fclib_matrix *, hid_t * file ){

}


/*  .... */





int fclib_write_global (fclib_global * problem, const char *path)
{


}

fclib_global * fclib_read_global (const char *path)
{

}

fclib_local * fclib_read_local (const char *path)
{

}
