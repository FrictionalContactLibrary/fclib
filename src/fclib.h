/* FCLIB Copyright (C) 2011--2020 FClib project
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Contact: fclib-project@lists.gforge.inria.fr
*/


/*!\file fclib.h
  frictional contact library interface
  
  This C API provides functions to read and write Frictional contact
  problems in HDF5 format Two kind of problem are considered Given
  
  - a symmetric positive semi--definite matrix \f$ {W} \in {\mathrm{I\!R}}^{m \times m} \f$
  - a vector \f$ {q} \in {\mathrm{I\!R}}^m \f$
  - a vector of coefficients of friction \f$ \mu \in{\mathrm{I\!R}}^{n_c} \f$

  the local FC problem  is to find two vectors \f$ u\in{\ensuremath{\mathrm{I\!R}}}^m \f$,
  the relative local velocity and \f$ r\in {\ensuremath{\mathrm{I\!R}}}^m \f$,
  the contact forces denoted by  \f$ \mathrm{FC}(W,q,\mu) \f$   such that

  \f[
  \begin{cases}
  \hat u = W r + q +\left[
  \left[\begin{array}{c}
  \mu^\alpha \|u^\alpha_T\|\\
  0 \\
  0
  \end{array}\right]^T, \alpha = 1 \ldots n_c
  \right]^T \\ \\
  C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
  \end{cases}
  \f]

  where the Coulomb friction cone for a  contact  \f$ \alpha \f$  is defined by
  \f[
  C_{\mu^\alpha}^{\alpha}  = \{r^\alpha, \|r^\alpha_T \| \leq \mu^\alpha |r^\alpha_N| \}
  \f]
  and the set  \f$ C^{\alpha,\star}_{\mu^\alpha} \f$  is its dual.
  We are also dealing with global  FC problem defined by
  Given
  
  - a symmetric positive definite matrix  \f$ {M} \in {\mathrm{I\!R}}^{n \times n} \f$
  - a vector  \f$  {f} \in {\mathrm{I\!R}}^n \f$ ,
  - a matrix   \f$ {H} \in {\mathrm{I\!R}}^{n \times m} \f$
  - a matrix   \f$ {G} \in {\mathrm{I\!R}}^{n \times p} \f$
  - a vector  \f$ w \in {\mathrm{I\!R}}^{m} \f$ ,
  - a vector  \f$ b \in {\mathrm{I\!R}}^{p} \f$ ,
  - a vector of coefficients of friction  \f$ \mu \in {\mathrm{I\!R}}^{n_c} \f$ 
  
  the Global Mixed 3DFC problem is to find four vectors  \f$  {v} \in {\mathrm{I\!R}}^n \f$,
  \f$ u\in{\mathrm{I\!R}}^m \f$ ,  \f$ r\in {\mathrm{I\!R}}^m \f$  and  \f$ \lambda \in {\mathrm{I\!R}}^p \f$ 
  denoted by  \f$ \mathrm{GM3DFC}(M,H,G,w,b,\mu) \f$  such that

  \f[
  \begin{cases}
  M v = {H} {r} + G\lambda + {f} \\ \\
  G^T v +b =0 \\ \\
  \hat u = H^T v + w +\left[
  \left[\begin{array}{c}
  \mu \|u^\alpha_T\|\\
  0 \\
  0
  \end{array}\right]^T, \alpha = 1 \ldots n_c
  \right]^T \\ \\
  C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
  \end{cases}
  \f]
 
*/

#ifndef _fclib_h_
#define _fclib_h_

#ifndef FCLIB_APICOMPILE
#define FCLIB_APICOMPILE // This macro is MSCV specific. Just ignore this on non-windows system.
#endif

/*@ CONFIG @*/

/*@@ */

#ifdef FCLIB_HEADER_ONLY
// Header only : so we declare all functions as static functions.
#define FCLIB_STATIC static
#define FCLIB_IMPLEMENTATION
#else
// In that case FCLIB_STATIC is empty
#define FCLIB_STATIC
#endif

/*@@*/

/* choose api version */
#define H5Gcreate_vers 2
#define H5Gopen_vers 2

/**\struct  fclib_info fclib.h
 * This structure allows the user to enter a  problem information  as a title, a short description and known mathematical properties of the problem
 */

struct FCLIB_APICOMPILE fclib_info
{
  /** title of the problem*/
  char *title;
  /** short decription of the problem*/
  char *description;
  /** known properties of the problem (existence, uniqueness, ...)*/
  char *math_info;
};

/**\struct fclib_matrix_info fclib.h
 * This structure allows the user to enter a description for a given
 * matrix (comment, conditionning, determinant, rank.) if they are
 * known.
 */
struct FCLIB_APICOMPILE fclib_matrix_info  /* matrix information */
{
  /** comment on the matrix properties*/
  char *comment;
  /** conditioning*/
  double conditioning;
  /** determinant */
  double determinant;
  /** rank */
  int rank;
};

/**\struct  fclib_matrix fclib.h
 * matrix in compressed row/column or triplet form
 */
struct FCLIB_APICOMPILE fclib_matrix   /*  */
{
  /** maximum number of entries */
  int nzmax ;
  /** number of rows */
  int m ;
  /** number of columns */
  int n ;
  /** compressed: row (size m+1) or column (size n+1) pointers; triplet: row indices (size nz) */
  int *p ;
  /** compressed: column or row indices, size nzmax; triplet: column indices (size nz) */
  int *i ;
  /** numerical values, size nzmax */
  double *x ;
  /** # of entries in triplet matrix,   -1 for compressed columns,  -2 for compressed rows    */
  int nz ;
  /** info for this matrix */
  struct fclib_matrix_info *info;
};

