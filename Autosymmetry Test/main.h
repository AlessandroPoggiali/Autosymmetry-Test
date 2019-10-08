#include <stdio.h>
#include <stdlib.h>
#include <cudd.h>

#define TMPFILE "tmp.pla" 
#define PERFORMANCE "performance.txt"

DdManager* manager;      /**< CUDD manager */
int num_in;              /**< number of input variables */
int num_x;               /**< number of x variables */
int num_out;             /**< number of output */
DdNode **vectorbdd_F;    /**< BDD array for output */
char** inames;           /**< variable names array */
