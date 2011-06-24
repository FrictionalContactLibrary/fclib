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

/* calculate merit function for a global problem */
double fclib_merit_global (struct fclib_global *problem, enum fclib_merit merit, struct fclib_solution *solution)
{
  return 0; /* TODO */
}

/* calculate merit function for a local problem */
double fclib_merit_local (struct fclib_local *problem, enum fclib_merit merit, struct fclib_solution *solution)
{
  return 0; /* TODO */
}
