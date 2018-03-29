#!/bin/bash
if [ "_$1" = "_" ];then
 echo "I need an argument for what my backend is." >&2
 exit 1
fi
cat camera.pos <(stdbuf -oL $1 < p | ./list_to_cubes.sh) | hackvr USER | ./action_to_target.sh > p
