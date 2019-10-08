#include <PLAparser.h>
#include <PLAtoVerilog.h>
#include <math.h>
#include <time.h>
#include <main.h>
#include <equations.h>
#include <string.h>
#include <libgen.h>


/**
 * @function vectorSpace
 * @brief Computes the vector space of function f, represented by the BDD g1
 * @param g1 BDD for f
 * @return BDD for the vector space of f
 */
DdNode* vectorSpace(DdNode* g1) {
    int i;
    DdNode* g2;      // BDD for f(x1 xor a1,...,xn xor an)
    DdNode* g3;      // BDD for g1 <=> g2
    DdNode* g4;      // BDD for forall(x1,...,xn).g3
    DdNode* tmp;
    // builds g2
    g2 = g1;
	Cudd_Ref(g2);
    for(i=num_in-1; i>=0; i--){
        if(i%2!=0){ // for xi variables
            DdNode* xi = Cudd_bddIthVar(manager, i);  	// gets xi node
            DdNode* ai = Cudd_bddIthVar(manager, i-1);  // gets ai node
			// computes node for xi xor alpha
			DdNode* xor = Cudd_bddXor(manager, xi, ai); 
            Cudd_Ref(xor);
			// substitutes the old variables in g2 with new xored variable
            tmp = Cudd_bddCompose(manager, g2, xor, i); 
			if(tmp == NULL){
                fprintf(stderr, "ERROR: building Lf\n");
                exit(-1);
            }
			Cudd_Ref(tmp);
			Cudd_RecursiveDeref(manager, xor);
            g2 = tmp;
        }
	}
	// builds g3: a <=> b is equivalent to a XNOR b
	g3 = Cudd_bddXnor(manager, g1, g2); 
	// builds g4
	g4 = g3;
	g4 = Cudd_bddUnivAbstract(manager, g4, Cudd_Support(manager, g1));
    return g4;
}


/**
 * @function canonicalVariables
 * @brief computes the set of the canonical variables of the vector space Lf
 * @param Lf vector space in BDD form 
 * @param i index of a canonical variable in canonical variables array
 * @param n max number of canonical variables
 * @param cv canonical variables array
 * @return cv canonical variables array
 */
int* canonicalVariables(DdNode* Lf, int i, int n, int* cv){
	int j;
	DdNode *h0, *h1;
	if (i < n) {
        if(Lf == Cudd_ReadLogicZero(manager))
            return cv;
        else if(Lf == Cudd_ReadOne(manager)){
            for(j=i; j<n; j++)
                cv[j] = j;
            return cv; // return {xi,...,xn}
        }
        else{
            h0 = Cudd_bddRestrict(manager, Lf, Cudd_Complement(Cudd_bddIthVar(manager, i*2)));
            h1 = Cudd_bddRestrict(manager, Lf, Cudd_bddIthVar(manager, i*2));
            if (h0 != Cudd_ReadLogicZero(manager) && h1 == Cudd_ReadLogicZero(manager))
                return canonicalVariables(h0, i+1, n, cv);
            else if (h0 == Cudd_ReadLogicZero(manager) && h1 != Cudd_ReadLogicZero(manager))
                return canonicalVariables(h1, i+1, n, cv);
            else{
                cv[i] = i;
                for(j=i+1; j<n; j++)
                    cv[j] = n+1;
				// return {xi} U canonicalVariables(h1, i+1, n, cv)
                return canonicalVariables(h1, i+1, n, cv); 
            }
        }
	}
    return cv;
}

/**
 * @function restrictionFunction
 * @brief computes the BDD for the restriction of f 
 * @param f function f in BDD form 
 * @param cv canonical variables array
 * @return BDD for the restriction fk of f
 */
DdNode* restrictionFunction(DdNode* f, int *cv){
	DdNode* g;
	g = f;
	for(int i=0; i<num_x; i++)
		if (cv[i] == i)
            g = Cudd_bddRestrict(manager, g, Cudd_Complement(Cudd_bddIthVar(manager, i*2+1)));
	return g;
}

