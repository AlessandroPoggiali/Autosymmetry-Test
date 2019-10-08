#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

int main(int argc, char* argv[]){
    if(argc != 5){
        fprintf(stderr, "usage: %s pla of f(.pla), output, verilog of fk(.v), reductions(.re)\n", argv[0]);
		exit(-1);
	}
    FILE *verilog_fk = fopen(argv[3], "r");
    FILE *reqns = fopen(argv[4], "r");
    int output = strtol(argv[2], NULL, 10);
    char* pla_filename = basename(argv[1]);
    if(verilog_fk == NULL || reqns == NULL){
        fprintf(stderr, "File not found, function %s_%d is probably not autosymmetric\n", pla_filename, output);
        exit(-1);
    }
    // parses reduction equations file
    char tmp[128];
    int first = 1;
    int n_var = 0;
    int rows = 0;
    // counts number of rows and number of variables
    while(fgets(tmp, 128, reqns)!=NULL){
        if(first){
            n_var = strlen(tmp)-1;
            first = 0;
        }
        rows++;
    }
    if(rows == 0){
        fprintf(stderr, "No reduction equations for function %s_%d: it is probably constant\n", pla_filename, output);
        return 0;
    }
    // matrix for reduction equations
    int **m = (int**)malloc(sizeof(int*)*rows); 
    for(int i=0; i<rows; i++)
        m[i] = (int*)malloc(sizeof(int)*n_var);
    rewind(reqns);
    int k = 0;
    // reads reduction equations
    while(fgets(tmp, 128, reqns)!=NULL){
        for(int i=0; i<strlen(tmp)-1; i++){
            if(tmp[i] != ' ')
                m[k][i] = tmp[i] - '0';
        }
        k++;
    }
    fclose(reqns);
    char* new_filename = malloc(sizeof(char)*(strlen(argv[1])+15)); 
    sprintf(new_filename, "%s_newV_%d.v", argv[1], output);
    FILE *verilog_f = fopen(new_filename, "w");
    // writes new verilog
    fprintf(verilog_f, "module \\%s  (\n",argv[1]);
    fprintf(verilog_f, "    ");
    // writes input and output variables
    for(int i=0; i<n_var; i++){
        fprintf(verilog_f, "x%d, ",i);
    }
    fprintf(verilog_f, "\n    z0  );\n");
    fprintf(verilog_f, "  input ");
    for(int i=0; i<n_var; i++){
        if(i<n_var-1)
            fprintf(verilog_f, "x%d, ",i);
        else fprintf(verilog_f, "x%d;\n",i);
    }
    fprintf(verilog_f, "  output z0;\n");
    fprintf(verilog_f, "  wire ");
    // write new intermediate y variables
    for(int i=0; i<rows; i++){
        if(rows>10 && i<10)
            fprintf(verilog_f, "y0%d, ", i);
        else if(i == rows-1)
            fprintf(verilog_f, "y%d", i);
        else 
            fprintf(verilog_f, "y%d, ", i);
    }
    // reads verilog of fk and writes old intermediate variables
    int checkwire = 0;
    int len;
    while(fgets(tmp, 128, verilog_fk)!=NULL){
        len = strlen(tmp);
        char* str = strtok(tmp, " ");
        if(!checkwire && !strcmp(str, "wire")){
            char* wire = strtok(NULL, "\n");
            fprintf(verilog_f, ", %s\n", wire);
            checkwire = 1;
        }
        else if(checkwire && strcmp(str, "assign")){
            char* wire = strtok(NULL, "\n");
            if(wire!=NULL)
                fprintf(verilog_f, "%s %s\n",str, wire);
            else fprintf(verilog_f, "%s\n",str);
        }
        else if(!strcmp(str, "assign"))
            break;
    }
    if(!checkwire){
        fprintf(verilog_f, ";\n");
    }
    // writes reduction equations in new verilog file
    for(int i=0; i<rows; i++){
        first = 1;
        for(int j=0; j<n_var; j++){
            if(m[i][j]==1){
                if(first){
                    if(rows>10 && i<10)
                        fprintf(verilog_f, "  assign y0%d = x%d", i, j);
                    else fprintf(verilog_f, "  assign y%d = x%d", i, j);
                    first = 0;
                }
                else fprintf(verilog_f, " ^ x%d", j);
            }
        }
        fprintf(verilog_f, ";\n");
    }
    // writes the rest of f_k
    fseek(verilog_fk, -len, SEEK_CUR);
    char current_char = 0;
    while((current_char  = fgetc(verilog_fk)) != EOF){                                            
        if(current_char == 'x') {
            fprintf(verilog_f, "%c", 'y'); 
        }
        else fprintf(verilog_f, "%c", current_char);
    }   
}