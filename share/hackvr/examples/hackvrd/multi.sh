#!/bin/bash
printf 'user: %s joined multi\n' "$USER" >> /var/log/hackvr.all
cat /var/cache/hackvr/world_history <(echo 'root export *') \
	| egrep -v '^#|^[^ ][^ ]*  *quit' \
	| grep -v 'move [a-z]' \
	| /usr/local/bin/hackvr_headless 2>/dev/null \
	| sed 's/^root_//g'
tail -f -n0 /var/cache/hackvr/world_history \
	| grep --line-buffered -v '^'"$USER"' ' &
tee -a /var/cache/hackvr/world_history >/dev/null
