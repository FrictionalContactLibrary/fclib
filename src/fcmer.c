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
#include "csparse.h"


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

