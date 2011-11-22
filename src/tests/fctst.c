/* 
 * fctst.c
 * ----------------------------------------------
 * frictional contact test
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "fclib.h"
#include "fcint.h"

/* allocate matrix info */
static struct fclib_matrix_info* matrix_info (struct fclib_matrix *mat, char *comment)
{
  struct fclib_matrix_info *info;

  MM (info = malloc (sizeof (struct fclib_matrix_info)));
  MM (info->comment = malloc (strlen (comment) + 1));
  strcpy (info->comment, comment);
  info->conditioning = rand ();
  info->determinant = rand ();
  info->rank = mat->m;

  return info;
}

/* generate random sparse matrix */
static struct fclib_matrix* random_matrix (int m, int n)
{
  struct fclib_matrix *mat;
  int j;

  MM (mat = malloc (sizeof (struct fclib_matrix)));
  mat->m = m;
  mat->n = n;
  mat->nzmax = (m + n) + m*n / (10 + rand () % 10);
  if (mat->nzmax > m*n) mat->nzmax = m*n;

  if (rand () % 2) /* triplet */
  {
    mat->nz = mat->nzmax;
    MM (mat->p = malloc (sizeof (int [mat->nzmax])));
    MM (mat->i = malloc (sizeof (int [mat->nzmax])));
    for (j = 0; j < mat->nzmax; j ++)
    {
      mat->p [j] = rand () % mat->m;
      mat->i [j] = rand () % mat->n;
    }
  }
  else
  {
    mat->nz = (rand () % 2 ? -1 : -2); /* csc / csr */
    int k = (mat->nz == -1 ? mat->n : mat->m);
    MM (mat->p = malloc (sizeof (int [k+1])));
    MM (mat->i = malloc (sizeof (int [mat->nzmax])));
    int l = mat->nzmax / k;
    for (mat->p [0] = j = 0; j < k; j ++) mat->p [j+1] = mat->p [j] + l;
    for (j = 0; j < mat->nzmax; j ++) mat->i [j] = rand () % k;
  }

  MM (mat->x = malloc (sizeof (double [mat->nzmax])));
  for (j = 0; j < mat->nzmax; j ++) mat->x [j] = (double) rand ()  / (double) RAND_MAX;

  if (rand ()) mat->info = matrix_info (mat, "A random matrix");
  else mat->info = NULL;

  return mat;
}

/* generate random vector */
static double* random_vector (int n)
{
  double *v;

  MM (v = malloc (sizeof (double [n])));
  for (n --; n >= 0; n --) v [n] = (double) rand () / (double) RAND_MAX;

  return v;
}

/* allocate problem info */
static struct fclib_info* problem_info (char *title, char *desc, char *math)
{
  struct fclib_info *info;

  MM (info = malloc (sizeof (struct fclib_info)));
  MM (info->title = malloc (strlen (title) + 1));
  strcpy (info->title, title);
  MM (info->description = malloc (strlen (desc) + 1));
  strcpy (info->description, desc);
  MM (info->math_info  = malloc (strlen (math) + 1));
  strcpy (info->math_info, math);

  return info;
}

/* generate random global problem */
static struct fclib_global* random_global_problem (int global_dofs, int contact_points, int neq)
{
  struct fclib_global *problem;

  MM (problem = malloc (sizeof (struct fclib_global)));
  if (rand () % 2) problem->spacedim = 2;
  else problem->spacedim = 3;
  problem->M = random_matrix (global_dofs, global_dofs);
  problem->H = random_matrix (global_dofs, problem->spacedim*contact_points);
  if (neq && rand () % 2) problem->G = random_matrix (global_dofs, neq);
  else problem->G = NULL;
  problem->mu = random_vector (contact_points);
  problem->f = random_vector (global_dofs);
  if (problem->G) problem->b = random_vector (problem->G->n);
  else problem->b = NULL;
  problem->w = random_vector (problem->spacedim*contact_points);
  if (rand () % 2) problem->info = problem_info ("A random global problem", "With random matrices", "And fake math");
  else problem->info = NULL;

  return problem;
}

