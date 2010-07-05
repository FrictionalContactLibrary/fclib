/* 
 * 3dfc.h
 * ------------------------------------------------------
 * initial idea for the fclib interface
 * ------------------------------------------------------
 * Authors:
 * --------
 * 2010, Tomasz Koziara
 */

/* COMMENTS
 * ========
 * The naming convention does not seem most elegant to me.
 * Perhaps 3dfc.h should be changed into fclib.h or similar.
 * Also DAT3DFC and MEA3DFC should be changed into something
 * less cumbersome.
 *
 * My objective here was to have a possibly simple interface
 * that only uses simple C types.
 *
 * (To be removed later on)
 */

#ifndef __3dfc__
#define __3dfc__

/* private data */
typedef struct dat3dfc DAT3DFC;

enum mea3dfc
{
  MEASURE_A, /* these could correspond to various merit functions */
  MEASURE_B
};

/* accuracy measures */
typedef enum mea3dfc MEA3DFC;

/* open for writing;
 * returns NULL on failure */
DAT3DFC* DAT3DFC_Write (const char *path,
                        int bodies,       /* number of bodies */
			int constraints); /* number of constraints */

/* open for reading;
 * returns NULL on failure */
DAT3DFC* DAT3DFC_Read (const char *path,
                       int *bodies,
		       int *constraints,
		       int *nzmax);      /* maximal number of nonzero elements in per-body matrix; can be NULL */

/* read/write body;
 * return 1 on success, 0 otherwise */
int DAT3DFC_Body (DAT3DFC *dat,
                  int index,                 /* body index */
                  int *n,                    /* body velocity degrees of freedom count */
		  double *M, int *p, int *i, /* mass matrix (n x n); can be compressed row for (p)pointers and (i)ndices not NULL ($$$);
                                                'nzmax' allows to asses buffer sizes in read mode; p[0] < 0 indicates dense matrix in read mode */
		  double *f);                /* f in: M new_velocity = f + constraint_impulses */

/* read/write constraint;
 * return 1 on success, 0 otherwise */
int DAT3DFC_Constraint (DAT3DFC *dat,
                        int index,                             /* constraint index */
                        int *b1, double *H1, int *p1, int *i1, /* first body index b1, first body part of the H-operator (or G); ($$$) */
			int *b2, double *H2, int *p2, int *i2, /* second body index b2, -||-; above and here 'nzmax' allows to asses buffer sizes in read mode;
								  second body index < 0 indicates a single-body constraint (H2 == NULL);
								  p1[0] < 0 or p2[0] < 0 indicate dense matrices in read mode */
			int *contact,                          /* 1 for contacts; 0 otherwise => in such case 'H' above corresponds to G */
			double *friction,                      /* friction coefficients */
			double *guess,                         /* initiall guess (3-component for contacts, 1-component otherwise); can be NULL */
			double *solution,                      /* solution (3- or 1-component as above); can be NULL */
			double *point,                         /* spatial point (3-component); can be NULL */
			double *base);                         /* spatial base (9-component for contacts; 3-component otherwise); can be NULL */

/* get global form in read mode;
 * return 1 on success, 0 otherwise */
int DAT3DFC_Global (DAT3DFC *dat,
                    int *n, double **M, int **pM, int **iM, /* compressed rows */
		    int *m, double **H, int **pH, int **iH,
		    int *p, double **G, int **pG, int **iG,
		    double **f,
		    double **b,
		    double **w,
		    double **friction,
		    double **guess,                          /* contact reactions (3-component) followed by equality constraint reactions (1-component); can be NULL */
		    double **solution,                       /* -||-; can be NULL */
		    double **points,                         /* spatial points (3-component); can be NULL */
		    double **bases);                         /* spatial bases (9-component); can be NULL */

/* get local form in read mode;
 * return 1 on success, 0 otherwise */
int DAT3DFC_Local (DAT3DFC *dat,
                   int blocked,                            /* if 1 get compressed blocked row matrices and use block indexing;
							      otherwise get copmpressed row matrices and use component indexing */
                   int *n, double **W, int **pW, int **iW, /* 3x3 blocks if blocked */
		   int *m, double **S, int **pS, int **iS, /* 3x1 blocks if blocked */
		   int *p, double **R, int **pR, int **iR, /* not blocked */
		   double **q,
		   double **s,
		   double **friction,
		   double **guess,
		   double **solution,
		   double **points,
		   double **bases);

/* get accuracy of a candidate solution in read mode */
double DAT3DFC_Accuracy (DAT3DFC *dat, MEA3DFC measure, double *solution);

/* close file;
 * return 1 on success, 0 otherwise */
int DAT3DFC_Close (DAT3DFC *dat);

#endif
