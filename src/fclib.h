/* 
 * fclib.h
 * -----------------------------------------
 * frictional contact library interface
 */

#ifndef _fclib_h_
#define _fclib_h_

struct fclib_info /* problem information */
{
  char *title;
  char *description;
  char *math_info;
};

struct fclib_matrix_info  /* matrix information */
{
  char *comment;
  double conditioning;
  double sparsity;
  double determinant;
  int rank;
};

struct fclib_matrix   /* matrix in compressed row/column or triplet form */
{
  int nzmax ;	    /* maximum number of entries */
  int m ;	    /* number of rows */
  int n ;	    /* number of columns */
  int *p ;	    /* compressed: row (size m+1) or column (size n+1) pointers; triplet: row indices (size nz) */
  int *i ;	    /* compressed: column or row indices, size nzmax; triplet: column indices (size nz) */
  double *x ;	    /* numerical values, size nzmax */
  int nz ;	    /* # of entries in triplet matrix;
		       -1 for compressed columns;
		       -2 for compressed rows */
  struct fclib_matrix_info *info;
};

struct fclib_global /* global problem */
{
  struct fclib_matrix *M;
  struct fclib_matrix *H;
  struct fclib_matrix *G;
  double *mu;
  double *f;
  double *b;
  double *w;
  int spacedim;             /* 2 or 3 */
  struct fclib_info *info;
};

struct fclib_local /* local problem */
{
  struct fclib_matrix *W;
  struct fclib_matrix *V;
  struct fclib_matrix *R;
  double *mu;
  double *q;
  double *s; 
  int spacedim;             /* 2 or 3 */
  struct fclib_info *info;
};

struct fclib_solution /* solution data */
{
  double *v;   
  double *u;
  double *r;
  double *l;
};

enum fclib_merit {MERIT_1, MERIT_2} ; /* merit functions */

/* write global problem;
 * return 1 on success, 0 on failure */
int fclib_write_global (struct fclib_global *problem, const char *path);

/* write local problem;
 * return 1 on success, 0 on failure */
int fclib_write_local (struct fclib_local *problem, const char *path);

/* write solution;
 * return 1 on success, 0 on failure */
int fclib_write_solution (struct fclib_solution *solution, const char *path);

/* write initial guesses;
 * return 1 on success, 0 on failure */
int fclib_write_guesses (int number_of_guesses,  struct fclib_solution *guesses, const char *path);

/* read global problem;
 * return problem on success; NULL on failure */
struct fclib_global* fclib_read_global (const char *path);

/* read local problem;
 * return problem on success; NULL on failure */
struct fclib_local* fclib_read_local (const char *path);

/* read solution;
 * return solution on success; NULL on failure */
struct fclib_solution* fclib_read_solution (const char *path);

/* read initial guesses;
 * return vector of guesses on success; NULL on failure;
 * output numebr of guesses in the variable pointed by 'number_of_guesses' */
struct fclib_solution* fclib_read_guesses (const char *path, int *number_of_guesses);

/* calculate merit function for a global problem */
double fclib_merit_global (struct fclib_global *problem, enum fclib_merit merit, struct fclib_solution *solution);

/* calculate merit function for a local problem */
double fclib_merit_local (struct fclib_local *problem, enum fclib_merit merit, struct fclib_solution *solution);

/* delete global problem */
void fclib_delete_global (struct fclib_global *problem);

/* delete local problem */
void fclib_delete_local (struct fclib_local *problem);

/* delete solutions or guesses */
void fclib_delete_solutions (struct fclib_solution *data, int count);

#endif /* _fclib_h_ */
