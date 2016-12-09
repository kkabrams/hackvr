#!/bin/sh
whob | egrep '^IP|^Lat|^Long' | cut '-d ' -f2- | paste - - - | tr '\t' ' ' | awk '{print $1, $3, $2, 0, $3, $2, 0, $3, $2, 0}' | xargs -L1 printf '%s addtriangle %s %s %s %s %s %s %s %s %s\n'
