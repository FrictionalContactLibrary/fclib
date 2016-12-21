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

/*!\file fclib.h
 * -----------------------------------------
 * frictional contact library interface
 * -----------------------------------------
 *
 * This C API provides functions to read and write Frictional contact problemes in HDF5 format
 * Two kind of problem are considered
 *  Given
 * <ul>
 *   <li> a symmetric positive semi--definite  matrix \f${W} \in {\mathrm{I\!R}}^{m \times m} \f$ </li>
 *   <li> a vector \f$ {q} \in {\mathrm{I\!R}}^m\f$</li>
 *   <li> a vector of coefficients of friction \f$\mu \in{\mathrm{I\!R}}^{n_c}\f$</li>
 *</ul>
 * the local FC problem  is to find two vectors \f$u\in{\ensuremath{\mathrm{I\!R}}}^m\f$,
 * the relative local velocity and \f$r\in {\ensuremath{\mathrm{I\!R}}}^m\f$,
 * the contact forces denoted by \f$\mathrm{FC}(W,q,\mu)\f$  such that
 * \f{eqnarray*}{
 * \begin{cases}
 *    \hat u = W r + q +\left[
 *      \left[\begin{array}{c}
 *          \mu^\alpha \|u^\alpha_T\|\         \
 *         0 \                                  \
 *         0
 *        \end{array}\right]^T, \alpha = 1 \ldots n_c
 *    \right]^T \\ \                                \
 *    C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
 * \end{cases}
 * \f}
 * where the Coulomb friction cone for a  contact \f$\alpha\f$ is defined by
 * \f{eqnarray*}{
 * \label{eq:CCC}
 * C_{\mu^\alpha}^{\alpha}  = \{r^\alpha, \|r^\alpha_T \| \leq \mu^\alpha |r^\alpha_N| \}
 *\f}
 * and the set \f$C^{\alpha,\star}_{\mu^\alpha}\f$ is its dual.
 * We are also dealing with global  FC problem defined by
 * Given
 * <ul>
 *    <li> a symmetric positive definite matrix \f${M} \in {\mathrm{I\!R}}^{n \times n}\f$</li>
 *   <li> a vector \f$ {f} \in {\mathrm{I\!R}}^n\f$,</li>
 *   <li> a matrix  \f${H} \in {\mathrm{I\!R}}^{n \times m}\f$</li>
 *   <li> a matrix  \f${G} \in {\mathrm{I\!R}}^{n \times p}\f$</li>
 *    <li> a vector \f$w \in {\mathrm{I\!R}}^{m}\f$,</li>
 *    <li> a vector \f$b \in {\mathrm{I\!R}}^{p}\f$,</li>
 *   <li> a vector of coefficients of friction \f$\mu \in {\mathrm{I\!R}}^{n_c}\f$
 *</ul>
 * the Global Mixed 3DFC problem  is to find four vectors \f$ {v} \in {\mathrm{I\!R}}^n\f$, \f$u\in{\mathrm{I\!R}}^m\f$, \f$r\in {\mathrm{I\!R}}^m\f$ and \f$\lambda \in {\mathrm{I\!R}}^p\f$ denoted by \f$\mathrm{GM3DFC}(M,H,G,w,b,\mu)\f$  such that
 * \f{eqnarray*}{
 * \begin{cases}
 *   M v = {H} {r} + G\lambda + {f} \\ \        \
 *   G^T v +b =0 \\ \                           \
 *   \hat u = H^T v + w +\left[
 *     \left[\begin{array}{c}
 *       \mu \|u^\alpha_T\|\                   \
 *       0 \                                    \
 *       0
 *     \end{array}\right]^T, \alpha = 1 \ldots n_c
 * \right]^T \\ \                                    \
 *   C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
 * \end{cases}
 * \f}
 *
 */

#ifndef _fclib_h_
#define _fclib_h_

/**\struct  fclib_info fclib.h
 * This structure allows the user to enter a  problem information  as a title, a short description and known mathematical properties of the problem
 */

