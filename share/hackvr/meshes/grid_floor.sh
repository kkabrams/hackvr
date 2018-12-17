#!/bin/bash
cat <(for i in $(seq 1 16);do for j in $(seq 1 16);do echo floor addshape 5 2 $i 0 $j $i 0 $[j+1];echo floor addshape 5 2 $i 0 $j $[i+1] 0 $j;done; done) <(echo -e "floor scaleup 8\nepoch export floor\nepoch quit\n") | ../src/hackvr_headless epoch | sed "s/\.0*//g"