/**
   The global frictional contact problem defined by
   
   Given

   - a symmetric positive definite matrix \f$ {M} \in {\mathrm{I\!R}}^{n \times n} \f$
   - a vector \f$ {f} \in {\mathrm{I\!R}}^n \f$,
   - a matrix  \f$ {H} \in {\mathrm{I\!R}}^{n \times m} \f$
   - a matrix  \f$ {G} \in {\mathrm{I\!R}}^{n \times p} \f$
   - a vector \f$ w \in {\mathrm{I\!R}}^{m} \f$,
   - a vector \f$ b \in {\mathrm{I\!R}}^{p} \f$,
   - a vector of coefficients of friction \f$ \mu \in {\mathrm{I\!R}}^{n_c} \f$

   the Global Mixed 3DFC problem  is to find four vectors \f$ {v} \in {\mathrm{I\!R}}^n \f$,
   \f$ u\in{\mathrm{I\!R}}^m \f$, \f$ r\in {\mathrm{I\!R}}^m \f$ and \f$ \lambda \in {\mathrm{I\!R}}^p \f$ denoted by
   \f$ \mathrm{GM3DFC}(M,H,G,w,b,\mu) \f$  such that
 
   \f[
   \begin{cases}
   M v = {H} {r} + G\lambda + {f} \\ \\
   G^T v +b =0 \\		      \
   \hat u = H^T v + w +\left[
   \left[\begin{array}{c}
   \mu \|u^\alpha_T\| \\
   0 \\
   0
   \end{array}\right]^T, \alpha = 1 \ldots n_c
   \right]^T \\ \\
   C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
   \end{cases}
   \f]


   where the Coulomb friction cone for a  contact \f$ \alpha \f$ is defined by

   \f[
   C_{\mu^\alpha}^{\alpha}  = \{r^\alpha, \|r^\alpha_T \| \leq \mu^\alpha |r^\alpha_N| \}
   \f]
   and the set \f$ C^{\alpha,\star}_{\mu^\alpha} \f$ is its dual.
*/
struct FCLIB_APICOMPILE fclib_global
{
  /** the matrix M (see mathematical description below)*/
  struct fclib_matrix *M;
  /** the matrix M (see mathematical description below)*/
  struct fclib_matrix *H;
  /** the matrix M (see mathematical description below)*/
  struct fclib_matrix *G;
  /** the vector \f$\mu\f$ of coefficient of friction (see mathematical description below)*/
  double *mu;
  /** the vector f (see mathematical description below)*/
  double *f;
  /** the vector b (see mathematical description below)*/
  double *b;
  /** the vector w (see mathematical description below)*/
  double *w;
  /** the dimension , 2 or 3, of the local space at contact (2d or 3d friction contact laws)*/
  int spacedim;
  /** info on the problem */
  struct fclib_info *info;
};
/**
   The global rolling frictional contact problem defined by
   
   Given

   - a symmetric positive definite matrix  \f$ {M} \in {\mathrm{I\!R}}^{n \times n} \f$-
   - a vector \f$ {f} \in {\mathrm{I\!R}}^n \f$ ,
   - a matrix   \f$ {H} \in {\mathrm{I\!R}}^{n \times m} \f$ 
   - a matrix   \f$ {G} \in {\mathrm{I\!R}}^{n \times p} \f$ 
   - a vector  \f$ w \in {\mathrm{I\!R}}^{m} \f$ ,
   - a vector  \f$ b \in {\mathrm{I\!R}}^{p} \f$ ,
   - a vector of coefficients of friction  \f$ \mu \in {\mathrm{I\!R}}^{n_c} \f$ 
   - a vector of coefficients of rolling friction  \f$ \mu_r \in {\mathrm{I\!R}}^{n_c} \f$ 

   the Global Mixed 3DFC problem  is to find four vectors \f$ {v} \in {\mathrm{I\!R}}^n \f$,
   \f$ u\in{\mathrm{I\!R}}^m \f$ , \f$ r\in {\mathrm{I\!R}}^m \f$  and
   \f$ \lambda \in {\mathrm{I\!R}}^p \f$  denoted by  \f$ \mathrm{GM3DFC}(M,H,G,w,b,\mu) \f$  such that

   \f[
   \begin{cases}
   M v = {H} {r} + G\lambda + {f} \\ \\
   G^T v +b =0 \\ \\
   \hat u = H^T v + w +\left[
   \left[\begin{array}{c}
   \mu \|u^\alpha_T\|\\
   0 \\
   0
   \end{array}\right]^T, \alpha = 1 \ldots n_c
   \right]^T \\ \\
   C^\star_{\mu,\mu_r} \ni {\hat u} \perp r \in C_{\mu,\mu_r}
   \end{cases}
   \f]
   
   where the Coulomb friction cone for a  contact  \f$ \alpha \f$  is defined by

   \f[
   C_{\mu^\alpha}^{\alpha}  = \{r^\alpha, \|r^\alpha_T \| \leq \mu^\alpha |r^\alpha_N| \}
   \f]
   
   and the set  \f$ C^{\alpha,\star}_{\mu^\alpha} \f$  is its dual.
 */
