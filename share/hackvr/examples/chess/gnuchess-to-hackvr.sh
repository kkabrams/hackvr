#!/bin/sh
e=$1
tee /dev/stderr | while read first second third;do
  if [ "_$first" = "_feature" ];then
    printf "# just gnuchess saying what features it has: %s %s %s\n" "$first" "$second" "$third" >&2
  elif printf "%s\n" "$first" | grep '^[0-9][0-9]*\.$' >/dev/null 2>&1;then
    #we made a successful move.
    ### how to tell hackvr to move the piece now? $second should contain what the move was.
    printf "# Hey! Hackvr! do this move: %s\n" "$second" >&2
    if [ "_$second" = "_..." ];then #computer/other person made a move.
      move=$third
    else
      move=$second
    fi
      from=$(printf "%s\n" "$move" | fold -w1 | head -n2 | tr -d '\n')
      to=$(printf "%s\n" "$move" | fold -w1 | tail -n+3 | head -n2 | tr -d '\n')
      x=$(echo "a b c d e f g h" | tr ' ' '\n' | grep -n "$(printf "%s\n" "$to" | fold -w1 | head -n1)" | cut -d: -f1)
      y=$(printf "%s\n" "$to" | fold -w1 | tail -n1)
      magic=$(printf "%s\n" "$move" | fold -w1 | tail -n+5 | tr -d '\n')
### if the board is on the wall we need to use x and y
#    printf "piece_%s move %s %s %s\n" "$from" "$x" "$y" "$e" | tee /dev/stderr
### the board is on the floor atm. use x and z
    printf "piece_%s move %s %s %s\n" "$from" "$x" "$e" "$y" | tee /dev/stderr

    printf "#delete the old piece_%s group\n"
    printf "epoch deletegroup piece_%s\n" "$to"
    printf "epoch renamegroup piece_%s piece_%s\n" "$from" "$to" | tee /dev/stderr
  else
    #something else.
    printf "oops. something else happened. let's see: %s %s %s\n" "$first" "$second" "$third" >&2
  fi
done
