#!/bin/sh
stdbuf -oL ./filebrow.sh < p | ./list_to_cubes.sh | ../hackvr epoch | ./action_to_target.sh > p
