/* 
 * fcint.h
 * -----------------------------------------
 * frictional contact library internals
 */

#ifndef _fcint_h_
#define _fcint_h_

/* useful macros */
#define ASSERT(Test, ...)\
  do {\
  if (! (Test)) { fprintf (stderr, "%s: %d => ", __FILE__, __LINE__);\
    fprintf (stderr, __VA_ARGS__);\
    fprintf (stderr, "\n"); exit (1); } } while (0)

#define IO(Call) ASSERT ((Call) >= 0, "ERROR: HDF5 call failed")
#define MM(Call) ASSERT ((Call), "ERROR: out of memory")

#endif /* _fcint_h_ */
