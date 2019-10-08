#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <PLAtoVerilog.h>

int main(int argc, char* argv[]){
    if(argc != 3){
        fprintf(stderr, "usage: %s inputfile (.pla) output\n", argv[0]);
		exit(-1);
	}
    int output = strtol(argv[2], NULL, 10); // output to be considered
    char newpla[strlen(argv[1])+8];
    sprintf(newpla, "%s_%d.pla", argv[1], output);
    // temporary PLA file for a single-output function
    FILE *new = fopen(newpla, "w");
    if(new == NULL){
       fprintf(stderr, "ERROR: opening file %s\n", newpla);
        exit(-1); 
    }
    // PLA original file
    FILE *old = fopen(argv[1], "r");
    if(old == NULL){
       fprintf(stderr, "ERROR: opening file %s\n", argv[1]);
        exit(-1); 
    }
    char tmp[256];
    char* str;
    int in = 0;
    int out = 0;
    char delimit[] = "|\t\n\r "; // possible delimiters between input and output 
    int readInput = 1;
    char* input;
    char *tmpstr;
    int k = 0;
    // reads inputs and outputs number 
	while((fgets(tmp, 256, old) != NULL)){
		if(tmp[0] == '.') { // keyword
			if(tmp[1] == 'i'){ // input keyword
				str = strtok(tmp, " ");
				str = strtok(NULL, " ");
				in = strtol(str, NULL, 10);
                // prints input number in new file
				fprintf(new, ".i %d\n", in); 
			}
			else if(tmp[1] == 'o'){ // output keyword
				str = strtok(tmp, " ");
				str = strtok(NULL, " ");
				out = strtol(str, NULL, 10);
				fprintf(new, ".o 1\n");
                if(output>out-1){
                    fprintf(stderr, "ERROR: wrong output\n");
                    exit(-1);
                }
                if(in > out)
                    input = (char*)calloc(in+1,sizeof(char));
                else input = (char*)calloc(out+1,sizeof(char));
			}
		}
        else{
            if(strlen(tmp) >= in+out){
                char* p1 = strtok_r(tmp, delimit, &tmpstr); // gets input
                char* p2 = strtok_r(NULL, "\n", &tmpstr);  // gets output
                // prints input line in new file
                if(p2[output] != '-' && (p2[output] - '0')){
                    fprintf(new, p1);
                    fprintf(new, " 1\n");
                    k++;
                }
            }
            else{
                if(readInput){
                    // tmp contains input
                    char* p1 = strtok_r(tmp, "\r", &tmpstr);
                    strcpy(input, p1);
                    readInput = 0;
                }
                else{
                    // tmp contains output
                    if(tmp[output] != '-' && (tmp[output] - '0')){
                        fprintf(new, input);
                        fprintf(new, " 1\n");
                        k++;
                    }
                    readInput = 1;
                }
            }
        }
	}
    if(!k){
        for(int i=0; i<in; i++)
            fprintf(new, "-");
        fprintf(new, " 1");
    }
    fclose(new);
    fclose(old);

    char verilog[strlen(newpla)+2];
    char filename[100];
    strcpy(filename, newpla);
    sprintf(verilog, "%s.v", newpla);
    // converts pla to verilog 
    if(printVerilog(filename, verilog) == -1){
        fprintf(stderr, "ERROR in printVerilog\n");
        exit(-1);
    }
}