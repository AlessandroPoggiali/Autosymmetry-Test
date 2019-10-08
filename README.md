# Autosymmetry-Test

This project provide the Autosymmetry Test of a given Boolean function.

## Installation

You must have CUDD (https://github.com/ivmai/cudd) and ABC-Berkeley (https://github.com/berkeley-abc/abc) libraries installed on your system.

## Compile

Use the Makefile to compile the project.

The command 

```bash
make
```
will create the following executable files for the autosymmetry test:

- main: executes the autosymmetry test of a given function.
- reverse: checks whether the autosymmetry test of a given function is correct.
- buildVerilog: builds the Verilog representation of a given single-output function.
- buildNewVerilog: builds an alternative representation in Verilog of an autosymmetric function.

