/* FCLIB Copyright (C) 2011 FClib project
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: fclib-project@lists.gforge.inria.fr   
 */
/* 
 * fclib.c
 * ----------------------------------------------
 * frictional contact library input/output
 */

/* choose api version */
#define H5Gcreate_vers 2
#define H5Gopen_vers 2

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <hdf5.h>
#include <hdf5_hl.h>
#include "fclib.h"
#include "fcint.h"


/* make grourp */
static hid_t H5Gmake (hid_t loc_id, const char *name)
{
  hid_t id;

  if (H5Lexists (loc_id, name, H5P_DEFAULT))
  {
    id = H5Gopen (loc_id, name, H5P_DEFAULT);
  }
  else return H5Gcreate (loc_id, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  return id;
}

/* write matrix */
static void write_matrix (hid_t id, struct fclib_matrix *mat)
{
  hsize_t dim = 1;

  IO (H5LTmake_dataset_int (id, "nzmax", 1, &dim, &mat->nzmax));
  IO (H5LTmake_dataset_int (id, "m", 1, &dim, &mat->m));
  IO (H5LTmake_dataset_int (id, "n", 1, &dim, &mat->n));
  IO (H5LTmake_dataset_int (id, "nz", 1, &dim, &mat->nz));

  if (mat->nz >= 0) /* triplet */
  {
    dim = mat->nz;
    IO (H5LTmake_dataset_int (id, "p", 1, &dim, mat->p));
    IO (H5LTmake_dataset_int (id, "i", 1, &dim, mat->i));
  }
  else if (mat->nz == -1) /* csc */
  {
    dim = mat->n+1;
    IO (H5LTmake_dataset_int (id, "p", 1, &dim, mat->p));
    dim = mat->nzmax;
    IO (H5LTmake_dataset_int (id, "i", 1, &dim, mat->i));
  }
  else if (mat->nz == -2) /* csr */
  {
    dim = mat->m+1;
    IO (H5LTmake_dataset_int (id, "p", 1, &dim, mat->p));
    dim = mat->nzmax;
    IO (H5LTmake_dataset_int (id, "i", 1, &dim, mat->i));
  }
  else ASSERT (0, "ERROR: unkown sparse matrix type => fclib_matrix->nz = %d\n", mat->nz);

  dim = mat->nzmax;
  IO (H5LTmake_dataset_double (id, "x", 1, &dim, mat->x));

  if (mat->info)
  {
    dim = 1;
    if (mat->info->comment) IO (H5LTmake_dataset_string (id, "comment", mat->info->comment));
    IO (H5LTmake_dataset_double (id, "conditioning", 1, &dim, &mat->info->conditioning));
    IO (H5LTmake_dataset_double (id, "determinant", 1, &dim, &mat->info->determinant));
    IO (H5LTmake_dataset_int (id, "rank", 1, &dim, &mat->info->rank));
  }
}

/* read matrix */
struct fclib_matrix* read_matrix (hid_t id)
{
  struct fclib_matrix *mat;

  MM (mat = malloc (sizeof (struct fclib_matrix)));
 
  IO (H5LTread_dataset_int (id, "nzmax", &mat->nzmax));
  IO (H5LTread_dataset_int (id, "m", &mat->m));
  IO (H5LTread_dataset_int (id, "n", &mat->n));
  IO (H5LTread_dataset_int (id, "nz", &mat->nz));

  if (mat->nz >= 0) /* triplet */
  {
    MM (mat->p = malloc (sizeof (int [mat->nz])));
    MM (mat->i = malloc (sizeof (int [mat->nz])));
    IO (H5LTread_dataset_int (id, "p", mat->p));
    IO (H5LTread_dataset_int (id, "i", mat->i));
  }
  else if (mat->nz == -1) /* csc */
  {
    MM (mat->p = malloc (sizeof (int [mat->n+1])));
    MM (mat->i = malloc (sizeof (int [mat->nzmax])));
    IO (H5LTread_dataset_int (id, "p", mat->p));
    IO (H5LTread_dataset_int (id, "i", mat->i));
  }
  else if (mat->nz == -2) /* csr */
  {
    MM (mat->p = malloc (sizeof (int [mat->m+1])));
    MM (mat->i = malloc (sizeof (int [mat->nzmax])));
    IO (H5LTread_dataset_int (id, "p", mat->p));
    IO (H5LTread_dataset_int (id, "i", mat->i));
  }
  else ASSERT (0, "ERROR: unkown sparse matrix type => fclib_matrix->nz = %d\n", mat->nz);

  MM (mat->x = malloc (sizeof (double [mat->nzmax])));
  IO (H5LTread_dataset_double (id, "x", mat->x));

  if (H5LTfind_dataset (id, "conditioning"))
  {
    H5T_class_t class_id;
    hsize_t dim;
    size_t size;

    MM (mat->info = malloc (sizeof (struct fclib_matrix_info)));
    if (H5LTfind_dataset (id, "comment"))
    {
      IO (H5LTget_dataset_info  (id, "comment", &dim, &class_id, &size));
      MM (mat->info->comment = malloc (sizeof (char [size])));
      IO (H5LTread_dataset_string (id, "comment", mat->info->comment));
    }
    else mat->info->comment = NULL;
    IO (H5LTread_dataset_double (id, "conditioning", &mat->info->conditioning));
    IO (H5LTread_dataset_double (id, "determinant", &mat->info->determinant));
    IO (H5LTread_dataset_int (id, "rank", &mat->info->rank));
  }

  return mat;
}

/* write global vectors */
static void write_global_vectors (hid_t id, struct fclib_global *problem)
{
  hsize_t dim;

  dim = problem->M->m;
  ASSERT (problem->f, "ERROR: f must be given");
  IO (H5LTmake_dataset_double (id, "f", 1, &dim, problem->f));

  dim = problem->H->n;
  ASSERT (problem->w && problem->mu, "ERROR: w and mu must be given");
  IO (H5LTmake_dataset_double (id, "w", 1, &dim, problem->w));
  ASSERT (dim % problem->spacedim == 0, "ERROR: number of H columns is not divisble by the spatial dimension");
  dim /= problem->spacedim;
  IO (H5LTmake_dataset_double (id, "mu", 1, &dim, problem->mu));

  if (problem->G)
  {
    dim = problem->G->n;
    ASSERT (problem->b, "ERROR: b must be given if G is present");
    IO (H5LTmake_dataset_double (id, "b", 1, &dim, problem->b));
  }
}

/* read global vectors */
static void read_global_vectors (hid_t id, struct fclib_global *problem)
{
  MM (problem->f = malloc (sizeof (double [problem->M->m])));
  IO (H5LTread_dataset_double (id, "f", problem->f));

  ASSERT (problem->H->n % problem->spacedim == 0, "ERROR: number of H columns is not divisble by the spatial dimension");
  MM (problem->w = malloc (sizeof (double [problem->H->n])));
  MM (problem->mu = malloc (sizeof (double [problem->H->n / problem->spacedim])));
  IO (H5LTread_dataset_double (id, "w", problem->w));
  IO (H5LTread_dataset_double (id, "mu", problem->mu));

  if (problem->G)
  {
    MM (problem->b = malloc (sizeof (double [problem->G->n])));
    IO (H5LTread_dataset_double (id, "b", problem->b));
  }
}

/* write local vectors */
static void write_local_vectors (hid_t id, struct fclib_local *problem)
{
  hsize_t dim;

  dim = problem->W->m;
  ASSERT (problem->q, "ERROR: q must be given");
  IO (H5LTmake_dataset_double (id, "q", 1, &dim, problem->q));

  ASSERT (dim % problem->spacedim == 0, "ERROR: number of W rows is not divisble by the spatial dimension");
  dim /= problem->spacedim;
  IO (H5LTmake_dataset_double (id, "mu", 1, &dim, problem->mu));

  if (problem->V)
  {
    dim = problem->R->m;
    ASSERT (problem->s, "ERROR: s must be given if R is present");
    IO (H5LTmake_dataset_double (id, "s", 1, &dim, problem->s));
  }
}

/* read local vectors */
static void read_local_vectors (hid_t id, struct fclib_local *problem)
{
  MM (problem->q = malloc (sizeof (double [problem->W->m])));
  IO (H5LTread_dataset_double (id, "q", problem->q));

  ASSERT (problem->W->m % problem->spacedim == 0, "ERROR: number of W rows is not divisble by the spatial dimension");
  MM (problem->mu = malloc (sizeof (double [problem->W->m / problem->spacedim])));
  IO (H5LTread_dataset_double (id, "mu", problem->mu));

  if (problem->R)
  {
    MM (problem->s = malloc (sizeof (double [problem->R->m])));
    IO (H5LTread_dataset_double (id, "s", problem->s));
  }
}

/* write problem info */
static void write_problem_info (hid_t id, struct fclib_info *info)
{
  if (info->title) IO (H5LTmake_dataset_string (id, "title", info->title));
  if (info->description) IO (H5LTmake_dataset_string (id, "description", info->description));
  if (info->math_info) IO (H5LTmake_dataset_string (id, "math_info", info->math_info));
}

/* read problem info */
static struct fclib_info* read_problem_info (hid_t id)
{
  struct fclib_info *info;
  H5T_class_t class_id;
  hsize_t dim;
  size_t size;

  MM (info = malloc (sizeof (struct fclib_info)));

  if (H5LTfind_dataset (id, "title"))
  {
    IO (H5LTget_dataset_info  (id, "title", &dim, &class_id, &size));
    MM (info->title = malloc (sizeof (char [size])));
    IO (H5LTread_dataset_string (id, "title", info->title));
  }
  else info->title = NULL;

  if (H5LTfind_dataset (id, "description"))
  {
    IO (H5LTget_dataset_info  (id, "description", &dim, &class_id, &size));
    MM (info->description = malloc (sizeof (char [size])));
    IO (H5LTread_dataset_string (id, "description", info->description));
  }
  else info->description = NULL;

  if (H5LTfind_dataset (id, "math_info"))
  {
    IO (H5LTget_dataset_info  (id, "math_info", &dim, &class_id, &size));
    MM (info->math_info = malloc (sizeof (char [size])));
    IO (H5LTread_dataset_string (id, "math_info", info->math_info));
  }
  else info->math_info = NULL;

  return info;
}

/* write solution */
static void write_solution (hid_t id, struct fclib_solution *solution, hsize_t nv, hsize_t nr, hsize_t nl)
{
  if (nv) IO (H5LTmake_dataset_double (id, "v", 1, &nv, solution->v));
  if (nl) IO (H5LTmake_dataset_double (id, "l", 1, &nl, solution->l));

  ASSERT (nr, "ERROR: contact constraints must be present");
  IO (H5LTmake_dataset_double (id, "u", 1, &nr, solution->u));
  IO (H5LTmake_dataset_double (id, "r", 1, &nr, solution->r));
}

/* read solution */
static void read_solution (hid_t id, hsize_t nv, hsize_t nr, hsize_t nl, struct fclib_solution *solution)
{
  if (nv)
  {
    MM (solution->v = malloc (sizeof (double [nv])));
    IO (H5LTread_dataset_double (id, "v", solution->v));
  }
  else solution->v = NULL;

  if (nl)
  {
    MM (solution->l = malloc (sizeof (double [nl])));
    IO (H5LTread_dataset_double (id, "l", solution->l));
  }
  else solution->l = NULL;

  ASSERT (nr, "ERROR: contact constraints must be present");
  MM (solution->u = malloc (sizeof (double [nr])));
  IO (H5LTread_dataset_double (id, "u", solution->u));
  MM (solution->r = malloc (sizeof (double [nr])));
  IO (H5LTread_dataset_double (id, "r", solution->r));
}

/* read solution sizes */
static int read_nvnunrnl (hid_t file_id, int *nv, int *nr, int *nl)
{
  if (H5Lexists (file_id, "/fclib_global", H5P_DEFAULT))
  {
    IO (H5LTread_dataset_int (file_id, "/fclib_global/M/n", nv));
    IO (H5LTread_dataset_int (file_id, "/fclib_global/H/n", nr));
    if (H5Lexists (file_id, "/fclib_global/G", H5P_DEFAULT))
    {
      IO (H5LTread_dataset_int (file_id, "/fclib_global/G/n", nl));
    }
    else *nl = 0;
  }
  else if (H5Lexists (file_id, "/fclib_local", H5P_DEFAULT))
  {
    *nv = 0;
    IO (H5LTread_dataset_int (file_id, "/fclib_local/W/n", nr));
    if (H5Lexists (file_id, "/fclib_local/R", H5P_DEFAULT))
    {
      IO (H5LTread_dataset_int (file_id, "/fclib_local/R/n", nl));
    }
    else *nl = 0;
  }
  else
  {
    fprintf (stderr, "ERROR: neither global nor local problem has been stored. Global or local have to be stored before solutions or guesses\n");
    return 0;
  }

  return 1;
}

/* delete matrix info */
static void delete_matrix_info (struct fclib_matrix_info *info)
{
  if (info)
  {
    free (info->comment);
    free (info);
  }
}

/* delete matrix */
static void delete_matrix (struct fclib_matrix *mat)
{
  if (mat)
  {
    free (mat->p);
    free (mat->i);
    free (mat->x);
    delete_matrix_info (mat->info);
    free (mat);
  }
}

/* delete problem info */
static void delete_info (struct fclib_info *info)
{
  if (info)
  {
    if (info->title) free (info->title);
    if (info->description) free (info->description);
    if (info->math_info) free (info->math_info);
  }
}

/* =========================== interface ============================ */

/* write global problem;
 * return 1 on success, 0 on failure */
int fclib_write_global (struct fclib_global *problem, const char *path)
{
  hid_t  file_id, main_id, id;
  hsize_t dim = 1;
  FILE *f;

  if ((f = fopen (path, "r"))) /* HDF5 outputs lots of warnings when file does not exist */
  {
    fclose (f);
    if ((file_id = H5Fopen (path, H5F_ACC_RDWR, H5P_DEFAULT)) < 0)
    {
      fprintf (stderr, "ERROR: opening file failed\n");
      return 0;
    }
    
    if (H5Lexists (file_id, "/fclib_global", H5P_DEFAULT)) /* cannot overwrite existing datasets */
    {
      fprintf (stderr, "ERROR: a global problem has already been written to this file\n");
      return 0;
    }
  }
  else if ((file_id = H5Fcreate (path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) /* cerate */
  {
    fprintf (stderr, "ERROR: creating file failed\n");
    return 0;
  }

  IO (main_id = H5Gmake (file_id, "/fclib_global"));

  ASSERT (problem->spacedim == 2 || problem->spacedim == 3, "ERROR: space dimension must be 2 or 3");
  IO (H5LTmake_dataset_int (file_id, "/fclib_global/spacedim", 1, &dim, &problem->spacedim));

  ASSERT (problem->M, "ERROR: M must be given");
  IO (id = H5Gmake (file_id, "/fclib_global/M"));
  write_matrix (id, problem->M);
  IO (H5Gclose (id));

  ASSERT (problem->H, "ERROR: H must be given");
  IO (id = H5Gmake (file_id, "/fclib_global/H"));
  write_matrix (id, problem->H);
  IO (H5Gclose (id));

  if (problem->G)
  {
    IO (id = H5Gmake (file_id, "/fclib_global/G"));
    write_matrix (id, problem->G);
    IO (H5Gclose (id));
  }

  IO (id = H5Gmake (file_id, "/fclib_global/vectors"));
  write_global_vectors (id, problem);
  IO (H5Gclose (id));

  if (problem->info)
  {
    IO (id = H5Gmake (file_id, "/fclib_global/info"));
    write_problem_info (id, problem->info);
    IO (H5Gclose (id));
  }

  IO (H5Gclose (main_id));
  IO (H5Fclose (file_id));

  return 1;
}

/* write local problem;
 * return 1 on success, 0 on failure */
int fclib_write_local (struct fclib_local *problem, const char *path)
{
  hid_t  file_id, main_id, id;
  hsize_t dim = 1;
  FILE *f;

  if ((f = fopen (path, "r"))) /* HDF5 outputs lots of warnings when file does not exist */
  {
    fclose (f);
    if ((file_id = H5Fopen (path, H5F_ACC_RDWR, H5P_DEFAULT)) < 0)
    {
      fprintf (stderr, "ERROR: opening file failed\n");
      return 0;
    }
    
    if (H5Lexists (file_id, "/fclib_local", H5P_DEFAULT)) /* cannot overwrite existing datasets */
    {
      fprintf (stderr, "ERROR: a local problem has already been written to this file\n");
      return 0;
    }
  }
  else if ((file_id = H5Fcreate (path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) /* cerate */
  {
    fprintf (stderr, "ERROR: creating file failed\n");
    return 0;
  }

  IO (main_id = H5Gmake (file_id, "/fclib_local"));

  ASSERT (problem->spacedim == 2 || problem->spacedim == 3, "ERROR: space dimension must be 2 or 3");
  IO (H5LTmake_dataset_int (file_id, "/fclib_local/spacedim", 1, &dim, &problem->spacedim));

  ASSERT (problem->W, "ERROR: W must be given");
  IO (id = H5Gmake (file_id, "/fclib_local/W"));
  write_matrix (id, problem->W);
  IO (H5Gclose (id));

  if (problem->V && problem->R)
  {
    IO (id = H5Gmake (file_id, "/fclib_local/V"));
    write_matrix (id, problem->V);
    IO (H5Gclose (id));

    IO (id = H5Gmake (file_id, "/fclib_local/R"));
    write_matrix (id, problem->R);
    IO (H5Gclose (id));
  }
  else ASSERT (!problem->V && !problem->R, "ERROR: V and R must be defined at the same time");

  IO (id = H5Gmake (file_id, "/fclib_local/vectors"));
  write_local_vectors (id, problem);
  IO (H5Gclose (id));

  if (problem->info)
  {
    IO (id = H5Gmake (file_id, "/fclib_local/info"));
    write_problem_info (id, problem->info);
    IO (H5Gclose (id));
  }

  IO (H5Gclose (main_id));
  IO (H5Fclose (file_id));

  return 1;
}

/* write solution;
 * return 1 on success, 0 on failure */
int fclib_write_solution (struct fclib_solution *solution, const char *path)
{
  hid_t  file_id, id;
  int nv, nr, nl;
  FILE *f;

  if ((f = fopen (path, "r"))) /* HDF5 outputs lots of warnings when file does not exist */
  {
    fclose (f);
    if ((file_id = H5Fopen (path, H5F_ACC_RDWR, H5P_DEFAULT)) < 0)
    {
      fprintf (stderr, "ERROR: opening file failed\n");
      return 0;
    }
    
    if (H5Lexists (file_id, "/solution", H5P_DEFAULT)) /* cannot overwrite existing datasets */
    {
      fprintf (stderr, "ERROR: a solution has already been written to this file\n");
      return 0;
    }
  }
  else
  {
    fprintf (stderr, "ERROR: opening file failed\n");
    return 0;
  }

  if (! read_nvnunrnl (file_id, &nv, &nr, &nl)) return 0;

  IO (id = H5Gmake (file_id, "/solution"));
  write_solution (id, solution, nv, nr, nl);
  IO (H5Gclose (id));

  IO (H5Fclose (file_id));

  return 1;
}

/* write initial guesses;
 * return 1 on success, 0 on failure */
int fclib_write_guesses (int number_of_guesses,  struct fclib_solution *guesses, const char *path)
{
  hid_t  file_id, main_id, id;
  int nv, nr, nl, i;
  hsize_t dim = 1;
  char num [128];
  FILE *f;

  if ((f = fopen (path, "r"))) /* HDF5 outputs lots of warnings when file does not exist */
  {
    fclose (f);
    if ((file_id = H5Fopen (path, H5F_ACC_RDWR, H5P_DEFAULT)) < 0)
    {
      fprintf (stderr, "ERROR: opening file failed\n");
      return 0;
    }
    
    if (H5Lexists (file_id, "/guesses", H5P_DEFAULT)) /* cannot overwrite existing datasets */
    {
      fprintf (stderr, "ERROR: some guesses have already been written to this file\n");
      return 0;
    }
  }
  else
  {
    fprintf (stderr, "ERROR: opening file failed\n");
    return 0;
  }

  if (! read_nvnunrnl (file_id, &nv, &nr, &nl)) return 0;

  IO (main_id = H5Gmake (file_id, "/guesses"));
  IO (H5LTmake_dataset_int (file_id, "/guesses/number_of_guesses", 1, &dim, &number_of_guesses));

  for (i = 0; i < number_of_guesses; i ++)
  {
    snprintf (num, 128, "%d", i+1);
    IO (id = H5Gmake (main_id, num));
    write_solution (id, &guesses [i], nv, nr, nl);
    IO (H5Gclose (id));
  }

  IO (H5Gclose (main_id));
  IO (H5Fclose (file_id));

  return 1;
}

/* read global problem;
 * return problem on success; NULL on failure */
struct fclib_global* fclib_read_global (const char *path)
{
  struct fclib_global *problem;
  hid_t  file_id, main_id, id;

  if ((file_id = H5Fopen (path, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
  {
    fprintf (stderr, "ERROR: opening file failed\n");
    return NULL;
  }

  MM (problem = calloc (1, sizeof (struct fclib_global)));

  IO (main_id = H5Gopen (file_id, "/fclib_global", H5P_DEFAULT));
  IO (H5LTread_dataset_int (file_id, "/fclib_global/spacedim", &problem->spacedim));

  IO (id = H5Gopen (file_id, "/fclib_global/M", H5P_DEFAULT));
  problem->M = read_matrix (id);
  IO (H5Gclose (id));

  IO (id = H5Gopen (file_id, "/fclib_global/H", H5P_DEFAULT));
  problem->H = read_matrix (id);
  IO (H5Gclose (id));

  if (H5Lexists (file_id, "/fclib_global/G", H5P_DEFAULT))
  {
    IO (id = H5Gopen (file_id, "/fclib_global/G", H5P_DEFAULT));
    problem->G = read_matrix (id);
    IO (H5Gclose (id));
  }

  IO (id = H5Gopen (file_id, "/fclib_global/vectors", H5P_DEFAULT));
  read_global_vectors (id, problem);
  IO (H5Gclose (id));

  if (H5Lexists (file_id, "/fclib_global/info", H5P_DEFAULT))
  {
    IO (id = H5Gopen (file_id, "/fclib_global/info", H5P_DEFAULT));
    problem->info = read_problem_info (id);
    IO (H5Gclose (id));
  }

  IO (H5Gclose (main_id));
  IO (H5Fclose (file_id));

  return problem;
}

/* read local problem;
 * return problem on success; NULL on failure */
struct fclib_local* fclib_read_local (const char *path)
{
  struct fclib_local *problem;
  hid_t  file_id, main_id, id;

  if ((file_id = H5Fopen (path, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
  {
    fprintf (stderr, "ERROR: creating file failed\n");
    return 0;
  }

  MM (problem = calloc (1, sizeof (struct fclib_local)));

  IO (main_id = H5Gopen (file_id, "/fclib_local", H5P_DEFAULT));
  IO (H5LTread_dataset_int (file_id, "/fclib_local/spacedim", &problem->spacedim));

  IO (id = H5Gopen (file_id, "/fclib_local/W", H5P_DEFAULT));
  problem->W = read_matrix (id);
  IO (H5Gclose (id));

  if (H5Lexists (file_id, "/fclib_local/V", H5P_DEFAULT))
  {
    IO (id = H5Gopen (file_id, "/fclib_local/V", H5P_DEFAULT));
    problem->V = read_matrix (id);
    IO (H5Gclose (id));

    IO (id = H5Gopen (file_id, "/fclib_local/R", H5P_DEFAULT));
    problem->R = read_matrix (id);
    IO (H5Gclose (id));
  }

  IO (id = H5Gopen (file_id, "/fclib_local/vectors", H5P_DEFAULT));
  read_local_vectors (id, problem);
  IO (H5Gclose (id));

  if (H5Lexists (file_id, "/fclib_local/info", H5P_DEFAULT))
  {
    IO (id = H5Gopen (file_id, "/fclib_local/info", H5P_DEFAULT));
    problem->info = read_problem_info (id);
    IO (H5Gclose (id));
  }

  IO (H5Gclose (main_id));
  IO (H5Fclose (file_id));

  return problem;
}

/* read solution;
 * return solution on success; NULL on failure */
struct fclib_solution* fclib_read_solution (const char *path)
{
  struct fclib_solution *solution;
  hid_t  file_id, id;
  int nv, nr, nl;

  if ((file_id = H5Fopen (path, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)  /* overwrite */
  {
    fprintf (stderr, "ERROR: opening file failed\n");
    return 0;
  }

  MM (solution = malloc (sizeof (struct fclib_solution)));

  if (! read_nvnunrnl (file_id, &nv, &nr, &nl)) return 0;

  IO (id = H5Gopen (file_id, "/solution", H5P_DEFAULT));
  read_solution (id, nv, nr, nl, solution);
  IO (H5Gclose (id));

  IO (H5Fclose (file_id));

  return solution;
}

/* read initial guesses;
 * return vector of guesses on success; NULL on failure;
 * output numebr of guesses in the variable pointed by 'number_of_guesses' */
struct fclib_solution* fclib_read_guesses (const char *path, int *number_of_guesses)
{
  struct fclib_solution *guesses;
  hid_t  file_id, main_id, id;
  int nv, nr, nl, i;
  char num [128];

  if ((file_id = H5Fopen (path, H5F_ACC_RDWR, H5P_DEFAULT)) < 0)  /* overwrite */
  {
    fprintf (stderr, "ERROR: opening file failed\n");
    return 0;
  }

  if (! read_nvnunrnl (file_id, &nv, &nr, &nl)) return 0;

  if (H5Lexists (file_id, "/guesses", H5P_DEFAULT))
  {
    IO (main_id = H5Gopen (file_id, "/guesses", H5P_DEFAULT));

    IO (H5LTread_dataset_int (file_id, "/guesses/number_of_guesses", number_of_guesses));

    MM (guesses = malloc ((*number_of_guesses) * sizeof (struct fclib_solution)));

    for (i = 0; i < *number_of_guesses; i ++)
    {
      snprintf (num, 128, "%d", i+1);
      IO (id = H5Gopen (main_id, num, H5P_DEFAULT));
      read_solution (id, nv, nr, nl, &guesses [i]);
      IO (H5Gclose (id));
    }

    IO (H5Gclose (main_id));
  }

  IO (H5Fclose (file_id));

  return guesses;
}

/* delete global problem */
void fclib_delete_global (struct fclib_global *problem)
{
  delete_matrix (problem->M);
  delete_matrix (problem->H);
  delete_matrix (problem->G);
  free (problem->mu);
  free (problem->f);
  if (problem->b) free (problem->b);
  free (problem->w);
  delete_info (problem->info);
}

/* delete local problem */
void fclib_delete_local (struct fclib_local *problem)
{
  delete_matrix (problem->W);
  delete_matrix (problem->V);
  delete_matrix (problem->R);
  free (problem->mu);
  free (problem->q);
  if (problem->s) free (problem->s);
  delete_info (problem->info);
}

/* delete solutions or guesses */
void fclib_delete_solutions (struct fclib_solution *data, int count)
{
  int i;

  for (i = 0; i < count; i ++)
  {
    if (data [i].v) free (data [i].v);
    if (data [i].u) free (data [i].u);
    if (data [i].r) free (data [i].r);
    if (data [i].l) free (data [i].l);
  }

  free (data);
}
