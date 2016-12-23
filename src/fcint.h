/* FCLIB Copyright (C) 2011--2016 FClib project
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
