#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <PLAtoVerilog.h>

/**
 * @function printVerilog
 * @brief converts pla inputfile in verilog outputfile
 * @param inputfile pla input file name
 * @param outfile verilog output file name
 * @return -1 if any errors occour, 1 otherwise
 */
int printVerilog(char* inputfile, char* outputfile){
	Abc_Frame_t * pAbc;
    char Command[1000];
    // start the ABC framework
    Abc_Start();
    pAbc = Abc_FrameGetGlobalFrame();
	sprintf(Command, "read_pla %s", inputfile);
	// reads pla file
    if(Cmd_CommandExecute(pAbc, Command)){
        fprintf(stdout, "Cannot execute command \"%s\".\n", Command);
        return -1;
    }
	sprintf(Command, "strash");
	if(Cmd_CommandExecute(pAbc, Command)){
        fprintf(stdout, "Cannot execute command \"%s\".\n", Command);
        return -1;
    }
	// tries to reduce the size
	for(int i=0; i<2; i++){
		sprintf(Command, "dc2");
		if(Cmd_CommandExecute(pAbc, Command)){
			fprintf(stdout, "Cannot execute command \"%s\".\n", Command);
			return -1;
		}
	}
	// writes verilog file
	sprintf(Command, "write_verilog %s", outputfile);
    if(Cmd_CommandExecute(pAbc, Command)){
        fprintf(stdout, "Cannot execute command \"%s\".\n", Command);
        return -1;
    }
	// stop the ABC framework
    Abc_Stop();
	return 1;
}