/* generate random global solutions */
static struct fclib_solution* random_global_solutions (struct fclib_global *problem, int count)
{
  struct fclib_solution *sol;
  int i;

  MM (sol = malloc (count * sizeof (struct fclib_solution)));

  for (i = 0; i < count; i ++)
  {
    sol [i].v = random_vector (problem->M->n);
    sol [i].u = random_vector (problem->H->n);
    sol [i].r = random_vector (problem->H->n);
    if (problem->G) sol [i].l = random_vector (problem->G->n);
    else sol [i].l = NULL;
  }

  return sol;
}

/* generate random local problem */
static struct fclib_local* random_local_problem (int contact_points, int neq)
{
  struct fclib_local *problem;

  MM (problem = malloc (sizeof (struct fclib_local)));
  if (rand () % 2) problem->spacedim = 2;
  else problem->spacedim = 3;
  problem->W = random_matrix (problem->spacedim*contact_points, problem->spacedim*contact_points);
  if (neq && rand () % 2)
  {
    problem->V = random_matrix (problem->spacedim*contact_points, neq);
    problem->R = random_matrix (neq, neq);
    problem->s = random_vector (neq);
  }
  else
  {
    problem->V = problem->R = NULL;
    problem->s = NULL;
  }
  problem->mu = random_vector (contact_points);
  problem->q = random_vector (problem->spacedim*contact_points);
  if (rand () % 2) problem->info = problem_info ("A random local problem", "With random matrices", "And fake math");
  else problem->info = NULL;

  return problem;
}

/* generate random local solutions */
static struct fclib_solution* random_local_solutions (struct fclib_local *problem, int count)
{
  struct fclib_solution *sol;
  int i;

  MM (sol = malloc (count * sizeof (struct fclib_solution)));

  for (i = 0; i < count; i ++)
  {
    sol [i].v = NULL;
    sol [i].u = random_vector (problem->W->n);
    sol [i].r = random_vector (problem->W->n);
    if (problem->R) sol [i].l = random_vector (problem->R->n);
    else sol [i].l = NULL;
  }

  return sol;
}

/* compare matrix infos */
static int compare_matrix_infos (struct fclib_matrix_info *a, struct fclib_matrix_info *b)
{
  if (!a && !b) return 1;
  else if ((a && !b) || (!a && b)) return 0;
  else if (strcmp (a->comment, b->comment) != 0 ||
      a->conditioning != b->conditioning ||
      a->determinant != b->determinant ||
      a->rank != b->rank) return 0;

  return 1; 
}

