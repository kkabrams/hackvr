#!/bin/sh

#mabe not. might be buggier.
#cat board p1out | ncat -lp 1050 > p1in &
#cat board p2out | ncat -lp 1051 > p2in &
cat board p1out | ncat -lp 1050 > p1in &
cat board p2out | ncat -lp 1051 > p1in &

cat p1in | ./game.sh | tee p1out p2out
