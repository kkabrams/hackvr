#!/bin/bash
if [ "_$1" = "_" ];then
 echo "I need a backend script passed as first argument." >&2
 exit 1
fi
stdbuf -oL $1 < p | stdbuf -oL tr '\n' ' ' | sed -u 's/$/_/' | stdbuf -oL tr '_' '\n' | xargs -L1 zenity --list --column file > p
