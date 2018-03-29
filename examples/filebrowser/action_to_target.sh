#!/bin/sh
#this script gets the action lines that hackvr outputs and turns them into as normal of a string as possible
#for the backend script to use.
#so hex encoded strings need to be unencoded before they leave here.
#the hex encoded strings don't have a newline at the end. we need to add one. see xargs printf "%s0a"
grep --line-buffered ^USER \
  | stdbuf -oL cut '-d ' -f2- \
  | grep --line-buffered ^action \
  | stdbuf -oL cut '-d ' -f2- \
  | xargs -L1 printf "%s0a\n" \
  | stdbuf -oL xxd -r -p
