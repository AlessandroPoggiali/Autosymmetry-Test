#include <PLAparser.h>
#include <string.h>

#define MAX_LEN 512

/**
 * @function transformPla
 * @brief writes to a temporary file a transformation of the original 
 *        inputfile which includes alpha variables
 * @param inputfile .pla original file
 * @return -1 if any errors occour, 1 otherwise
 */
int transformPla(char* inputfile){
	char tmp[MAX_LEN];
	FILE *fin, *fout;
	fin = fopen(inputfile, "r"); // opens .pla input file
	fout = fopen(TMPFILE, "w");  // opens .pla output file
	if(fin == NULL){
		fprintf(stderr, "Error opening file %s\n", inputfile);
		return -1;
	}
	if(fout == NULL){
		fprintf(stderr, "Error opening file %s\n", TMPFILE);
		return -1;
	}
	int ninput = 0, noutput = 0, i, k=0;
	char *str;
	int done = 0;
	// reads inputs and outputs number 
	while(!done && (fgets(tmp, MAX_LEN, fin) != NULL)){
		if(tmp[0] == '.') { // keyword
			if(tmp[1] == 'i'){ // input keyword
				str = strtok(tmp, " ");
				str = strtok(NULL, " ");
				ninput = strtol(str, NULL, 10);
				num_x = ninput;
				fprintf(fout, ".i %d\n", ninput*2);
				fprintf(stdout, "input: %d\n", ninput);
			}
			else if(tmp[1] == 'o'){ // output keyword
				str = strtok(tmp, " ");
				str = strtok(NULL, " ");
				noutput = strtol(str, NULL, 10);
				fprintf(fout, ".o %d\n", noutput);
				fprintf(stdout, "output: %d\n", noutput);
				done = 1;
			}
		}
	}
	if(ninput<=0 || noutput<=0)
		return -1;
	int inputreaded = 0;  // inputs currently read
	int outputreaded = 0; // outputs currently read
	int readInput = 1;	  // determines input reading cycle 
	int readOutput = 0;   // determines output reading cycle 
	char* input = (char*)calloc(num_x+1, sizeof(char));
	char* output = (char*)calloc(noutput+1, sizeof(char));
	int multiline = 0;	  // denotes whether output must be printed in another line
	char delimit[] = "|"; // possible delimiters between input and output 
	if(num_x+noutput>MAX_LEN)
		multiline = 1;
	while(fscanf(fin, "%s", tmp) > 0){
		if(tmp[0] == '.'){
			fgets(tmp, MAX_LEN, fin);
			continue;
		}
		if(strlen(tmp) > num_x+noutput){ 
			// tmp contains input and output, separated by a special character
			char *tmpstr;
            char* p1 = strtok_r(tmp, delimit, &tmpstr); // gets input
			strcpy(input, p1);
            char* p2 = strtok_r(NULL, "\n", &tmpstr);  // gets output
			strcpy(output, p2);
			// writes input to the new pla file
			for(i=0; i<num_x*2; i++){
				if(i%2==0){
					fprintf(fout, "-");
				}
				else{
					fprintf(fout, "%c", input[k]);
					k++;
				}
			}
			k = 0;
			if(multiline)
				fprintf(fout, "\n");
			else fprintf(fout, " ");
			// writes output to the new pla file
			for(i=0; i<noutput; i++)
				fprintf(fout, "%c", output[i]);
			fprintf(fout, "\n");
			readInput = 0;
			readOutput = 0;
		}
		else if(readInput){
			inputreaded += strlen(tmp);
			if(inputreaded < num_x){	// input on multiple lines
				input = strcat(input, tmp);
			}
			else{
				input = strcat(input, tmp);
				readInput = 0;
				readOutput = 1;
				inputreaded = 0;
				// writes input to the new pla file
				for(i=0; i<num_x*2; i++){
					if(i%2==0){
						fprintf(fout, "-");
					}
					else{
						fprintf(fout, "%c", input[k]);
						k++;
					}
				}
				k = 0;
				if(multiline)
					fprintf(fout, "\n");
				else fprintf(fout, " ");
				input[0] = '\0';
			}
		}
		else if(readOutput){
			outputreaded += strlen(tmp);
			if(outputreaded < noutput){	// output on multiple lines
				output = strcat(output, tmp);
			}
			else{
				output = strcat(output, tmp);
				readInput = 1;
				readOutput = 0;
				outputreaded = 0;
				// writes output to the new pla file
				for(i=0; i<noutput; i++)
					fprintf(fout, "%c", output[i]);
				fprintf(fout, "\n");
				output[0] = '\0';
			}
		}
	}
	fflush(fout);
	fclose(fin);
	fclose(fout);
	free(input);
	free(output);
	return 1; 
}

