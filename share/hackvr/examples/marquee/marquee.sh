#!/bin/bash
set -o pipefail

pi=3 #lol

from_cam=150

elevation=10
while printf "marquee rotate 0 +-2 0\n";do
  sleep .1
done &

tail -F ~/.local/var/cache/music/nowplaying.txt 2>/dev/null | while read -r line;do
  printf "%s deleteallexcept %s\n" "$USER" "$USER"
  printf "nowplaying flatten\n"
  printf "%s\n" "$line" >&2
  chars="$(printf "%s" "$line" | wc -c | tr -cd '[0-9]')"
  char_width=5 #I think..
  rot=$[ 360 / $chars ]
  diameter=$[ $chars * $char_width / $pi ]
  printf "%s\n" "$line" | fold -w1 | while read -r char;do
    printf "%s\n" "$char" | makelabel.sh nowplaying 0 0 0 | offsetshape.sh 0 0 "-${diameter}"
    printf "nowplaying rotate 0 %d 0\n" "-$rot"
    printf "nowplaying flatten\n"
  done
  printf "marquee assimilate nowplaying\n"
  printf "marquee move 0 %s %s\n" "$elevation" "$[$diameter + $from_cam]" #this won't work before the generation loop unless I can flatten on certain parts. 
done
