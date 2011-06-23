/* 
 * fclib.c
 * ----------------------------------------------
 * frictional contact library implementation ? :)
 */

#include <stdlib.h>
#include "fclib.h"

struct fclib_root
{
  struct fclib_global *global_problem;
  struct fclib_local  *local_problem;
  struct fclib_solution  *solution;
  int number_of_guesses;
  struct fclib_solution  *guess; /* pointer to the first guess */
};

/* write global problem;
 * return 1 on success, 0 on failure */
int fclib_write_global (struct fclib_global *problem, const char *path)
{
  return 0;
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
