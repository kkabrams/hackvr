#!/bin/bash
#give a list of files.
#wait for a selection on stdin
#cd or start that file
#repeat.
while true;do
  echo
  echo .* * | tr ' ' '\n'
  read -r selection
  if [ -f "$selection" ];then
    xdg-open "$selection" #good enough?
  fi
  if [ -d "$selection" ];then
    cd "$selection"
  fi
done
