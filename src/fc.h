/* 
 * fc.h
 * ------------------------------------------------------
 * a simpliest possible interface idea
 * ------------------------------------------------------
 * Authors:
 * --------
 * 2010, Tomasz Koziara
 */

#ifndef __fc__
#define __fc__

typedef struct fc FC; /* private structure */

typedef enum fc_merit {MERIT_1, MERIT_2} FC_MERIT; /* merit functions */

/* open for writing;
 * returns NULL on failure */
FC* FC_Write (const char *path);

/* open for reading;
 * returns NULL on failure */
FC* FC_Read (const char *path);

/* read/write global form;
 * returns 1 on success, 0 otherwise */
int FC_Global (FC *fc,
               int *n, double **M, int **pM, int **iM, /* compressed rows (values, row pointers, column indices) */
               int *m, double **H, int **pH, int **iH,
               int *p, double **G, int **pG, int **iG,
               double **f,
               double **b,
               double **w,
               double **friction,                       /* 'm' friction coefficients for */
               double **guess,                          /* 'm' contact reactions (3-component) followed by 'p' equality constraint reactions (1-component); can be NULL */
               double **solution,                       /* -||-; can be NULL */
               double **points,                         /* spatial points (3-component); can be NULL */
               double **bases);                         /* spatial bases (9-component); can be NULL */

/* read/write local form;
 * returns 1 on success, 0 otherwise */
int FC_Local (FC *fc,
              int blocked,                            /* 1 => compressed block row matrices and use block indexing assumed; otherwise normal CSR */
              int *n, double **W, int **pW, int **iW, /* 3x3 blocks if blocked */
              int *m, double **V, int **pV, int **iV, /* 3x1 blocks if blocked */
              int *p, double **R, int **pR, int **iR, /* not blocked */
              double **q,
              double **s,
              double **friction,
              double **guess,
              double **solution,
              double **points,
              double **bases);

/* calculate merit function */
double FC_Merit (FC *fc, FC_MERIT merit, double *solution);

/* close file;
 * returns 1 on success, 0 otherwise */
int FC_Close (FC *fc);

#endif
