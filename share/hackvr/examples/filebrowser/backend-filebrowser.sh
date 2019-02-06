#!/bin/bash
#give a list of files.
#wait for a selection on stdin
#cd or start that file
#repeat.
while true;do
#not sure why this needs to be printed to show up every time.
  echo ..
  find . -maxdepth 1 #| tr ' ' '\n' #wut? no?
  read -r selection
  if [ -f "$selection" ];then
    xdg-open "$selection" #good enough?
  fi
  if [ -d "$selection" ];then
    cd "$selection"
  fi
  echo
done