/**
 * @function reductionEquation
 * @brief computes the reduction equations
 * @param h vector space in BDD form 
 * @param i index of variable 
 * @param n number of variables
 * @param eq_man equation set manager
 * @return a pointer to the equation set containing the reduction equations
 */
Eqns_t* reductionEquations(DdNode* h, int i, int n, EQ_manager *eq_man){
	if(h == Cudd_ReadOne(manager) || h == Cudd_ReadLogicZero(manager) || i>=n){
		Eqns_t *empty = new_eqns(eq_man);
		return empty;
	}
	DdNode* h0 = Cudd_bddRestrict(manager, h, Cudd_Complement(Cudd_bddIthVar(manager, i*2)));  
	DdNode* h1 = Cudd_bddRestrict(manager, h, Cudd_bddIthVar(manager, i*2));	  			   
	if (h1 == Cudd_ReadLogicZero(manager) && h0 == Cudd_ReadOne(manager)){
		Eqns_t *eqns = new_eqns(eq_man);
		Eqn_t* eqn = new_eqn(n);
		add_var(eqn, i, 1);
		add_eqn(eqns, eqn);
		return eqns;
	}
	else if (h0 == Cudd_ReadLogicZero(manager) && h1 == Cudd_ReadOne(manager)){
		Eqns_t *eqns = new_eqns(eq_man);
		Eqn_t* eqn = new_eqn(n);
		add_var(eqn, i, 0);
		add_eqn(eqns, eqn);
		return eqns;
	}
	else if(h1 == Cudd_ReadLogicZero(manager) && (h0 != Cudd_ReadLogicZero(manager) || h0 != Cudd_ReadOne(manager))){	
		Eqns_t *S0 = reductionEquations(h0, i+1, n, eq_man);
		Eqn_t* eqn = new_eqn(n);
		add_var(eqn, i, 1);
		add_eqn(S0, eqn);
		return S0;
	}
	else if (h0 == Cudd_ReadLogicZero(manager) && (h1 != Cudd_ReadLogicZero(manager) || h1 != Cudd_ReadOne(manager))){
		Eqns_t *S1 =  reductionEquations(h1, i+1, n, eq_man);
		Eqn_t* eqn = new_eqn(n);
		add_var(eqn, i, 0);
		add_eqn(S1, eqn);
		return S1;
	}
	else{
		Eqns_t *S0 = reductionEquations(h0, i+1, n, eq_man);
		Eqns_t *S1 = reductionEquations(h1, i+1, n, eq_man);
		Eqns_t *S = new_eqns(eq_man);
		list_t l = S0->head;
		while(l!=NULL){
			Eqn_t *eqn = new_eqn(n);
			memcpy(eqn, l, sizeof(Eqn_t)); 
			if(!isInEqns(S1, eqn))
				add_var(eqn, i, 0);
			add_eqn(S, eqn);
			l = l->next;
		}
		return S;
	}
}

