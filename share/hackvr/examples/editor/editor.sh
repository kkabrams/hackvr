#!/bin/sh
cat p | hackvr $USER \
  | stdbuf -oL tr -s ' ' \
  | grep --line-buffered "^[^ ][^ ]* action " \
  | stdbuf -oL cut '-d ' -f3 \
  | xargs -L1 printf "epoch control %s\n" > p
