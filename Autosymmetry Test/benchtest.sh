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
planame="$(basename -- $filename)"

./main $filename
for (( i=0; i < $noutput; i++ ))
    do
        echo "testing $planame"\_"$i"
        ./reverse restrictions/$planame\_res\_$i.pla reductionEqns/$planame\_$i.re $filename $i
        if [[ $? -ne 0 ]]; then
            exit 1
        fi
    done 
