#!/usr/bin/env bash
cat <(for i in $(seq 1 32);do
        for j in $(seq 1 32);do
          echo floor addshape 2 4 $i 0 $j $i 0 $[j+1] $[i+1] 0 $[j+1] $[i+1] 0 $j
#          echo floor addshape 2 4 $i 0 $j $[i+1] 0 $j $[i+1] 0 $[j+1] $i 0 $[j+1]
        done
      done) \
    <(echo -e "floor scaleup 8\nepoch export floor\nepoch quit\n") \
 | hackvr_headless $USER \
 | sed "s/\.0*//g"
