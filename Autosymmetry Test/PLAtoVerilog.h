#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// procedures to start and stop the ABC framework
void   Abc_Start();
void   Abc_Stop();

// procedures to get the ABC framework and execute commands in it
typedef struct Abc_Frame_t_ Abc_Frame_t;

Abc_Frame_t * Abc_FrameGetGlobalFrame();
int    Cmd_CommandExecute( Abc_Frame_t * pAbc, const char * sCommand );

/**
 * @function printVerilog
 * @brief converts pla inputfile in verilog outputfile
 * @param inputfile pla input file name
 * @param outfile verilog output file name
 * @return -1 if any errors occour, 1 otherwise
 */
int printVerilog(char* inputfile, char* outputfile);