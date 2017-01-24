#!/bin/bash
cat <(./tools/dat2hackvr.pl earth <(zcat earth/world_110m.txt)) ./world_camera <(./traceroute_to_dat.sh $1 | cut '-d ' -f2- | awk '{print $2,$1}' | tail -n+2 | ./tools/dat2hackvr.pl route /dev/stdin) /dev/stdin | ./hackvr epoch
