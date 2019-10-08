#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cudd.h>
#include <PLAparser.h>
#include <libgen.h>

int main(int argc, char* argv[]){
    if(argc!=5){
		fprintf(stderr, "usage: %s fk (.pla) reductions (.re) f (.pla) output\n", argv[0]);
		exit(-1);
	}
    int out = strtol(argv[4], NULL, 10);
    int nfk = 0;   // number of variables in fk
    int n = 0;     // number of variables in reduction equations
    int rows = 0;  // number of rows
    // parses reductions file
    FILE *re = fopen(argv[2], "r");
    if(re == NULL){
        fprintf(stderr, "File %s not found, function %s_%d is probably not autosymmetric\n", argv[2], basename(argv[3]), out);
        return 0;
    }
    char tmp[128];
    int first = 1;
    // counts number of rows and number of variables
    while(fgets(tmp, 128, re)!=NULL){
        if(first){
            n = strlen(tmp)-1;
            first = 0;
        }
        rows++;
    }
    if(rows == 0){
        fprintf(stderr, "No reduction equations for function %s_%d: it is probably constant\n", basename(argv[3]), out);
        return 0;
    }
    // matrix for reduction equations
    int **m = (int**)malloc(sizeof(int*)*rows); 
    for(int i=0; i<rows; i++)
        m[i] = (int*)malloc(sizeof(int)*n);
    rewind(re);
    int k = 0;
    // reads reduction equations
    while(fgets(tmp, 100, re)!=NULL){
        for(int i=0; i<strlen(tmp)-1; i++){
            if(tmp[i] != ' ')
                m[k][i] = tmp[i] - '0';
        }
        k++;
    }
    fclose(re);
    // parses restriction file
    if(parse(argv[1], 1) == -1){
		fprintf(stderr, "ERROR: parsing pla file\n");
        exit(-1);
	}
    nfk = num_in;
    DdNode* f = vectorbdd_F[0];
    DdNode** newvar = malloc(n*sizeof(DdNode*));
    // creates n variables
    for(int i=0; i<n; i++)
        newvar[i] = Cudd_bddNewVar(manager);
    // adds reduction equations to reduced bdd
    for(int i=0; i<num_in; i++){
        DdNode* xor = Cudd_ReadLogicZero(manager);
        for(int j=0; j<n; j++){
            if(m[i][j])
                xor = Cudd_bddXor(manager, xor, newvar[j]);
        }
        f = Cudd_bddCompose(manager, f, xor, i);
    }
    inames = (char **) calloc(n+num_in, sizeof(char*)); 
	for(int i=0; i<nfk; i++){
		inames[i] = (char *)calloc(10, sizeof(char));
		sprintf(inames[i], "oldx%d", i);
	}
    for(int i=nfk; i<n+nfk; i++){
		inames[i] = (char *)calloc(10, sizeof(char));
		sprintf(inames[i], "x%d", i-num_in);
	}
    dumpBdd(manager, f, "dot/reverse.dot",  inames); // dumps bdd
    printPla("reverse.pla", f, n+nfk); // prints function in .pla format
    // parses original pla file
    if(parse(argv[3], 0) == -1){
		fprintf(stderr, "ERROR: parsing pla file\n");
        exit(-1);
	}
    if(out > num_out){
        fprintf(stderr, "Wrong output\n");
        exit(-1);
    }
    DdNode* original = vectorbdd_F[out]; // gets original BDD
    int *permut = (int*)malloc(sizeof(int)*(num_in+rows));
    for(int i=0; i<num_in+rows; i++){
        if(i<rows)
            permut[i] = rows+i+1;
        else permut[i] = i-rows;
    }
    // permutes variables in f to have same ordering that in original
    f = Cudd_bddPermute(manager, f, permut);
    // equivalence test
    DdNode* result = Cudd_bddXnor(manager, f, original);
    if(result == Cudd_ReadOne(manager)){
        Cudd_Quit(manager);
        fprintf(stdout, "TEST PASSED!\n");
        return 0;
    }
    else{
        Cudd_Quit(manager);
        fprintf(stdout, "TEST FAILED!\n");
        return -1;
    }
}