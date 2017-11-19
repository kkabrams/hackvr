#!/bin/bash
cat <(cat meshes/female_basemesh1_2.hackvr) <(./makerotate.sh woman | ./src/slowcat 10000) | ./hackvr $USER
