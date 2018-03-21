#!/bin/bash
cat <(printf "%s move 0 0 -270\n" "$USER") \
    <(wget http://hacking.allowed.org/cgi-bin/map_hackvr.cgi -qO- 2>/dev/null) \
    /dev/stdin \
 | slowcat 0 \
 | hackvr_x11 "$USER" \
 | grep --line-buffered action \
 | stdbuf -oL cut '-d ' -f3 \
 | xargs -r -L1 printf "AS%s\n" \
 | tee /dev/stderr \
 | xargs -r -L1 ./xcmd.sh whois -h hacking.allowed.org
