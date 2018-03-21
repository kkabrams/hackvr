#!/bin/bash
cat camera.pos <(stdbuf -oL $1 < p | ./list_to_cubes.sh) | hackvr USER | ./action_to_target.sh > p
