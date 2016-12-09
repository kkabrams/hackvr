#!/bin/sh
traceroute -n $1 | tail -n+2 | sed 's/^ *//g' | cut '-d ' -f3 | whob | egrep '^IP|^Lat|^Long' | cut '-d ' -f2- | paste - - - | tr '\t' ' '
