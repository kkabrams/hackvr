#!/bin/sh

#mabe not. might be buggier.
#cat board p1out | ncat -lp 1050 > p1in &
#cat board p2out | ncat -lp 1051 > p2in &
echo listening in port 1050 for player 1 and 1051 for player 2

mknod pin p
mknod p1out p
mknod p2out p

cat board p1out | ncat -lp 1050 > pin &
cat board p2out | ncat -lp 1051 > pin &

cat pin | ./game.sh | tee p1out p2out

rm pin
rm p1out
rm p2out
