#!/bin/bash
#give a list of files.
#wait for a selection on stdin
#cd or start that file
#repeat.
while true;do
#not sure why this needs to be printed to show up every time.
  ps | tail -n+2
  read -r selection
  echo $selection
  echo
done