struct FCLIB_APICOMPILE fclib_global_rolling
{
  /** the matrix M (see mathematical description below)*/
  struct fclib_matrix *M;
  /** the matrix M (see mathematical description below)*/
  struct fclib_matrix *H;
  /** the matrix M (see mathematical description below)*/
  struct fclib_matrix *G;
  /** the vector  \f$ \mu \f$  of coefficient of friction (see mathematical description below)*/
  double *mu;
  /** the vector  \f$ \mu \f$  of rolling coefficient of friction (see mathematical description below)*/
  double *mu_r;
  /** the vector f (see mathematical description below)*/
  double *f;
  /** the vector b (see mathematical description below)*/
  double *b;
  /** the vector w (see mathematical description below)*/
  double *w;
  /** the dimension , 2 or 3, of the local space at contact (2d or 3d friction contact laws)*/
  int spacedim;
  /** info on the problem */
  struct fclib_info *info;
};
/**
   The local frictional contact problem defined by
   given

   - a positive semi--definite matrix   \f$ {W} \in {\mathrm{I\!R}}^{m \times m} \f$ 
   - a matrix   \f$ {V} \in {\mathrm{I\!R}}^{m \times p} \f$ 
   - a matrix   \f$ {R} \in {\mathrm{I\!R}}^{p \times p} \f$ 
   - a vector  \f$ q \in {\mathrm{I\!R}}^{m} \f$ ,
   - a vector  \f$ s \in {\mathrm{I\!R}}^{p} \f$ ,
   - a vector of coefficients of friction  \f$ \mu \in {\mathrm{I\!R}}^{n_c} \f$ 

   the  Mixed 3DFC problem  is to find three vectors  \f$ u\in{\mathrm{I\!R}}^m \f$ ,
   \f$ r\in {\mathrm{I\!R}}^m \f$  and  \f$ \lambda \in {\mathrm{I\!R}}^p \f$  denoted by
   \f$ \mathrm{M3DFC}(R,V,W,q,s,\mu) \f$   such that

   \f[
   \begin{cases}
   V^T {r} + R \lambda + s = 0 \\ \\
   \hat u = W {r}    + V\lambda  + q +\left[
   \left[\begin{array}{c}
   \mu^\alpha \|u^\alpha_T\|\\
   0 \\
   0
   \end{array}\right]^T, \alpha = 1 \ldots n_c
   \right]^T \\ \\
   C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
   \end{cases}
   \f]
   
   where the Coulomb friction cone for a  contact  \f$ \alpha \f$  is defined by

   \f[

   C_{\mu^\alpha}^{\alpha}  = \{r^\alpha, \|r^\alpha_T \| \leq \mu^\alpha |r^\alpha_N| \}
   \f]
   and the set  \f$ C^{\alpha,\star}_{\mu^\alpha} \f$  is its dual.
 */
struct FCLIB_APICOMPILE fclib_local
{
  /** the matrix W (see mathematical description below)*/
  struct fclib_matrix *W;
  /** the matrix V (see mathematical description below)*/
  struct fclib_matrix *V;
  /** the matrix R (see mathematical description below)*/
  struct fclib_matrix *R;
  /** the vector  \f$ \mu \f$  of coefficient of friction (see mathematical description below)*/
  double *mu;
  /** the vector q (see mathematical description below)*/
  double *q;
  /** the vector s (see mathematical description below)*/
  double *s;
  /** the dimension , 2 or 3, of the local space at contact (2d or 3d friction contact laws)*/
  int spacedim;             /* 2 or 3 */
  /** info on the problem */
  struct fclib_info *info;
};

/**
   A solution or a guess for the frictional contact problem.
 
   This structure allows to store a solution vector of a guess vector for the
   various frictional contact problems.
*/
struct FCLIB_APICOMPILE fclib_solution /* solution data */
{
  /** global velocity (or position/displacement for quasi-static problems) solution vector */
  double *v;
  /** local velocity  (or position/displacement for quasi-static problems) solution vector */
  double *u;
  /** local contact forces  (or impulses) solution vector */
  double *r;
  /** multiplier for equlity constraints ( \f$ \lambda \f$ ) solution vector */
  double *l;
};

/** MERIT_1 is a implementation of the merit function based on the natural map for a SOCCP
 */
enum FCLIB_APICOMPILE fclib_merit {MERIT_1, MERIT_2} ; /* merit functions */


#if defined(__cplusplus)
extern "C"
{
#endif



/** write global problem
 *
 *  \return 1 on success, 0 on failure */
FCLIB_STATIC int fclib_write_global (struct fclib_global *problem,
                                     const char *path);

/** write local problem
 *
 *  \return 1 on success, 0 on failure */
FCLIB_STATIC int fclib_write_local (struct fclib_local *problem,
                                    const char *path);
/** write global rolling problem
 *
 *  \return 1 on success, 0 on failure */
FCLIB_STATIC int fclib_write_global_rolling (struct fclib_global_rolling *problem,
                                             const char *path);

/** write solution
 *
 *  \return 1 on success, 0 on failure */
FCLIB_STATIC int fclib_write_solution (struct fclib_solution *solution,
                                       const char *path);

/** write initial guesses
 *
 *  \return 1 on success, 0 on failure */
FCLIB_STATIC int fclib_write_guesses (int number_of_guesses,
                                      struct fclib_solution *guesses,
                                      const char *path);


/** read global problem
 *
 *  \return problem on success; NULL on failure */
FCLIB_STATIC struct fclib_global* fclib_read_global (const char *path);

/** read local problem
 *
 *  \return problem on success; NULL on failure */
FCLIB_STATIC struct fclib_local* fclib_read_local (const char *path);

/** read global rolling problem
 *
 *  \return problem on success; NULL on failure */
FCLIB_STATIC struct fclib_global_rolling* fclib_read_global_rolling (const char *path);


/** read solution
 *
 *  \return solution on success; NULL on failure */
FCLIB_STATIC struct fclib_solution* fclib_read_solution (const char *path);

/** read initial guesses
 *
 *  \return vector of guesses on success; NULL on failure
 *  output number of guesses in the variable pointed by 'number_of_guesses' */
FCLIB_STATIC struct fclib_solution* fclib_read_guesses (const char *path,
                                                        int *number_of_guesses);

#ifdef FCLIB_WITH_MERIT_FUNCTIONS
/** calculate merit function for a global problem */
FCLIB_STATIC double fclib_merit_global (struct fclib_global *problem,
                                        enum fclib_merit merit,
                                        struct fclib_solution *solution);

/** calculate merit function for a local problem */
FCLIB_STATIC double fclib_merit_local (struct fclib_local *problem,
                                       enum fclib_merit merit,
                                       struct fclib_solution *solution);
#endif

/** delete global problem */
FCLIB_STATIC void fclib_delete_global (struct fclib_global *problem);

/** delete local problem */
FCLIB_STATIC void fclib_delete_local (struct fclib_local *problem);

/** delete global rolling problem */
FCLIB_STATIC void fclib_delete_global_rolling (struct fclib_global_rolling *problem);


/** delete solutions or guesses */
FCLIB_STATIC void fclib_delete_solutions (struct fclib_solution *data,
                                          int count);

/** create and set attributes of tyoe int in info */
FCLIB_STATIC int fclib_create_int_attributes_in_info(const char *path,
                                               const char * attr_name,
                                               int attr_value);


#if defined(__cplusplus)
}
#endif

  
/*@@*/
/* ========================= implementation =========================*/

