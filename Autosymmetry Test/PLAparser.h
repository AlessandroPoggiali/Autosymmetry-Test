#include <stdio.h>
#include <stdlib.h>
#include <cudd.h>
#include <main.h>

/**
 * @function transformPla
 * @brief writes to a temporary file a transformation of the original
 *        inputfile which includes alpha variables
 * @param inputfile .pla original file
 * @return -1 if any errors occour, 1 otherwise
 */
int transformPla(char* inputfile);

/**
 * @function parse
 * @brief reads the .pla file containing the Boolean function description and builds BDD
 * @param inputfile .pla file
 * @param init_manager if 1, CUDD manager must be initialized
 * @return -1 if any errors occour, 1 otherwise
 */
int parse(char* inputfile,  int init_manager); 

/**
 * @function dumpBdd
 * @brief prints the BDD on a .dot file 
 * @param manager 
 * @param bdd BDD to be printed
 * @param outputfile output file name
 * @param inames array of variable names
 */
void dumpBdd(DdManager *manager, DdNode *bdd, char* outputfile, char** inames);

/**
 * @function printPla
 * @brief writes the bdd in pla format to outputfile
 * @param outputfile output file name
 * @param bdd BDD to be printed
 * @param n_var number of bdd variables 
 */
void printPla(char* outputfile, DdNode* bdd, int n_var);