struct fclib_info
{
  /** title of the problem*/
  char *title;
  /** short decription of the problem*/
  char *description;
  /** known properties of the problem (existence, uniqueness, ...)*/
  char *math_info;
};

/**\struct  fclib_matrix_info fclib.h
 * This structure allows the user to enter a description for a given matrix (comment, conditionning, determinant, rank.) if they are known.
 */
struct fclib_matrix_info  /* matrix information */
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
struct fclib_matrix   /*  */
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

/**\struct fclib_global fclib.h
 * The global frictional contact problem defined by
 *
 * Given
 * <ul>
 *    <li> a symmetric positive definite matrix \f${M} \in {\mathrm{I\!R}}^{n \times n}\f$</li>
 *   <li> a vector \f$ {f} \in {\mathrm{I\!R}}^n\f$,</li>
 *   <li> a matrix  \f${H} \in {\mathrm{I\!R}}^{n \times m}\f$</li>
 *   <li> a matrix  \f${G} \in {\mathrm{I\!R}}^{n \times p}\f$</li>
 *    <li> a vector \f$w \in {\mathrm{I\!R}}^{m}\f$,</li>
 *    <li> a vector \f$b \in {\mathrm{I\!R}}^{p}\f$,</li>
 *   <li> a vector of coefficients of friction \f$\mu \in {\mathrm{I\!R}}^{n_c}\f$
 *</ul>
 * the Global Mixed 3DFC problem  is to find four vectors \f$ {v} \in {\mathrm{I\!R}}^n\f$, \f$u\in{\mathrm{I\!R}}^m\f$, \f$r\in {\mathrm{I\!R}}^m\f$ and \f$\lambda \in {\mathrm{I\!R}}^p\f$ denoted by \f$\mathrm{GM3DFC}(M,H,G,w,b,\mu)\f$  such that
 * \f{eqnarray*}{
 * \begin{cases}
 *   M v = {H} {r} + G\lambda + {f} \\ \        \
 *   G^T v +b =0 \\ \                           \
 *   \hat u = H^T v + w +\left[
 *     \left[\begin{array}{c}
 *       \mu \|u^\alpha_T\|\                   \
 *       0 \                                    \
 *       0
 *     \end{array}\right]^T, \alpha = 1 \ldots n_c
 * \right]^T \\ \                                    \
 *   C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
 * \end{cases}
 * \f}
 * where the Coulomb friction cone for a  contact \f$\alpha\f$ is defined by
 * \f{eqnarray*}{
 * \label{eq:CCC}
 * C_{\mu^\alpha}^{\alpha}  = \{r^\alpha, \|r^\alpha_T \| \leq \mu^\alpha |r^\alpha_N| \}
 *\f}
 * and the set \f$C^{\alpha,\star}_{\mu^\alpha}\f$ is its dual.
 */
struct fclib_global
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
/**\struct fclib_local fclib.h
 * The local frictional contact problem defined by
 *
 * given
 * <ul>
 *   <li> a positive semi--definite matrix  \f${W} \in {\mathrm{I\!R}}^{m \times m}\f$</li>
 *   <li> a matrix  \f${V} \in {\mathrm{I\!R}}^{m \times p}\f$</li>
 *   <li> a matrix  \f${R} \in {\mathrm{I\!R}}^{p \times p}\f$</li>
 *   <li> a vector \f$q \in {\mathrm{I\!R}}^{m}\f$,</li>
 *   <li> a vector \f$s \in {\mathrm{I\!R}}^{p}\f$,</li>
 *   <li> a vector of coefficients of friction \f$\mu \in {\mathrm{I\!R}}^{n_c}\f$
 * </ul>
 * the  Mixed 3DFC problem  is to find three vectors \f$u\in{\mathrm{I\!R}}^m\f$, \f$r\in {\mathrm{I\!R}}^m\f$ and \f$\lambda \in {\mathrm{I\!R}}^p\f$ denoted by \f$\mathrm{M3DFC}(R,V,W,q,s,\mu)\f$  such that
 * \f{eqnarray*}\label{eq:lcp1}
 *\begin{cases}
 *   V^T {r} + R \lambda + s = 0 \\ \           \
 *   \hat u = W {r}    + V\lambda  + q +\left[
 *     \left[\begin{array}{c}
 *       \mu^\alpha \|u^\alpha_T\|\            \
 *       0 \                                    \
 *       0
 *     \end{array}\right]^T, \alpha = 1 \ldots n_c
 * \right]^T \\ \                                    \
 *   C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
 * \end{cases}
 * \f}
 * where the Coulomb friction cone for a  contact \f$\alpha\f$ is defined by
 * \f{eqnarray*}{
 * \label{eq:CCC}
 * C_{\mu^\alpha}^{\alpha}  = \{r^\alpha, \|r^\alpha_T \| \leq \mu^\alpha |r^\alpha_N| \}
 * \f}
 * and the set \f$C^{\alpha,\star}_{\mu^\alpha}\f$ is its dual.
 */