/**
 * @function read_product
 * @brief reads the product from a line in PLA
 * @param input char array containing the product
 * @return BDD for the product
 */
DdNode* read_product(char* input){
	DdNode* f;
	DdNode* tmpNode;
	DdNode* var;
	f = Cudd_ReadOne(manager);
	Cudd_Ref(f);
	// reads the product
	for(int i=num_in-1; i>=0; i--){
		if(input[i] == '-' || input[i] == '4' || input[i] == '~')
			continue;
		var = Cudd_bddIthVar(manager, i); // creates node
		//Cudd_Ref(var);
		if(input[i] == '0')
			// computes BDD resulting of AND of the prior BDD with the complement of the new variable 
			tmpNode = Cudd_bddAnd(manager, Cudd_Not(var), f); 
		else
			// computes BDD resulting of AND of the prior BDD with the new variable 
			tmpNode = Cudd_bddAnd(manager, var, f); 
		Cudd_Ref(tmpNode);
		//Cudd_RecursiveDeref(manager, var);
		Cudd_RecursiveDeref(manager, f);		  
		f = tmpNode; 
	}
	return f;
}

/**
 * @function build_bdd
 * @brief inserts the product f in BDD
 * @param f BDD for the product 
 * @param output char array containing output for the product
 */
void build_bdd(DdNode* f, char* output){
	// inserts the f in the BDD
	DdNode* tmpNode;
	for(int i = 0; i < num_out; i++){
		if(output[i] == '0') // OFF set
			continue;
		if(output[i] == '1'){	// ON set
			// computes BDD resulting of the OR of the f product with actual BDD
			tmpNode = Cudd_bddOr(manager, f, vectorbdd_F[i]); 
			Cudd_Ref(tmpNode);
			Cudd_RecursiveDeref(manager, vectorbdd_F[i]);
			vectorbdd_F[i] = tmpNode;
		}
	}
	Cudd_RecursiveDeref(manager, f);
}

/**
 * @function parse
 * @brief reads the .pla file containing the Boolean function description and builds BDD
 * @param inputfile .pla file
 * @param init_manager if 1, CUDD manager must be initialized
 * @return -1 if any errors occour, 1 otherwise
 */
int parse(char* inputfile,  int init_manager) {

	char tmp[MAX_LEN];
	int done = 0;
	DdNode *f;
	FILE* PLAFile; 
	PLAFile = fopen(inputfile, "r"); // opens .pla input file
	if(PLAFile == NULL){
		fprintf(stderr, "Error opening file %s\n ", inputfile);
		return -1;
	}
	// reads inputs and outputs number 
	while(!done && fscanf(PLAFile, "%s\n",tmp) != EOF){
		if(tmp[0] == '.') { // keyword
			switch(tmp[1]){
				case 'i': { // input keyword
                    // reads number of function inputs
					fscanf(PLAFile, "%d\n", &num_in);  
					if(init_manager){
						// initializes CUDD manager
						manager = Cudd_Init(num_in, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0); 
					    //Cudd_DisableGarbageCollection(manager);
					}
				}break;
				case 'o':{ // output keyword
					int i;
					// reads number of function outputs 
					fscanf(PLAFile, "%d\n", &num_out); 
					// initializes BDDs vector
					vectorbdd_F = (DdNode **) calloc (num_out, sizeof(DdNode*)); 
					if(vectorbdd_F == NULL)	{
						fprintf(stderr, "INPUT vectorbdd_F: Error in calloc\n");
						Cudd_Quit(manager);
						return -1;	
					}
					// initializes each BDD to constant 0
					for(i=0; i<num_out; i++) {
						vectorbdd_F[i] = Cudd_ReadLogicZero(manager); 
						Cudd_Ref(vectorbdd_F[i]); 
					}
					done = 1;
				}break;
			} 
		} 
	}
	if(num_in<=0 || num_out<=0)
		return -1;
	int readInput = 1;    // determines input reading cycle 
	int readOutput = 0;	  // determines output reading cycle
	int inputreaded = 0;  // inputs currently read
	int outputreaded = 0; // outputs currently read
	char delimit[] = "|"; // possible delimiters between input and output 
	char* input = (char*)calloc(num_in+1,sizeof(char));
	char* output = (char*)calloc(num_out+1,sizeof(char));
	while(fscanf(PLAFile, "%s", tmp) > 0){
		if(strlen(tmp) > num_in + num_out){
			// tmp contains input and output, separated by a special character
			char *tmpstr;
            char* p1 = strtok_r(tmp, delimit, &tmpstr); // gets input
			strcpy(input, p1);
            char* p2 = strtok_r(NULL, "\n", &tmpstr);  // gets output
			strcpy(output, p2);
			f = read_product(input); // reads the product in PLA
			Cudd_Ref(f);
			build_bdd(f, output);    // inserts the product in BDD
			readInput = 0;
			readOutput = 0;
		}
		else if(readInput){
			inputreaded += strlen(tmp);
			if(inputreaded < num_in){ // input on multiple lines
				input = strcat(input, tmp);
			}
			else{
				input = strcat(input, tmp);
				//printf("input: %s\n",input);
				readInput = 0;
				readOutput = 1;
				inputreaded = 0;
				f = read_product(input); // reads the product in PLA
				Cudd_Ref(f);
				input[0] = '\0';
			}
		}
		else if(readOutput){
			outputreaded += strlen(tmp);
			if(outputreaded < num_out){ // output on multiple lines
				output = strcat(output, tmp);
			}
			else{
				output = strcat(output, tmp);
				//printf("output: %s\n",output);
				readInput = 1;
				readOutput = 0;
				outputreaded = 0;
				build_bdd(f, output); // inserts the product in BDD
				output[0] = '\0';
			}
		}
	}
	free(input);
	free(output);
	fclose(PLAFile);
	return 1; 
}


