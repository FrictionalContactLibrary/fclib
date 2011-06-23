/* 
 * fclib.h
 * -----------------------------------------
 * frictional contact library interface ? :)
 */

#ifndef _fclib_h_
#define _fclib_h_

struct fclib_matrix   /* matrix in compressed-column or triplet form */
{
  int nzmax ;	    /* maximum number of entries */
  int m ;	    /* number of rows */
  int n ;	    /* number of columns */
  int *p ;	    /* column pointers (size n+1) or col indices (size nzmax) */
  int *i ;	    /* row indices, size nzmax */
  double *x ;	    /* numerical values, size nzmax */
  int nz ;	    /* # of entries in triplet matrix, -1 for compressed-col */
  int csr;
} ;

struct fclib_global
{
  fclib_matrix * M;
  fclib_matrix * H;
  fclib_matrix * G;
  double * mu;
  double * f;
  double * b;
  double * w;
  
} ;

struct fclib_local
{
  fclib_matrix * W;
  fclib_matrix * V;
  fclib_matrix * R;
  double * mu;
  double * q;
  double * s;
} ;

struct fclib_solution
{
  /* */
  double * v;   
  double * u;
  double * r;
  double * l;
} ;

enum fclib_merit {MERIT_1, MERIT_2} ; /* merit functions */

/* open for writingn the problem;
 * returns NULL on failure */
int fclib_write_global (fclib_global * , const char *path);
int fclib_write_local  (fclib_local  * , const char *path);

/* open for writing;
 * returns NULL on failure */
int fclib_write_solution (fclib_solution * , const char *path);

/* open for writing;
 * returns NULL on failure */
int fclib_write_guesses (int number_of_guesses,  fclib_solution * , const char *path);

/* open for reading;
 * returns NULL on failure */
fclib_global * fclib_read_global (const char *path);
fclib_local  * fclib_read_local  (const char *path);

/* open for reading;
 * returns NULL on failure */
int fclib_read_solution (fclib_solution * , const char *path);

/* open for reading;
 * returns NULL on failure */
fclib_solution * fclib_read_guesses (int * number_of_guesses,   const char *path);

/* calculate merit function */
double fclib_merit (fclib_global *fc, fclib_merit  merit, fclib_solution *solution);

int fclib_delete_global(fclib_global *);
int fclib_delete_local(fclib_global *);

#endif // _fclib_h_