#ifdef FCLIB_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <hdf5.h>
#include <hdf5_hl.h>

/* useful macros */
#define ASSERT(Test, ...)\
  do {\
  if (! (Test)) { fprintf (stderr, "%s: %d => ", __FILE__, __LINE__);\
    fprintf (stderr, __VA_ARGS__);\
    fprintf (stderr, "\n"); exit (1); } } while (0)

#define IO(Call) ASSERT ((Call) >= 0, "ERROR: HDF5 call failed")
#define MM(Call) ASSERT ((Call), "ERROR: out of memory")


/* make group */
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
    IO (H5LTmake_dataset_double (id, "x", 1, &dim, mat->x));
  }
  else if (mat->nz == -1) /* csc */
  {
    dim = mat->n+1;
    IO (H5LTmake_dataset_int (id, "p", 1, &dim, mat->p));
    dim = mat->nzmax;
    IO (H5LTmake_dataset_int (id, "i", 1, &dim, mat->i));
    IO (H5LTmake_dataset_double (id, "x", 1, &dim, mat->x));
  }
  else if (mat->nz == -2) /* csr */
  {
    dim = mat->m+1;
    IO (H5LTmake_dataset_int (id, "p", 1, &dim, mat->p));
    dim = mat->nzmax;
    IO (H5LTmake_dataset_int (id, "i", 1, &dim, mat->i));
    IO (H5LTmake_dataset_double (id, "x", 1, &dim, mat->x));
  }
  else ASSERT (0, "ERROR: unknown sparse matrix type => fclib_matrix->nz = %d\n", mat->nz);

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
static struct fclib_matrix* read_matrix (hid_t id)
{
  struct fclib_matrix *mat;

  MM (mat = (struct fclib_matrix*)malloc (sizeof (struct fclib_matrix)));

  IO (H5LTread_dataset_int (id, "nzmax", &mat->nzmax));
  IO (H5LTread_dataset_int (id, "m", &mat->m));
  IO (H5LTread_dataset_int (id, "n", &mat->n));
  IO (H5LTread_dataset_int (id, "nz", &mat->nz));

  if (mat->nz >= 0) /* triplet */
  {
    MM (mat->p = (int*)malloc (sizeof(int) * mat->nz));
    MM (mat->i = (int*)malloc (sizeof(int) * mat->nz));
    IO (H5LTread_dataset_int (id, "p", mat->p));
    IO (H5LTread_dataset_int (id, "i", mat->i));
  }
  else if (mat->nz == -1) /* csc */
  {
    MM (mat->p = (int*)malloc (sizeof(int)*(mat->n+1)));
    MM (mat->i = (int*)malloc (sizeof(int)*mat->nzmax));
    IO (H5LTread_dataset_int (id, "p", mat->p));
    IO (H5LTread_dataset_int (id, "i", mat->i));
  }
  else if (mat->nz == -2) /* csr */
  {
    MM (mat->p = (int*)malloc (sizeof(int)*(mat->m+1)));
    MM (mat->i = (int*)malloc (sizeof(int)*mat->nzmax));
    IO (H5LTread_dataset_int (id, "p", mat->p));
    IO (H5LTread_dataset_int (id, "i", mat->i));
  }
  else ASSERT (0, "ERROR: unknown sparse matrix type => fclib_matrix->nz = %d\n", mat->nz);

  MM (mat->x = (double*)malloc (sizeof(double)*mat->nzmax));
  IO (H5LTread_dataset_double (id, "x", mat->x));

  if (H5LTfind_dataset (id, "conditioning"))
  {
    H5T_class_t class_id;
    hsize_t dim;
    size_t size;

    MM (mat->info = (struct fclib_matrix_info*)malloc (sizeof (struct fclib_matrix_info)));
    if (H5LTfind_dataset (id, "comment"))
    {
      IO (H5LTget_dataset_info  (id, "comment", &dim, &class_id, &size));
      MM (mat->info->comment = (char*)malloc (sizeof(char)*size));
      IO (H5LTread_dataset_string (id, "comment", mat->info->comment));
    }
    else mat->info->comment = NULL;
    IO (H5LTread_dataset_double (id, "conditioning", &mat->info->conditioning));
    IO (H5LTread_dataset_double (id, "determinant", &mat->info->determinant));
    IO (H5LTread_dataset_int (id, "rank", &mat->info->rank));
  }
  else
  {
    mat->info = NULL;
  }

  return mat;
}

