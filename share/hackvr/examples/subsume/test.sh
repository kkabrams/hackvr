#!/bin/bash
cat arms.hackvr

# these start all straight-up, rotate counter clockwise as rotation increases, and
# at these rates of rotation they should all be curled in on themselves at the bottom.
while true;do
  printf "base rotate 0 0 +1\n"
  printf "mid  rotate 0 0 +2\n"
  printf "end  rotate 0 0 +3\n"
  sleep .1
done
