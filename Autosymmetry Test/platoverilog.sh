#!/bin/bash

function usage () 
{
    echo "use $0 filename noutput"
}

if [[ $# -ne 2 ]]; then
    usage
    exit 1
fi

filename="$1"
noutput="$2"

for (( i=0; i < $noutput; i++ ))
    do
        ./buildVerilog $filename $i
        echo "building verilog for function $filename"\_"$i "
    done 
for (( i=0; i < $noutput; i++ ))
    do
        rm $filename\_$i.pla
    done 
