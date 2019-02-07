#!/usr/bin/env bash

#to finish out any that might still be listening
nc -z 127.0.0.1 1050
nc -z 127.0.0.1 1051

echo hackvr://$(hostname).$(domainname):1050/ player 1
echo hackvr://$(hostname).$(domainname):1051/ player 2

coproc ./game.sh
tee >(ncat -lp 1050 >&"${COPROC[1]}") >(ncat -lp 1051 >&"${COPROC[1]}") >/dev/null <&"${COPROC[0]}"