struct fclib_local
{
  /** the matrix W (see mathematical description below)*/
  struct fclib_matrix *W;
  /** the matrix V (see mathematical description below)*/
  struct fclib_matrix *V;
  /** the matrix R (see mathematical description below)*/
  struct fclib_matrix *R;
  /** the vector \f$\mu\f$ of coefficient of friction (see mathematical description below)*/
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

/** \struct fclib_solution fclib.h
 *  A solution or a guess for the frictional contact problem.
 *
 * This structure allows to store a solution vector of a guess vector for the
 * various frictional contact problems.
 */
struct fclib_solution /* solution data */
{
  /** global velocity (or position/displacement for quasi-static problems) solution vector */
  double *v;
  /** local velocity  (or position/displacement for quasi-static problems) solution vector */
  double *u;
  /** local contact forces  (or impulses) solution vector */
  double *r;
  /** multiplier for equlity constraints (\f$\lambda\f$) solution vector */
  double *l;
};

/** \enum  fclib_merit
 * MERIT_1 is a implementation of the merit function based on the natural map for a SOCCP
 */
enum fclib_merit {MERIT_1, MERIT_2} ; /* merit functions */

/** write global problem;
 * return 1 on success, 0 on failure */
int fclib_write_global (struct fclib_global *problem, const char *path);

/** write local problem;
 * return 1 on success, 0 on failure */
int fclib_write_local (struct fclib_local *problem, const char *path);

/** write solution;
 * return 1 on success, 0 on failure */
int fclib_write_solution (struct fclib_solution *solution, const char *path);

/** write initial guesses;
 * return 1 on success, 0 on failure */
int fclib_write_guesses (int number_of_guesses,  struct fclib_solution *guesses, const char *path);

/** read global problem;
 * return problem on success; NULL on failure */
struct fclib_global* fclib_read_global (const char *path);

/** read local problem;
 * return problem on success; NULL on failure */
struct fclib_local* fclib_read_local (const char *path);

/** read solution;
 * return solution on success; NULL on failure */
struct fclib_solution* fclib_read_solution (const char *path);

/** read initial guesses;
 * return vector of guesses on success; NULL on failure;
 * output numebr of guesses in the variable pointed by 'number_of_guesses' */
struct fclib_solution* fclib_read_guesses (const char *path, int *number_of_guesses);

/** calculate merit function for a global problem */
double fclib_merit_global (struct fclib_global *problem, enum fclib_merit merit, struct fclib_solution *solution);

/** calculate merit function for a local problem */
double fclib_merit_local (struct fclib_local *problem, enum fclib_merit merit, struct fclib_solution *solution);

/** delete global problem */
void fclib_delete_global (struct fclib_global *problem);

/** delete local problem */
void fclib_delete_local (struct fclib_local *problem);

/** delete solutions or guesses */
void fclib_delete_solutions (struct fclib_solution *data, int count);

/** create and set attributes of tyoe int in info */
int fclib_create_int_attributes_in_info(const char *path, const char * attr_name,
                                        int attr_value);

#endif /* _fclib_h_ */
