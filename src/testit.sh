#!/bin/bash
cat <(../tools/obj2hackvr2.pl woman ../meshes/female_basemesh1_2.obj) \
 <(echo "woman move 10 0 0") \
 <(../tools/obj2hackvr2.pl woman2 ../meshes/female_basemesh1_2.obj) \
 <(echo "woman2 move -10 0 0") \
 <(../tools/obj2hackvr2.pl woman3 ../meshes/female_basemesh1_2.obj) | ./hackvr $USER
#../tools/obj2hackvr.pl woman ../meshes/female_basemesh1_2.obj | ./hackvr epoch