/* compare two matrices */
static int compare_matrices (char *name, struct fclib_matrix *a, struct fclib_matrix *b)
{
  int i;

  if (!a && !b) return 1;
  else if ((a && !b) || (!a && b)) return 0;

  if (a->nzmax != b->nzmax ||
      a->n != b->n ||
      a->m != b->m ||
      a->nz != b->nz)
  {
    fprintf (stderr,
	     "ERROR: dimensions of matrix %s differ:\n"
	     "a->nzmax = %d, b->nzmax = %d\n"
	     "a->n = %d, b->n = %d\n"
	     "a->m = %d, b->m = %d\n"
	     "a->nz = %d, b->nz = %d\n", name,
	      a->nzmax, b->nzmax,
	      a->n, b->n,
	      a->m, b->m,
	      a->nz, b->nz);
    return 0;
  }

  if (a->nz >= 0)
  {
    for (i = 0; i < a->nzmax; i ++)
    {
      if (a->p [i] != b->p [i])
      {
	fprintf (stderr,
	         "ERROR: For %s in {a,b} a->p [%d] != b->p [%d] => %d != %d\n",
		 name, i, i, a->p [i], b->p [i]);
	return 0;
      }
      if (a->i [i] != b->i [i])
      {
	fprintf (stderr,
	         "ERROR: For %s in {a,b} a->i [%d] != b->i [%d] => %d != %d\n",
		 name, i, i, a->i [i], b->i [i]);
	return 0;
      }
    }
  }
  else if (a->nz == -1)
  {
    for (i = 0; i < a->n+1; i ++)
      if (a->p [i] != b->p [i])
      {
	fprintf (stderr,
	         "ERROR: For %s in {a,b} a->p [%d] != b->p [%d] => %d != %d\n",
		 name, i, i, a->p [i], b->p [i]);
	return 0;
      }

    for (i = 0; i < a->nzmax; i ++)
      if (a->i [i] != b->i [i])
      {
	fprintf (stderr,
	         "ERROR: For %s in {a,b} a->i [%d] != b->i [%d] => %d != %d\n",
		 name, i, i, a->i [i], b->i [i]);
	return 0;
      }
  }
  else if (a->nz == -2)
  {
    for (i = 0; i < a->m+1; i ++)
      if (a->p [i] != b->p [i])
      {
	fprintf (stderr,
	         "ERROR: For %s in {a,b} a->p [%d] != b->p [%d] => %d != %d\n",
		 name, i, i, a->p [i], b->p [i]);
	return 0;
      }

    for (i = 0; i < a->nzmax; i ++)
      if (a->i [i] != b->i [i])
      {
	fprintf (stderr,
	         "ERROR: For %s in {a,b} a->i [%d] != b->i [%d] => %d != %d\n",
		 name, i, i, a->i [i], b->i [i]);
	return 0;
      }
  }

  for (i = 0; i < a->nzmax; i ++)
    if (a->x [i] != b->x [i]) 
    {
	fprintf (stderr,
	         "ERROR: For %s in {a, b} a->x [%d] != b->x [%d] => %g != %g\n",
		 name, i, i, a->x [i], b->x [i]);
      return 0;
    }

  if (! compare_matrix_infos (a->info, b->info))
  {
    fprintf (stderr, "ERROR: matrix %s infos differ\n", name);
    return 0;
  }

  return 1;
}

/* compare two vectors */
static int compare_vectors (char *name, int n, double *a, double *b)
{
  int i;

  if (!a && !b) return 1;
  else if ((a && !b) || (!a && b)) return 0;

  for (i = 0; i < n; i ++)
    if (a [i] != b [i])
    {
      fprintf (stderr,
	       "ERROR: for %s in {a, b} a [%d] != b [%d] => %g != %g\n",
	       name, i, i, a [i], b [i]);
      return 0;
    }

  return 1;
}

/* compare problem infos */
static int compare_infos (struct fclib_info *a, struct fclib_info *b)
{
  if (!a && !b) return 1;
  else if ((a && !b) || (!a && b)) return 0;
  else if (strcmp (a->title, b->title) != 0 ||
      strcmp (a->description, b->description) != 0 ||
      strcmp (a->math_info, b->math_info) != 0) return 0;

  return 1;
}

/* compare global problems */
static int compare_global_problems (struct fclib_global *a, struct fclib_global *b)
{
  if (! compare_matrices ("M", a->M, b->M) ||
      ! compare_matrices ("H", a->H, b->H) ||
      ! compare_matrices ("G", a->G, b->G) ||
      ! compare_vectors  ("mu", a->H->n / a->spacedim, a->mu, b->mu) ||
      ! compare_vectors  ("f", a->M->m, a->f, b->f) ||
      ! compare_vectors  ("b", a->G ? a->G->n : 0, a->b, b->b) ||
      ! compare_vectors  ("w", a->H->n, a->w, b->w) ||
      a->spacedim != b->spacedim ||
      ! compare_infos (a->info, b->info)) return 0;

  return 1;
}

/* compare local problems */
static int compare_local_problems (struct fclib_local *a, struct fclib_local *b)
{
  if (! compare_matrices ("W", a->W, b->W) ||
      ! compare_matrices ("V", a->V, b->V) ||
      ! compare_matrices ("R", a->R, b->R) ||
      ! compare_vectors  ("mu", a->W->n / a->spacedim, a->mu, b->mu) ||
      ! compare_vectors  ("q", a->W->n, a->q, b->q) ||
      ! compare_vectors  ("s", a->R ? a->R->n : 0, a->s, b->s) ||
      a->spacedim != b->spacedim ||
      ! compare_infos (a->info, b->info)) return 0;

  return 1;
}

