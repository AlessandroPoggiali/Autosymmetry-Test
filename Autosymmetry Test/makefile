CC		=  gcc
CX 		= g++
AR              =  ar
CFLAGS	        += -Wall -pedantic -g
ARFLAGS         =  rvs
INCLUDES	= -I.
LDFLAGS 	= -L.
LIBS		= -lcudd -lm -ggdb -ldl -lpthread -lreadline -labc
TARGETS		= main reverse buildVerilog buildNewVerilog

OBJECTS		= PLAparser.o PLAtoVerilog.o equations.o

INCLUDE_FILES   = main.h PLAparser.h PLAtoVerilog.h equations.h\

.PHONY: all clean cleanall install
.SUFFIXES: .c .h

%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

all		: $(TARGETS)

main: main.o libautosymtest.a $(INCLUDE_FILES)
	$(CX) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LIBS)
reverse: reverse.o libautosymtest.a $(INCLUDE_FILES)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LIBS)
PLAtoVerilog: PLAtoVerilog.o
	$(CX) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
buildVerilog: buildVerilog.o PLAtoVerilog.o PLAtoVerilog.h
	$(CX) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LIBS)
buildNewVerilog: buildNewVerilog.o
	$(CX) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LIBS)

libautosymtest.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

clean		: 
	rm -f $(TARGETS)

cleanall	: clean
	\rm -f *.o *~ libautosymtest.a dot/* reductionEqns/* restrictions/* tmp.pla reverse.pla bench/DivisiInDifficolta/facili/*.v 
install:
	mkdir dot 
	mkdir reductionEqns
	mkdir restrictions
	



