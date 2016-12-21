/* 
 * fcmer.c
 * ----------------------------------------------
 * frictional contact library merit functions
 */

#include <string.h>
#include <stdlib.h>
#include <hdf5.h>
#include <hdf5_hl.h>
#include "fclib.h"
#include "fcint.h"
#include "math.h"


/* void fclib_matrix_to_cssparse(struct fclib_matrix * W, cs * M) */
/* { */
/*   M->nzmax = W->nzmax; */
/*   M->m = W->m; */
/*   M->n = W->n; */
/*   M->nz = W->nz; */
/*   M->p = W->p; */
/*   M->i = W->i; */
/*   M->x = W->x; */
/* } */

static inline double dnrm2(double * v ,  int n)
{
  int i;
  double norm2=0.0;
  for (i=0; i <n ; i++) norm2 += v[i]*v[i];
  return sqrt(norm2);
}

static inline void projectionOnCone(double* r, double  mu)
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

void FrictionContact3D_unitary_compute_and_add_error(double *z , double *w, double mu, double * error)
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
double fclib_merit_global (struct fclib_global *problem, enum fclib_merit merit, struct fclib_solution *solution)
{
  return 0; /* TODO */
}

/* calculate merit function for a local problem */
double fclib_merit_local (struct fclib_local *problem, enum fclib_merit merit, struct fclib_solution *solution)
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
