/* 
 * fclib.c
 * ----------------------------------------------
 * frictional contact library implementation ? :)
 */

#include <string.h>
#include <stdlib.h>
#include <hdf5.h>
#include <hdf5_hl.h>
#include "fclib.h"

#define ASSERT(Test, ...)\
  do {\
  if (! (Test)) { fprintf (stderr, "%s: %d => ", __FILE__, __LINE__);\
    fprintf (stderr, __VA_ARGS__);\
    fprintf (stderr, "\n"); exit (1); } } while (0)

struct fclib_root
{
  struct fclib_global *global_problem;
  struct fclib_local  *local_problem;
  struct fclib_solution  *solution;
  int number_of_guesses;
  struct fclib_solution  *guess; /* pointer to the first guess */
};

/* write matrix */
static void write_matrix (hid_t id, struct fclib_matrix *mat)
{
  int info = mat->info ? 1 : 0;
  hsize_t dim = 1;

  H5LTmake_dataset_int (id, "nzmax", 1, &dim, &mat->nzmax);
  H5LTmake_dataset_int (id, "m", 1, &dim, &mat->m);
  H5LTmake_dataset_int (id, "n", 1, &dim, &mat->n);
  H5LTmake_dataset_int (id, "nz", 1, &dim, &mat->nz);

  if (mat->nz >= 0) /* triplet */
  {
    dim = mat->nz;
    H5LTmake_dataset_int (id, "p", 1, &dim, mat->p);
    H5LTmake_dataset_int (id, "i", 1, &dim, mat->i);
  }
  else if (mat->nz == -1) /* csc */
  {
    dim = mat->n+1;
    H5LTmake_dataset_int (id, "p", 1, &dim, mat->p);
    dim = mat->nzmax;
    H5LTmake_dataset_int (id, "i", 1, &dim, mat->i);
  }
  else if (mat->nz == -2) /* csr */
  {
    dim = mat->m+1;
    H5LTmake_dataset_int (id, "p", 1, &dim, mat->p);
    dim = mat->nzmax;
    H5LTmake_dataset_int (id, "i", 1, &dim, mat->i);
  }
  else ASSERT (0, "ERROR: unkown sparse matrix type => mat->nz = %d\n", mat->nz);

  dim = mat->nzmax;
  H5LTmake_dataset_double (id, "x", 1, &dim, mat->x);

  dim = 1;
  H5LTmake_dataset_int (id, "info", 1, &dim, &info);

  if (mat->info)
  {
    ASSERT (mat->info->comment, "ERROR: matrix info must contain a comment");
    int length = strlen (mat->info->comment);
    H5LTmake_dataset_int (id, "comment_length", 1, &dim, &length);
    H5LTmake_dataset_string (id, "comment", mat->info->comment); /* XXX: does it work with NULL-termined strings? */
    H5LTmake_dataset_double (id, "conditioning", 1, &dim, &mat->info->conditioning);
    H5LTmake_dataset_double (id, "sparsity", 1, &dim, &mat->info->sparsity);
    H5LTmake_dataset_double (id, "determinant", 1, &dim, &mat->info->determinant);
    H5LTmake_dataset_int (id, "rank", 1, &dim, &mat->info->rank);
  }
}

/* read matrix */
struct fclib_matrix* read_matrix (hid_t id)
{
  struct fclib_matrix *mat;
  int info;

  ASSERT (mat = malloc (sizeof (struct fclib_matrix)), "ERROR: out of memory");
 
  H5LTread_dataset_int (id, "nzmax", &mat->nzmax);
  H5LTread_dataset_int (id, "m", &mat->m);
  H5LTread_dataset_int (id, "n", &mat->n);
  H5LTread_dataset_int (id, "nz", &mat->nz);

  if (mat->nz >= 0) /* triplet */
  {
    ASSERT (mat->p = malloc (mat->nz * sizeof (int)), "ERROR: out of memory");
    ASSERT (mat->i = malloc (mat->nz * sizeof (int)), "ERROR: out of memory");
    H5LTread_dataset_int (id, "p", mat->p);
    H5LTread_dataset_int (id, "i", mat->i);
  }
  else if (mat->nz == -1) /* csc */
  {
    ASSERT (mat->p = malloc ((mat->n+1) * sizeof (int)), "ERROR: out of memory");
    ASSERT (mat->i = malloc (mat->nzmax * sizeof (int)), "ERROR: out of memory");
    H5LTread_dataset_int (id, "p", mat->p);
    H5LTread_dataset_int (id, "i", mat->i);
  }
  else if (mat->nz == -2) /* csr */
  {
    ASSERT (mat->p = malloc ((mat->m+1) * sizeof (int)), "ERROR: out of memory");
    ASSERT (mat->i = malloc (mat->nzmax * sizeof (int)), "ERROR: out of memory");
    H5LTread_dataset_int (id, "p", mat->p);
    H5LTread_dataset_int (id, "i", mat->i);
  }
  else ASSERT (0, "ERROR: unkown sparse matrix type => mat->nz = %d\n", mat->nz);

  ASSERT (mat->x = malloc (mat->nzmax * sizeof (double)), "ERROR: out of memory");
  H5LTread_dataset_double (id, "x", mat->x);

  H5LTread_dataset_int (id, "info", &info);

