#!/bin/sh
stdbuf -oL $1 < p | ./list_to_cubes.sh | ../hackvr epoch | ./action_to_target.sh > p