/* write global vectors */
static void write_global_vectors (hid_t id, struct fclib_global *problem)
{
  hsize_t dim;

  dim = (hsize_t)problem->M->m;
  ASSERT (problem->f, "ERROR: f must be given");
  IO (H5LTmake_dataset_double (id, "f", 1, &dim, problem->f));

  dim = (hsize_t)problem->H->n;
  ASSERT (problem->w && problem->mu, "ERROR: w and mu must be given");
  IO (H5LTmake_dataset_double (id, "w", 1, &dim, problem->w));
  ASSERT (dim % (hsize_t)problem->spacedim == 0, "ERROR: number of H columns is not divisble by the spatial dimension");
  dim /= (hsize_t)problem->spacedim;
  IO (H5LTmake_dataset_double (id, "mu", 1, &dim, problem->mu));

  if (problem->G)
  {
    dim = (hsize_t)problem->G->n;
    ASSERT (problem->b, "ERROR: b must be given if G is present");
    IO (H5LTmake_dataset_double (id, "b", 1, &dim, problem->b));
  }
}

/* read global vectors */
static void read_global_vectors (hid_t id, struct fclib_global *problem)
{
  MM (problem->f = (double*)malloc (sizeof(double)*problem->M->m));
  IO (H5LTread_dataset_double (id, "f", problem->f));

  ASSERT (problem->H->n % problem->spacedim == 0, "ERROR: number of H columns is not divisble by the spatial dimension");
  MM (problem->w = (double*)malloc (sizeof(double)*problem->H->n));
  MM (problem->mu = (double*)malloc (sizeof(double)*(problem->H->n / problem->spacedim)));
  IO (H5LTread_dataset_double (id, "w", problem->w));
  IO (H5LTread_dataset_double (id, "mu", problem->mu));

  if (problem->G)
  {
    MM (problem->b = (double*)malloc (sizeof(double)*problem->G->n));
    IO (H5LTread_dataset_double (id, "b", problem->b));
  }
}
/* write global vectors */
static void write_global_rolling_vectors (hid_t id, struct fclib_global_rolling *problem)
{
  hsize_t dim;

  dim = (hsize_t)problem->M->m;
  ASSERT (problem->f, "ERROR: f must be given");
  IO (H5LTmake_dataset_double (id, "f", 1, &dim, problem->f));

  dim = (hsize_t)problem->H->n;
  ASSERT (problem->w && problem->mu, "ERROR: w and mu must be given");
  IO (H5LTmake_dataset_double (id, "w", 1, &dim, problem->w));
  ASSERT (dim % (hsize_t)problem->spacedim == 0, "ERROR: number of H columns is not divisble by the spatial dimension");
  dim /= (hsize_t)problem->spacedim;
  IO (H5LTmake_dataset_double (id, "mu", 1, &dim, problem->mu));
  IO (H5LTmake_dataset_double (id, "mu_r", 1, &dim, problem->mu_r));

  if (problem->G)
  {
    dim = (hsize_t)problem->G->n;
    ASSERT (problem->b, "ERROR: b must be given if G is present");
    IO (H5LTmake_dataset_double (id, "b", 1, &dim, problem->b));
  }
}

/* read global vectors */
static void read_global_rolling_vectors (hid_t id, struct fclib_global_rolling *problem)
{
  MM (problem->f = (double*)malloc (sizeof(double)*problem->M->m));
  IO (H5LTread_dataset_double (id, "f", problem->f));

  ASSERT (problem->H->n % problem->spacedim == 0, "ERROR: number of H columns is not divisble by the spatial dimension");
  MM (problem->w = (double*)malloc (sizeof(double)*problem->H->n));
  MM (problem->mu = (double*)malloc (sizeof(double)*(problem->H->n / problem->spacedim)));
  MM (problem->mu_r = (double*)malloc (sizeof(double)*(problem->H->n / problem->spacedim)));
  IO (H5LTread_dataset_double (id, "w", problem->w));
  IO (H5LTread_dataset_double (id, "mu", problem->mu));
  IO (H5LTread_dataset_double (id, "mu_r", problem->mu_r));

  if (problem->G)
  {
    MM (problem->b = (double*)malloc (sizeof(double)*problem->G->n));
    IO (H5LTread_dataset_double (id, "b", problem->b));
  }
}
/* write local vectors */
static void write_local_vectors (hid_t id, struct fclib_local *problem)
{
  hsize_t dim;

  dim = (hsize_t)problem->W->m;
  ASSERT (problem->q, "ERROR: q must be given");
  IO (H5LTmake_dataset_double (id, "q", 1, &dim, problem->q));

  ASSERT (dim % (hsize_t)problem->spacedim == 0, "ERROR: number of W rows is not divisble by the spatial dimension");
  dim /= (hsize_t)problem->spacedim;
  IO (H5LTmake_dataset_double (id, "mu", 1, &dim, problem->mu));

  if (problem->V)
  {
    dim = (hsize_t)problem->R->m;
    ASSERT (problem->s, "ERROR: s must be given if R is present");
    IO (H5LTmake_dataset_double (id, "s", 1, &dim, problem->s));
  }
}

