#!/bin/bash
cat <(sort -R ../meshes/female_basemesh1_2.hackvr) \
 <(echo "woman move 10 0 -50") \
 <(sort -R ../meshes/female_basemesh1_2.hackvr | sed 's/woman/woman2/g') \
 <(echo "woman2 move -10 0 -200") \
 <(sort -R ../meshes/female_basemesh1_2.hackvr | sed 's/woman/woamn3/g') | ./slowcat 10000 | ./hackvr_opengl $USER
#../tools/obj2hackvr.pl woman ../meshes/female_basemesh1_2.obj | ./hackvr_opengl epoch
