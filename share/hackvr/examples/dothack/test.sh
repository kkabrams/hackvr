#!/usr/bin/env bash
nonblocktail \
	<(./chaosgate.py) \
	<(echo gate move 0 0 150) \
	<(while true;do for i in $(seq 0 5 360);do echo "gate rotate 0 $i 0";sleep .1;done;done) \
  | hackvr_open