/* read local vectors */
static void read_local_vectors (hid_t id, struct fclib_local *problem)
{
  MM (problem->q = (double*)malloc (sizeof(double)*problem->W->m));
  IO (H5LTread_dataset_double (id, "q", problem->q));

  ASSERT (problem->W->m % problem->spacedim == 0, "ERROR: number of W rows is not divisble by the spatial dimension");
  MM (problem->mu = (double*)malloc (sizeof(double)*(problem->W->m / problem->spacedim)));
  IO (H5LTread_dataset_double (id, "mu", problem->mu));

  if (problem->R)
  {
    MM (problem->s = (double*)malloc (sizeof(double)*problem->R->m));
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

  MM (info = (struct fclib_info*)malloc (sizeof (struct fclib_info)));

  if (H5LTfind_dataset (id, "title"))
  {
    IO (H5LTget_dataset_info  (id, "title", &dim, &class_id, &size));
    MM (info->title = (char*)malloc (sizeof(char)*size));
    IO (H5LTread_dataset_string (id, "title", info->title));
  }
  else info->title = NULL;

  if (H5LTfind_dataset (id, "description"))
  {
    IO (H5LTget_dataset_info  (id, "description", &dim, &class_id, &size));
    MM (info->description = (char*)malloc (sizeof(char)*size));
    IO (H5LTread_dataset_string (id, "description", info->description));
  }
  else info->description = NULL;

  if (H5LTfind_dataset (id, "math_info"))
  {
    IO (H5LTget_dataset_info  (id, "math_info", &dim, &class_id, &size));
    MM (info->math_info = (char*)malloc (sizeof(char)*size));
    IO (H5LTread_dataset_string (id, "math_info", info->math_info));
  }
  else info->math_info = NULL;

  return info;
}

/* write solution */
static void write_solution (hid_t id, struct fclib_solution *solution, int nv, int nr, int nl)
{
  hsize_t nv_t = (hsize_t)nv;
  hsize_t nl_t = (hsize_t)nl;
  hsize_t nr_t = (hsize_t)nr;
  if (nv) IO (H5LTmake_dataset_double (id, "v", 1, &nv_t, solution->v));
  if (nl) IO (H5LTmake_dataset_double (id, "l", 1, &nl_t, solution->l));

  ASSERT (nr, "ERROR: contact constraints must be present");
  IO (H5LTmake_dataset_double (id, "u", 1, &nr_t, solution->u));
  IO (H5LTmake_dataset_double (id, "r", 1, &nr_t, solution->r));
}

/* read solution */
static void read_solution (hid_t id, int nv, int nr, int nl, struct fclib_solution *solution)
{
  if (nv)
  {
    MM (solution->v = (double*)malloc (sizeof(double)*nv));
    IO (H5LTread_dataset_double (id, "v", solution->v));
  }
  else solution->v = NULL;

  if (nl)
  {
    MM (solution->l = (double*)malloc (sizeof(double)*nl));
    IO (H5LTread_dataset_double (id, "l", solution->l));
  }
  else solution->l = NULL;

  ASSERT (nr, "ERROR: contact constraints must be present");
  MM (solution->u = (double*)malloc (sizeof(double)*nr));
  IO (H5LTread_dataset_double (id, "u", solution->u));
  MM (solution->r = (double*)malloc (sizeof(double)*nr));
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
  else if (H5Lexists (file_id, "/fclib_global_rolling", H5P_DEFAULT))
  {
    IO (H5LTread_dataset_int (file_id, "/fclib_global_rolling/M/n", nv));
    IO (H5LTread_dataset_int (file_id, "/fclib_global_rolling/H/n", nr));
    if (H5Lexists (file_id, "/fclib_global_rolling/G", H5P_DEFAULT))
    {
      IO (H5LTread_dataset_int (file_id, "/fclib_global_rolling/G/n", nl));
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
    free(info);
  }
}

FCLIB_STATIC int FCLIB_APICOMPILE fclib_create_int_attributes_in_info(const char *path, const char * attr_name,
                                        int attr_value)
{
  hid_t  file_id = -1, id, dataspace_id, attr_id;
  hsize_t     dims[1];
  FILE *f;

  if ((f = fopen (path, "r"))) /* HDF5 outputs lots of warnings when file does not exist */
  {
    fclose (f);
    if ((file_id = H5Fopen (path, H5F_ACC_RDWR, H5P_DEFAULT)) < 0)
    {
      fprintf (stderr, "ERROR: opening file failed\n");
      return 0;
    }
  }

  if (H5Lexists (file_id, "/fclib_local/info", H5P_DEFAULT))
  {
    IO (id = H5Gopen (file_id, "/fclib_local/info", H5P_DEFAULT));
    dims[0]=1;
    dataspace_id = H5Screate_simple(1, dims, NULL);
    attr_id = H5Acreate (id, attr_name, H5T_NATIVE_INT, dataspace_id,
                         H5P_DEFAULT, H5P_DEFAULT);
    IO(H5Awrite(attr_id, H5T_NATIVE_INT , &attr_value ));
    IO(H5Aclose (attr_id));
    IO (H5Gclose (id));
  }
  else
  {
    IO (id = H5Gmake (file_id, "/fclib_local/info"));
    dims[0]=1;
    dataspace_id = H5Screate_simple(1, dims, NULL);
    attr_id = H5Acreate (id, attr_name, H5T_NATIVE_INT, dataspace_id,
                         H5P_DEFAULT, H5P_DEFAULT);
    IO(H5Awrite(attr_id, H5T_NATIVE_INT , &attr_value ));
    IO(H5Aclose (attr_id));
    IO (H5Gclose (id));
  }
  IO (H5Fclose (file_id));

  return 1;
}

/* =========================== interface ============================ */

/* write global problem;
 * return 1 on success, 0 on failure */
FCLIB_STATIC int FCLIB_APICOMPILE fclib_write_global (struct fclib_global *problem, const char *path)
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

/* write global problem rolling;
 * return 1 on success, 0 on failure */
FCLIB_STATIC int FCLIB_APICOMPILE fclib_write_global_rolling (struct fclib_global_rolling *problem, const char *path)
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

    if (H5Lexists (file_id, "/fclib_global_rolling", H5P_DEFAULT)) /* cannot overwrite existing datasets */
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

  IO (main_id = H5Gmake (file_id, "/fclib_global_rolling"));

  ASSERT (problem->spacedim == 3 || problem->spacedim == 5, "ERROR: space dimension must be 3 or 5");
  IO (H5LTmake_dataset_int (file_id, "/fclib_global_rolling/spacedim", 1, &dim, &problem->spacedim));

  ASSERT (problem->M, "ERROR: M must be given");
  IO (id = H5Gmake (file_id, "/fclib_global_rolling/M"));
  write_matrix (id, problem->M);
  IO (H5Gclose (id));

  ASSERT (problem->H, "ERROR: H must be given");
  IO (id = H5Gmake (file_id, "/fclib_global_rolling/H"));
  write_matrix (id, problem->H);
  IO (H5Gclose (id));

  if (problem->G)
  {
    IO (id = H5Gmake (file_id, "/fclib_global_rolling/G"));
    write_matrix (id, problem->G);
    IO (H5Gclose (id));
  }

  IO (id = H5Gmake (file_id, "/fclib_global_rolling/vectors"));
  write_global_rolling_vectors (id, problem);
  IO (H5Gclose (id));

  if (problem->info)
  {
    IO (id = H5Gmake (file_id, "/fclib_global_rolling/info"));
    write_problem_info (id, problem->info);
    IO (H5Gclose (id));
  }

  IO (H5Gclose (main_id));
  IO (H5Fclose (file_id));

  return 1;
}






/* write local problem;
 * return 1 on success, 0 on failure */
FCLIB_STATIC int FCLIB_APICOMPILE fclib_write_local (struct fclib_local *problem, const char *path)
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
FCLIB_STATIC int FCLIB_APICOMPILE fclib_write_solution (struct fclib_solution *solution, const char *path)
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
FCLIB_STATIC int FCLIB_APICOMPILE fclib_write_guesses (int number_of_guesses,  struct fclib_solution *guesses, const char *path)
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
FCLIB_STATIC struct FCLIB_APICOMPILE fclib_global* fclib_read_global (const char *path)
{
  struct fclib_global *problem;
  hid_t  file_id, main_id, id;

  if ((file_id = H5Fopen (path, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
  {
    fprintf (stderr, "ERROR: opening file failed\n");
    return NULL;
  }

  MM (problem = (struct fclib_global*)calloc (1, sizeof (struct fclib_global)));

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
/* read global problem;
 * return problem on success; NULL on failure */
FCLIB_STATIC struct FCLIB_APICOMPILE fclib_global_rolling* fclib_read_global_rolling (const char *path)
{
  struct fclib_global_rolling *problem;
  hid_t  file_id, main_id, id;

  if ((file_id = H5Fopen (path, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
  {
    fprintf (stderr, "ERROR: opening file failed\n");
    return NULL;
  }

  MM (problem = (struct fclib_global_rolling*)calloc (1, sizeof (struct fclib_global_rolling)));

  IO (main_id = H5Gopen (file_id, "/fclib_global_rolling", H5P_DEFAULT));
  IO (H5LTread_dataset_int (file_id, "/fclib_global_rolling/spacedim", &problem->spacedim));

  IO (id = H5Gopen (file_id, "/fclib_global_rolling/M", H5P_DEFAULT));
  problem->M = read_matrix (id);
  IO (H5Gclose (id));

  IO (id = H5Gopen (file_id, "/fclib_global_rolling/H", H5P_DEFAULT));
  problem->H = read_matrix (id);
  IO (H5Gclose (id));

  if (H5Lexists (file_id, "/fclib_global_rolling/G", H5P_DEFAULT))
  {
    IO (id = H5Gopen (file_id, "/fclib_global_rolling/G", H5P_DEFAULT));
    problem->G = read_matrix (id);
    IO (H5Gclose (id));
  }

  IO (id = H5Gopen (file_id, "/fclib_global_rolling/vectors", H5P_DEFAULT));
  read_global_rolling_vectors (id, problem);
  IO (H5Gclose (id));

  if (H5Lexists (file_id, "/fclib_global_rolling/info", H5P_DEFAULT))
  {
    IO (id = H5Gopen (file_id, "/fclib_global_rolling/info", H5P_DEFAULT));
    problem->info = read_problem_info (id);
    IO (H5Gclose (id));
  }

  IO (H5Gclose (main_id));
  IO (H5Fclose (file_id));

  return problem;
}
/* read local problem;
 * return problem on success; NULL on failure */
FCLIB_STATIC struct FCLIB_APICOMPILE fclib_local* fclib_read_local (const char *path)
{
  struct fclib_local *problem;
  hid_t  file_id, main_id, id;

  if ((file_id = H5Fopen (path, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
  {
    fprintf (stderr, "ERROR: opening file failed\n");
    return NULL;
  }

  if (!H5Lexists (file_id, "/fclib_local", H5P_DEFAULT))
  {
    fprintf (stderr, "ERROR: spurious input file %s :: fclib_local group does not exists", path);
    return NULL;
  }

  MM (problem = (struct fclib_local*)calloc (1, sizeof (struct fclib_local)));

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
FCLIB_STATIC struct FCLIB_APICOMPILE fclib_solution* fclib_read_solution (const char *path)
{
  struct fclib_solution *solution;
  hid_t  file_id, id;
  int nv, nr, nl;

  if ((file_id = H5Fopen (path, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)  /* overwrite */
  {
    fprintf (stderr, "ERROR: opening file failed\n");
    return 0;
  }

  MM (solution = (struct fclib_solution*)malloc (sizeof (struct fclib_solution)));

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
FCLIB_STATIC struct FCLIB_APICOMPILE fclib_solution* fclib_read_guesses (const char *path, int *number_of_guesses)
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

    MM (guesses = (struct fclib_solution*)malloc ((*number_of_guesses) * sizeof (struct fclib_solution)));

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
FCLIB_STATIC void FCLIB_APICOMPILE fclib_delete_global (struct fclib_global *problem)
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
FCLIB_STATIC void FCLIB_APICOMPILE fclib_delete_local (struct fclib_local *problem)
{
  delete_matrix (problem->W);
  delete_matrix (problem->V);
  delete_matrix (problem->R);
  free (problem->mu);
  free (problem->q);
  if (problem->s) free (problem->s);
  delete_info (problem->info);
}
/* delete global problem */
FCLIB_STATIC void FCLIB_APICOMPILE fclib_delete_global_rolling (struct fclib_global_rolling *problem)
{
  delete_matrix (problem->M);
  delete_matrix (problem->H);
  delete_matrix (problem->G);
  free (problem->mu);
  free (problem->mu_r);
  free (problem->f);
  if (problem->b) free (problem->b);
  free (problem->w);
  delete_info (problem->info);
}



/* delete solutions or guesses */
FCLIB_STATIC void FCLIB_APICOMPILE fclib_delete_solutions (struct fclib_solution *data, int count)
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

#ifdef FCLIB_WITH_MERIT_FUNCTIONS
#include "cs.h"


FCLIB_STATIC inline double dnrm2(double * v ,  int n)
{
  int i;
  double norm2=0.0;
  for (i=0; i <n ; i++) norm2 += v[i]*v[i];
  return sqrt(norm2);
}

FCLIB_STATIC inline void projectionOnCone(double* r, double  mu)
{
  double normT = hypot(r[1], r[2]);
  if (mu * normT <= - r[0])
  {
    r[0] = 0.0;
    r[1] = 0.0;
    r[2] = 0.0;
    return ;
  }
  else if (normT <= mu * r[0])
  {
    return ;
  }
  else
  {
    double mu2 = mu * mu;
    r[0] = (mu * normT + r[0]) / (mu2 + 1.0);
    r[1] = mu * r[0] * r[1] / normT;
    r[2] = mu * r[0] * r[2] / normT;
    return;
  }
}

FCLIB_STATIC void FrictionContact3D_unitary_compute_and_add_error(double *z , double *w, double mu, double * error)
{

  double normUT;
  double worktmp[3];
  /* Compute the modified local velocity */
  normUT = hypot(w[1], w[2]); // i.e sqrt(w[ic3p1]*w[ic3p1]+w[ic3p2]*w[ic3p2]);
  worktmp[0] = z[0] -  (w[0] + mu * normUT);
  worktmp[1] = z[1] -   w[1] ;
  worktmp[2] = z[2] -  w[2] ;
  projectionOnCone(worktmp, mu);
  worktmp[0] = z[0] -  worktmp[0];
  worktmp[1] = z[1] -  worktmp[1];
  worktmp[2] = z[2] -  worktmp[2];
  *error +=  worktmp[0] * worktmp[0] + worktmp[1] * worktmp[1] + worktmp[2] * worktmp[2];

}

/* calculate merit function for a global problem */
FCLIB_STATIC double fclib_merit_global (struct fclib_global *problem, enum fclib_merit merit, struct fclib_solution *solution)
{
  return 0; /* TODO */
}

/* calculate merit function for a local problem */
FCLIB_STATIC double fclib_merit_local (struct fclib_local *problem, enum fclib_merit merit, struct fclib_solution *solution)
{

  struct fclib_matrix * W =  problem->W;
  struct fclib_matrix * V =  problem->V;
  struct fclib_matrix * R =  problem->R;

  double *mu = problem->mu;
  double *q = problem->q;
  double *s = problem->s;
  int d = problem->spacedim;
  if (d !=3 )
  {
    printf("fclib_merit_local for space dimension = %i not yet implemented\n",d);
    return 0;
  }

  double *v = solution->v;
  double *r = solution->r;
  double *u = solution->u;
  double *l = solution->l;

  double error_l, error;
  double * tmp;

  error=0.0;
  error_l=0.0;
  int i, ic, ic3;
  if (merit == MERIT_1)
  {

    /* cs M_cs;  */
    /* fclib_matrix_to_cssparse(W, &M_cs); */
    /* cs V_cs;  */
    /* fclib_matrix_to_cssparse(V, &V_cs); */
    /* cs R_cs;  */
    /* fclib_matrix_to_cssparse(R, &R_cs); */
    int n_e =0;
    if (R) n_e = R->n;
    /* compute V^T {r} + R \lambda + s */
    if (n_e >0)
    {
      cs * VT = cs_transpose((cs *)V, 0) ;
      tmp = (double *)malloc(n_e*sizeof(double));
      for (i =0; i <n_e; i++) tmp[i] = s[i] ;
      cs_gaxpy(VT, r, tmp);
      cs_gaxpy((cs *)R, l, tmp);
      error_l += dnrm2(tmp,n_e)/(1.0 +  dnrm2(s,n_e) );
      free(tmp);
    }
    /* compute  \hat u = W {r}    + V\lambda  + q  */

    tmp = (double *)malloc(W->n*sizeof(double));
    for (i =0; i <W->n; i++) tmp[i] = q[i] ;
    cs_gaxpy((cs*)V, l, tmp);
    cs_gaxpy((cs*)W, r, tmp);

    /* Compute natural map */
    int nc = W->n/3;
    for (ic = 0, ic3 = 0 ; ic < nc ; ic++, ic3 += 3)
    {
      FrictionContact3D_unitary_compute_and_add_error(r + ic3, tmp + ic3, mu[ic], &error);
    }

    free(tmp);
    error = sqrt(error)/(1.0 +  sqrt(dnrm2(q,W->n)) )+error_l;

    /* printf("error_l = %12.8e", error_l); */
    /* printf("norm of u  = %12.8e\n",  dnrm2(u,W->n)); */
    /* printf("norm of r  = %12.8e\n",  dnrm2(r,W->n)); */
    /* printf("error = %12.8e\n", error); */

    return error;
  }

  return 0; /* TODO */
}
#endif /* FCLIB_WITH_MERIT_FUNCTIONS */

#endif /* FCLIB_IMPLEMENTATION */
/*@@*/

#endif /* _fclib_h_ */