/* compare solutions */
static int compare_solutions (struct fclib_solution *a, struct fclib_solution *b, int nv, int nr, int nl)
{
  if (! compare_vectors ("v", nv, a->v, b->v) ||
      ! compare_vectors ("u", nr, a->u, b->u) ||
      ! compare_vectors ("r", nr, a->r, b->r) ||
      ! compare_vectors ("l", nl, a->l, b->l)) return 0;

  return 1;
}

int main (int argc, char **argv)
{
  int i;

  srand (time (NULL));

  if (rand () % 2)
  {
    struct fclib_global *problem, *p;
    struct fclib_solution *solution, *s;
    struct fclib_solution *guesses, *g;
    int numguess = rand () % 10, n;
    short allfine = 0;

    problem = random_global_problem (10 + rand () % 900, 10 + rand () % 900, 10 + rand () % 900);
    solution = random_global_solutions (problem, 1);
    guesses = random_global_solutions (problem, numguess);

    if (fclib_write_global (problem, "output_file.hdf5"))
      if (fclib_write_solution (solution, "output_file.hdf5"))
        if (fclib_write_guesses (numguess, guesses, "output_file.hdf5")) allfine = 1;

    if (allfine)
    {
      p = fclib_read_global ("output_file.hdf5");
      s = fclib_read_solution ("output_file.hdf5");
      g = fclib_read_guesses ("output_file.hdf5", &n);

      printf ("Comparing written and read global problem data ...\n");

      ASSERT (compare_global_problems (problem, p), "ERROR: written/read problem comparison failed");
      ASSERT (compare_solutions (solution, s, p->M->n, p->H->n, (p->G ? p->G->n : 0)), "ERROR: written/read solution comparison failed");
      ASSERT (numguess == n, "ERROR: numbers of written and read guesses differ");
      for (i = 0; i < n; i ++)
      {
        ASSERT (compare_solutions (guesses+i, g+i, p->M->n, p->H->n, (p->G ? p->G->n : 0)), "ERROR: written/read guess comparison failed");
      }

      printf ("All comparisons PASSED\n");
    
      fclib_delete_global (p);
      fclib_delete_solutions (s, 1);
      fclib_delete_solutions (g, n);
    }

    fclib_delete_global (problem);
    fclib_delete_solutions (solution, 1);
    fclib_delete_solutions (guesses, numguess);
  }
  else
  {
    struct fclib_local *problem, *p;
    struct fclib_solution *solution, *s;
    struct fclib_solution *guesses, *g;
    int numguess = rand () % 10, n;
    short allfine = 0;

    problem = random_local_problem (10 + rand () % 900, 10 + rand () % 900);
    solution = random_local_solutions (problem, 1);
    guesses = random_local_solutions (problem, numguess);

    if (fclib_write_local (problem, "output_file.hdf5"))
      if (fclib_write_solution (solution, "output_file.hdf5"))
        if (fclib_write_guesses (numguess, guesses, "output_file.hdf5")) allfine = 1;

    if (allfine)
    {
      p = fclib_read_local ("output_file.hdf5");
      s = fclib_read_solution ("output_file.hdf5");
      g = fclib_read_guesses ("output_file.hdf5", &n);

      printf ("Comparing written and read local problem data ...\n");

      ASSERT (compare_local_problems (problem, p), "ERROR: written/read problem comparison failed");
      ASSERT (compare_solutions (solution, s, 0, p->W->m, (p->R ? p->R->n : 0)), "ERROR: written/read solution comparison failed");
      ASSERT (numguess == n, "ERROR: numbers of written and read guesses differ");
      for (i = 0; i < n; i ++)
      {
        ASSERT (compare_solutions (guesses+i, g+i, 0, p->W->n, (p->R ? p->R->n : 0)), "ERROR: written/read guess comparison failed");
      }

      printf ("All comparions PASSED\n");
    
      fclib_delete_local (p);
      fclib_delete_solutions (s, 1);
      fclib_delete_solutions (g, n);
    }

    fclib_delete_local (problem);
    fclib_delete_solutions (solution, 1);
    fclib_delete_solutions (guesses, numguess);
  }

   remove ("output_file.hdf5"); 

  return 0;
}