int main(int argc, char* argv[]){

	DdNode *f;         // BDD for function f
    DdNode *Lf;        // BDD for vector space Lf 
	DdNode *fk;        // BDD for restriction fk

	double degree;     // autosymmetry degree of f
	double sum = 0;    // sum of autosymmetry degrees
    int i, k;
	int *cv; 	       // array of canonical variables

    clock_t init, end; // to measure time
    double duration;

	if(argc!=2){
		fprintf(stderr, "usage: %s inputfile(.pla)\n", argv[0]);
		exit(-1);
	}
    // reads .pla file and writes another .pla including alpha variables 
    if(transformPla(argv[1]) == -1){
        fprintf(stderr, "ERROR: trasforming pla file\n");
        exit(-1);
    }
    // parses new .pla file and sets global variables
	if(parse(TMPFILE, 1) == -1){
		fprintf(stderr, "ERROR: parsing pla file\n");
        exit(-1);
	}
	k = 0;
	// variables name array in this order [a0, x0, a1, x1, ... an, xn]
	inames = (char **) calloc(num_in, sizeof(char*)); 
	for(i=0; i<num_in; i++){
		inames[i] = (char *)calloc(5, sizeof(char));
		if(i%2 == 0){
            sprintf(inames[i], "a%d", k);
            k++;
        }
        else 
			sprintf(inames[i], "x%d", i-k);
	}
	cv = (int*)calloc(num_x, sizeof(int)); 
	if(cv == NULL){
		fprintf(stderr, "ERROR: calloc\n");
		exit(-1);
	}
	init = clock();
	char* pla_name = basename(argv[1]); // extracts pla file name from path
	for(int j=0; j<num_out; j++){
		EQ_manager *eq_man = EQ_manager_init(); // equation set manager
		f = vectorbdd_F[j];
		char f_filename[strlen(pla_name)+15];
		sprintf(f_filename, "dot/%s_%d.dot", pla_name, j);
		dumpBdd(manager, f, f_filename, inames); // dumps BDD for f
		k = 0;
		// computes vector space for f
		Lf = vectorSpace(f);						  
		char Lf_filename[strlen(pla_name)+20];
		sprintf(Lf_filename, "dot/%s_lf_%d.dot", pla_name, j);
		dumpBdd(manager, Lf, Lf_filename, inames); // dumps BDD of vector space
		degree = Cudd_CountMinterm(manager, Lf, num_x); 
		if(degree == 0) continue;
		degree = log(degree)/log(2);  // autosymmetry degree 
		printf("Autosymmetry degree of output %d: %.2f\n", j, degree);
		fflush(NULL);
		sum += degree;
		if(degree>=1 && degree<num_x){
			for(i=0; i<num_x; i++)
				cv[i] = num_x+1;      // resets canonical variables array 
			// computes canonical variables of Lf
			cv = canonicalVariables(Lf, 0, num_x, cv);	
			// computes restriction fk of f
			fk = restrictionFunction(f, cv);		    
			char dot_filename[strlen(pla_name)+20];
			sprintf(dot_filename, "dot/%s_res_%d.dot", pla_name, j);
			dumpBdd(manager, fk, dot_filename, inames); // dumps restriction fk
			char res_filename[strlen(pla_name)+30];
			char verilog_filename[strlen(pla_name)+30];
			sprintf(res_filename, "restrictions/%s_res_%d.pla", pla_name, j);
			sprintf(verilog_filename, "restrictions/%s_res_%d.v", pla_name, j);
			printPla(res_filename, fk, num_in);	
			if(printVerilog(res_filename, verilog_filename) == -1){
				fprintf(stderr, "ERROR: printing verilog file\n");
				exit(-1);
			}
			// computes reduction equations of Lf
			Eqns_t* re = new_eqns(eq_man);			 
			re = reductionEquations(Lf, 0, num_x, eq_man);
			complements_all(re); 	  // complements the reduction equations 							
			FILE *fre;
			char re_filename[strlen(pla_name)+30];
			sprintf(re_filename, "reductionEqns/%s_%d.re", pla_name, j);
			fre = fopen(re_filename, "w");
			print_equations(fre, re); // prints reduction equations 			   
			fclose(fre);
			EQ_manager_quit(eq_man);
		}
	}
	end = clock(); 
	duration = (double)(end - init)/CLOCKS_PER_SEC;
	degree = sum/num_out; 
	printf("k: %.2f\n", degree);
	printf("Time: %f\n", duration); // prints time performance
	// prints performance in file
	FILE *p = fopen(PERFORMANCE, "a");
	if(p == NULL){
		fprintf(stderr, "Error opening file %s\n ", PERFORMANCE);
		return -1;
	}
	fprintf(p, "%s ", argv[1]);
	fprintf(p, "%d ", num_x);
	fprintf(p, "%d ", num_out);
	fprintf(p, "%.2f ", degree);
	fprintf(p, "%f", duration);
	fprintf(p, "\n");
    // free memory
	Cudd_Quit(manager);
    for(i=0; i<num_in; i++){
        free(inames[i]);
	}
	free(cv);
    free(inames);
    return 0;
}
