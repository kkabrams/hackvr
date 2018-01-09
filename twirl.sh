#!/bin/bash
cat <(cat $1 | sed 's/^[^ ]* /mesh /g') <(./makerotate.sh mesh | ./src/slowcat 10000) | ./src/hackvr $USER
