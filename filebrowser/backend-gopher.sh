#!/bin/bash
#give a list of files.
#wait for a selection on stdin
#cd or start that file
#repeat.
server=192.168.0.2
port=70
selection=/
while true;do
#not sure why this needs to be printed to show up every time.
  printf '%s\n' "$selection" | ncat "$server" "$port"
  read -r selection
  echo
done