/**
 * @function dumpBdd
 * @brief prints the BDD on a .dot file 
 * @param manager 
 * @param bdd BDD to be printed
 * @param outputfile output file name
 * @param inames array of variable names
 */
void dumpBdd(DdManager *manager, DdNode *bdd, char* outfile, char**inames) {
	FILE *file;
	int i;
	char** onames; // names of onput variables

	if(manager == NULL || bdd == NULL || outfile == NULL)
		return;
	// converts BDD to ADD for display purpose
	bdd = Cudd_BddToAdd(manager, bdd); 
	
	onames = (char **) calloc (num_out, sizeof(char*));
	if(onames == NULL){
		fprintf(stderr, "ERROR: calloc\n");
	}
	for(i=0; i<num_out; i++){
		onames[i] = (char *) calloc (10, sizeof(char));
		sprintf(onames[i],"y%d", i);
	}

	file = fopen(outfile, "w");	
	if(file == NULL)
		return;
	// dumps bdd
	Cudd_DumpDot(manager, 1, &bdd, (char const * const *)inames, (char const * const *)onames, file); 
	// converts ADD to BDD
	bdd = Cudd_addBddPattern(manager, bdd); 

	fclose(file);
	for(i=0; i<num_out; i++)
		free(onames[i]);
	free(onames);
}

/**
 * @function printPla
 * @brief writes the bdd in pla format to outputfile
 * @param outputfile output file name
 * @param bdd BDD to be printed
 * @param n_var Number of bdd variables 
 */
void printPla(char* outputfile, DdNode* bdd, int n_var){
	DdGen* gen;
	int* cube;
	CUDD_VALUE_TYPE value;
	int ssize = 1; // support size, if 0 bdd is costant
	FILE *f = fopen(outputfile, "w"); // opens output file (.pla)
	if(f == NULL){
		fprintf(stderr, "ERROR: opening file %s\n", outputfile);
		exit(-1);
	}
    ssize = Cudd_SupportSize(manager, bdd);			   
	if(ssize == 0){
		// prints input number
		fprintf(f, ".i 1\n"); 
		// prints output number
		fprintf(f, ".o 1\n");	
		fprintf(f, "- 1\n");
	}
	else{
		// prints input number
		fprintf(f, ".i %d\n", ssize); 
		// prints output number
		fprintf(f, ".o 1\n");	
		// gets the variables indices on which bdd depends
		int* support = Cudd_SupportIndex(manager, bdd);	
		// prints each cube on output file
		Cudd_ForeachCube(manager, bdd, gen, cube, value){	    
			for(int i=0; i<n_var; i++){
				if(support[i]){
					if (cube[i] == 2)
						fprintf(f, "%c", '-'); 
					else
						fprintf(f, "%d", cube[i]);
				}
			}
			fprintf(f, " 1\n");
		}
		free(support);
	}
	fclose(f);
}

