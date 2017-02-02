#!/bin/bash
#give a list of files.
#wait for a selection on stdin
#cd or start that file
#repeat.
server=gopher.hacking.allowed.org
port=70
selection=/
while true;do
#not sure why this needs to be printed to show up every time.
  printf '%s\n' "$selection" | ncat "$server" "$port" | tee /dev/stderr | grep -v ^i | cut -f2
### need to ask user for input in the form of a pop-up window if the selected target was of type 7
  read -r selection
  echo
done