  if (info)
  {
    int length;
    ASSERT (mat->info = malloc (sizeof (struct fclib_matrix_info)), "ERROR: out of memory");
    H5LTread_dataset_int (id, "comment_length", &length);
    ASSERT (mat->info->comment = malloc (sizeof (char [length+1])), "ERROR: out of memory");
    H5LTread_dataset_string (id, "comment", mat->info->comment); /* XXX: there must be a better way */
    mat->info->comment [length] = '\0'; /* XXX: just in case => test without this */
    H5LTread_dataset_double (id, "conditioning", &mat->info->conditioning);
    H5LTread_dataset_double (id, "sparsity", &mat->info->sparsity);
    H5LTread_dataset_double (id, "determinant", &mat->info->determinant);
    H5LTread_dataset_int (id, "rank", &mat->info->rank);
  }

  return mat;
}

/* write global vectors */
static void write_global_vectors (hid_t id, struct fclib_global *problem)
{
  hsize_t dim;

  dim = problem->M->m;
  ASSERT (problem->f, "ERROR: f must be given");
  H5LTmake_dataset_double (id, "f", 1, &dim, problem->f);

  if (problem->G)
  {
    dim = problem->G->n;
    ASSERT (problem->b, "ERROR: b must be given if G is present");
    H5LTmake_dataset_double (id, "b", 1, &dim, problem->b);
  }

  if (problem->H)
  {
    dim = problem->H->n;
    ASSERT (problem->w && problem->mu, "ERROR: w and mu must be given if H is present");
    H5LTmake_dataset_double (id, "w", 1, &dim, problem->w);
    ASSERT (dim % problem->spacedim == 0, "ERROR: number of H columns not divisble by the spatial dimension");
    dim /= problem->spacedim;
    H5LTmake_dataset_double (id, "w", 1, &dim, problem->mu);
  }
}

/* read global vectors */
static void read_global_vectors (hid_t id, struct fclib_global *problem)
{
}

/* write problem info */
static void write_problem_info (hid_t id, struct fclib_info *info)
{
}

/* read problem info */
static void read_problem_info (hid_t id, struct fclib_info *info)
{
}

/* =========================== interface ============================ */

/* write global problem;
 * return 1 on success, 0 on failure */
int fclib_write_global (struct fclib_global *problem, const char *path)
{
  hid_t  file_id, problem_id, id;
  hsize_t dim = 1;
  herr_t  status;

  /* TODO: handle HDF5 errors */

  file_id = H5Fcreate (path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  problem_id = H5Gcreate (file_id, "/fclib_global", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  ASSERT (problem->spacedim == 2 || problem->spacedim == 3, "ERROR: space dimension must be 2 or 3");
  H5LTmake_dataset_int (problem_id, "spacedim", 1, &dim, &problem->spacedim);

  ASSERT (problem->M, "ERROR: M must be given");
  id = H5Gcreate (file_id, "/fclib_global/M", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  write_matrix (id, problem->M);
  status = H5Gclose (id);

  if (problem->H)
  {
    id = H5Gcreate (file_id, "/fclib_global/H", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    write_matrix (id, problem->H);
    status = H5Gclose (id);
  }

  if (problem->G)
  {
    id = H5Gcreate (file_id, "/fclib_global/G", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    write_matrix (id, problem->G);
    status = H5Gclose (id);
  }

  id = H5Gcreate (file_id, "/fclib_global/vectors", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  write_global_vectors (id, problem);
  status = H5Gclose (id);

  if (problem->info)
  {
    id = H5Gcreate (file_id, "/fclib_global/info", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    write_problem_info (id, problem->info);
    status = H5Gclose (id);
  }

  status = H5Gclose (problem_id);
  status = H5Fclose (file_id); 

  return 1; /* TODO: handle HDF5 errors */
}

/* write local problem;
 * return 1 on success, 0 on failure */
int fclib_write_local (struct fclib_local *problem, const char *path)
{
  return 0;
}

/* write solution;
 * return 1 on success, 0 on failure */
int fclib_write_solution (struct fclib_solution *solution, const char *path)
{
  return 0;
}

/* write initial guesses;
 * return 1 on success, 0 on failure */
int fclib_write_guesses (int number_of_guesses,  struct fclib_solution *guesses, const char *path)
{
  return 0;
}

/* read global problem;
 * return problem on success; NULL on failure */
struct fclib_global* fclib_read_global (const char *path)
{
  return NULL;
}

/* read local problem;
 * return problem on success; NULL on failure */
struct fclib_local* fclib_read_local (const char *path)
{
  return NULL;
}

/* read solution;
 * return 1 on success, 0 on failure */
int fclib_read_solution (struct fclib_solution *solution, const char *path)
{
  return 0;
}

/* read initial guesses;
 * return vector of guesses on success; NULL on failure;
 * output numebr of guesses in the variable pointed by 'number_of_guesses' */
struct fclib_solution* fclib_read_guesses (const char *path, int *number_of_guesses)
{
  return NULL;
}

/* calculate merit function for a global problem */
double fclib_merit_global (struct fclib_global *problem, enum fclib_merit merit, struct fclib_solution *solution)
{
  return 0;
}

/* calculate merit function for a local problem */
double fclib_merit_local (struct fclib_local *problem, enum fclib_merit merit, struct fclib_solution *solution)
{
  return 0;
}

/* delete global problem;
 * return 1 on success, 0 on failure */
int fclib_delete_global (struct fclib_global *problem)
{
  return 0;
}

/* delete local problem;
 * return 1 on success, 0 on failure */
int fclib_delete_local (struct fclib_global *problem)
{
  return 0;
}
